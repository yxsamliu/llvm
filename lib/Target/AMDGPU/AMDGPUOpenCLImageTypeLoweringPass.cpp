//===-- AMDGPUOpenCLImageTypeLoweringPass.cpp -----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
/// \file
/// This pass resolves calls to OpenCL image attribute, image resource ID and
/// sampler resource ID getter functions.
///
/// Image attributes (size and format) are expected to be passed to the kernel
/// as kernel arguments immediately following the image argument itself,
/// therefore this pass adds image size and format arguments to the kernel
/// functions in the module. The kernel functions with image arguments are
/// re-created using the new signature. The new arguments are added to the
/// kernel metadata with kernel_arg_type set to "image_size" or "image_format".
/// Note: this pass may invalidate pointers to functions.
///
/// Resource IDs of read-only images, write-only images and samplers are
/// defined to be their index among the kernel arguments of the same
/// type and access qualifier.
//===----------------------------------------------------------------------===//

#include "AMDGPU.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Transforms/Utils/Cloning.h"

using namespace llvm;

namespace {

StringRef GetImageSizeFunc =         "llvm.OpenCL.image.get.size";
StringRef GetImageFormatFunc =       "llvm.OpenCL.image.get.format";
StringRef GetImageResourceIDFunc =   "llvm.OpenCL.image.get.resource.id";
StringRef GetSamplerResourceIDFunc = "llvm.OpenCL.sampler.get.resource.id";

StringRef ImageSizeArgMDType =   "__llvm_image_size";
StringRef ImageFormatArgMDType = "__llvm_image_format";

// Name of kernel argument metadata.
StringRef KernelArgMDNodeNames[] = {
  "kernel_arg_addr_space",
  "kernel_arg_access_qual",
  "kernel_arg_type",
  "kernel_arg_base_type",
  "kernel_arg_type_qual"};
const unsigned NumKernelArgMDNodes = 5;

typedef SmallVector<Metadata *, 8> MDVector;

// Structure containing all kernel argument metadata for a kernel.
// Each element of ArgVector contains operands for a specific kernel argument
// metadata whose name corresponds to element of KernelArgMDNodeNames with
// the same array index.
struct KernelArgMD {
  MDVector ArgVector[NumKernelArgMDNodes];
};

} // end anonymous namespace

static inline bool
IsImageType(StringRef TypeString) {
  return TypeString == "image2d_t"
      || TypeString == "image3d_t";
}

static inline bool
IsSamplerType(StringRef TypeString) {
  return TypeString == "sampler_t";
}

static Function *
GetFunctionFromMDNode(MDNode *Node) {
  if (!Node)
    return nullptr;

  size_t NumOps = Node->getNumOperands();
  if (NumOps != NumKernelArgMDNodes + 1)
    return nullptr;

  auto F = mdconst::dyn_extract<Function>(Node->getOperand(0));
  if (!F)
    return nullptr;

  // Sanity checks.
  size_t ExpectNumArgNodeOps = F->arg_size() + 1;
  for (size_t i = 0; i < NumKernelArgMDNodes; ++i) {
    MDNode *ArgNode = dyn_cast_or_null<MDNode>(Node->getOperand(i + 1));
    if (ArgNode->getNumOperands() != ExpectNumArgNodeOps)
      return nullptr;
    if (!ArgNode->getOperand(0))
      return nullptr;

    // FIXME: It should be possible to do image lowering when some metadata
    // args missing or not in the expected order.
    MDString *StringNode = dyn_cast<MDString>(ArgNode->getOperand(0));
    if (!StringNode || StringNode->getString() != KernelArgMDNodeNames[i])
      return nullptr;
  }

  return F;
}

static StringRef
AccessQualFromMD(Function &F, unsigned ArgIdx) {
  return cast<MDString>(F.getMetadata(KernelArgMDNodeNames[1])->getOperand(
    ArgIdx))->getString();
}

static StringRef
ArgTypeFromMD(Function &F, unsigned ArgIdx) {
  return cast<MDString>(F.getMetadata(KernelArgMDNodeNames[3])->getOperand(
    ArgIdx))->getString();
}

// Get metadata for an argument and return them as a vector.
// \param ArgInd argument index.
// \return vector containing metadata for the given argument.
static MDVector
GetArgMD(Function &F, unsigned ArgInd) {
  MDVector Res;
  for (unsigned i = 0; i < NumKernelArgMDNodes; ++i)
    Res.push_back(F.getMetadata(KernelArgMDNodeNames[i])->getOperand(ArgInd));
  return Res;
}

// Put metadata for an argument into KernelArgMD.
// Each element of member
// \param MD is the container for all kernel argument metadata.
// \param V contains metadata for an argument.

static void
PushArgMD(KernelArgMD &MD, const MDVector &V) {
  assert(V.size() == NumKernelArgMDNodes);
  for (unsigned i = 0; i < NumKernelArgMDNodes; ++i) {
    MD.ArgVector[i].push_back(V[i]);
  }
}

namespace {

class AMDGPUOpenCLImageTypeLoweringPass : public ModulePass {
  static char ID;

  LLVMContext *Context;
  Type *Int32Type;
  Type *ImageSizeType;
  Type *ImageFormatType;
  SmallVector<Instruction *, 4> InstsToErase;

  bool replaceImageUses(Argument &ImageArg, uint32_t ResourceID,
                        Argument &ImageSizeArg,
                        Argument &ImageFormatArg) {
    bool Modified = false;

    for (auto &Use : ImageArg.uses()) {
      auto Inst = dyn_cast<CallInst>(Use.getUser());
      if (!Inst) {
        continue;
      }

      Function *F = Inst->getCalledFunction();
      if (!F)
        continue;

      Value *Replacement = nullptr;
      StringRef Name = F->getName();
      if (Name.startswith(GetImageResourceIDFunc)) {
        Replacement = ConstantInt::get(Int32Type, ResourceID);
      } else if (Name.startswith(GetImageSizeFunc)) {
        Replacement = &ImageSizeArg;
      } else if (Name.startswith(GetImageFormatFunc)) {
        Replacement = &ImageFormatArg;
      } else {
        continue;
      }

      Inst->replaceAllUsesWith(Replacement);
      InstsToErase.push_back(Inst);
      Modified = true;
    }

    return Modified;
  }

  bool replaceSamplerUses(Argument &SamplerArg, uint32_t ResourceID) {
    bool Modified = false;

    for (const auto &Use : SamplerArg.uses()) {
      auto Inst = dyn_cast<CallInst>(Use.getUser());
      if (!Inst) {
        continue;
      }

      Function *F = Inst->getCalledFunction();
      if (!F)
        continue;

      Value *Replacement = nullptr;
      StringRef Name = F->getName();
      if (Name == GetSamplerResourceIDFunc) {
        Replacement = ConstantInt::get(Int32Type, ResourceID);
      } else {
        continue;
      }

      Inst->replaceAllUsesWith(Replacement);
      InstsToErase.push_back(Inst);
      Modified = true;
    }

    return Modified;
  }

  bool replaceImageAndSamplerUses(Function *F, MDNode *KernelMDNode) {
    uint32_t NumReadOnlyImageArgs = 0;
    uint32_t NumWriteOnlyImageArgs = 0;
    uint32_t NumReadWriteImageArgs = 0;
    uint32_t NumSamplerArgs = 0;

    bool Modified = false;
    InstsToErase.clear();
    for (auto ArgI = F->arg_begin(); ArgI != F->arg_end(); ++ArgI) {
      Argument &Arg = *ArgI;
      StringRef Type = ArgTypeFromMD(*F, Arg.getArgNo());

      // Handle image types.
      if (IsImageType(Type)) {
        StringRef AccessQual = AccessQualFromMD(*F, Arg.getArgNo());
        uint32_t ResourceID;
        if (AccessQual == "read_only") {
          ResourceID = NumReadOnlyImageArgs++;
        } else if (AccessQual == "write_only") {
          ResourceID = NumWriteOnlyImageArgs++;
        } else if (AccessQual == "read_write") {
          ResourceID = NumReadWriteImageArgs++;
        } else {
          llvm_unreachable("Wrong image access qualifier.");
        }

        Argument &SizeArg = *(++ArgI);
        Argument &FormatArg = *(++ArgI);
        Modified |= replaceImageUses(Arg, ResourceID, SizeArg, FormatArg);

      // Handle sampler type.
      } else if (IsSamplerType(Type)) {
        uint32_t ResourceID = NumSamplerArgs++;
        Modified |= replaceSamplerUses(Arg, ResourceID);
      }
    }
    for (unsigned i = 0; i < InstsToErase.size(); ++i) {
      InstsToErase[i]->eraseFromParent();
    }

    return Modified;
  }

  Function *
  addImplicitArgs(Function *F) {
    bool Modified = false;

    FunctionType *FT = F->getFunctionType();
    SmallVector<Type *, 8> ArgTypes;

    // Metadata operands for new MDNode.
    KernelArgMD NewArgMDs;

    // Add implicit arguments to the signature.
    for (unsigned i = 0; i < FT->getNumParams(); ++i) {
      ArgTypes.push_back(FT->getParamType(i));
      MDVector ArgMD = GetArgMD(*F, i);
      PushArgMD(NewArgMDs, ArgMD);

      if (!IsImageType(ArgTypeFromMD(*F, i)))
        continue;

      // Add size implicit argument.
      ArgTypes.push_back(ImageSizeType);
      ArgMD[2] = ArgMD[3] = MDString::get(*Context, ImageSizeArgMDType);
      PushArgMD(NewArgMDs, ArgMD);

      // Add format implicit argument.
      ArgTypes.push_back(ImageFormatType);
      ArgMD[2] = ArgMD[3] = MDString::get(*Context, ImageFormatArgMDType);
      PushArgMD(NewArgMDs, ArgMD);

      Modified = true;
    }
    if (!Modified) {
      return std::make_tuple(nullptr, nullptr);
    }

    // Create function with new signature and clone the old body into it.
    auto NewFT = FunctionType::get(FT->getReturnType(), ArgTypes, false);
    auto NewF = Function::Create(NewFT, F->getLinkage(), F->getName());
    ValueToValueMapTy VMap;
    auto NewFArgIt = NewF->arg_begin();
    for (auto &Arg: F->args()) {
      auto ArgName = Arg.getName();
      NewFArgIt->setName(ArgName);
      VMap[&Arg] = &(*NewFArgIt++);
      if (IsImageType(ArgTypeFromMD(*F, Arg.getArgNo()))) {
        (NewFArgIt++)->setName(Twine("__size_") + ArgName);
        (NewFArgIt++)->setName(Twine("__format_") + ArgName);
      }
    }
    SmallVector<ReturnInst*, 8> Returns;
    CloneFunctionInto(NewF, F, VMap, /*ModuleLevelChanges=*/false, Returns);

    // Update function metadata.
    for (unsigned i = 0; i < NumKernelArgMDNodes; ++i)
      NewF->setMetadata(KernelArgMDNodeNames[i],
                        MDNode::get(*Context, NewArgMDs.ArgVector[i]));

    return NewF;
  }

  bool transformKernels(Function &F) {
    bool Modified = false;
    auto M = F.getParent();
    Function *NewF;
    NewF = addImplicitArgs(&F);
    if (NewF) {
      // Replace old function and metadata with new ones.
      F.eraseFromParent();
      M->getFunctionList().push_back(NewF);
      M->getOrInsertFunction(NewF->getName(), NewF->getFunctionType(),
                             NewF->getAttributes());
      Modified = true;
    }
    Modified |= replaceImageAndSamplerUses(NewF);
    return Modified;
  }

 public:
  AMDGPUOpenCLImageTypeLoweringPass() : ModulePass(ID) {}

  bool runOnModule(Module &M) override {
    Context = &M.getContext();
    Int32Type = Type::getInt32Ty(M.getContext());
    ImageSizeType = ArrayType::get(Int32Type, 3);
    ImageFormatType = ArrayType::get(Int32Type, 2);

    bool Changed = false;
    for (auto F: M.functions())
      Changed != transformKernels(F);
    return Changed;
  }

  const char *getPassName() const override {
    return "AMDGPU OpenCL Image Type Pass";
  }
};

char AMDGPUOpenCLImageTypeLoweringPass::ID = 0;

} // end anonymous namespace

ModulePass *llvm::createAMDGPUOpenCLImageTypeLoweringPass() {
  return new AMDGPUOpenCLImageTypeLoweringPass();
}
