//===-- AMDGPURuntimeMDYamlMapping.cpp - Generates runtime metadata -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
/// \file
///
/// Generates AMDGPU runtime metadata for YAML mapping.
//
//===----------------------------------------------------------------------===//
//

#include "AMDGPU.h"
#include "AMDGPURuntimeMetadata.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/YAMLTraits.h"
#include <vector>
#include "AMDGPURuntimeMD.h"

using namespace ::AMDGPU;
using namespace llvm;

namespace llvm {

class Module;

namespace yaml {

// Invalid values are used to indicate an optional key should not be emitted.
#define INVALID_ADDR_QUAL     uint8_t(0xff)
#define INVALID_ACC_QUAL      uint8_t(0xff)
#define INVALID_KERNEL_INDEX  uint32_t(~0U)

struct Arg {
  uint32_t Size;
  uint32_t Align;
  uint32_t PointeeAlign;
  uint8_t Kind;
  uint16_t ValueType;
  StringRef TypeName;
  StringRef Name;
  uint8_t AddrQual;
  uint8_t AccQual;
  uint8_t IsVolatile;
  uint8_t IsConst;
  uint8_t IsRestrict;
  uint8_t IsPipe;
  Arg() : Size(0), Align(0), PointeeAlign(0), Kind(0), ValueType(0),
      AddrQual(INVALID_ADDR_QUAL), AccQual(INVALID_ACC_QUAL), IsVolatile(0),
      IsConst(0), IsRestrict(0), IsPipe(0) {}
};

struct Kernel {
  StringRef Name;
  StringRef Language;
  std::vector<uint8_t> LanguageVersion;
  std::vector<uint32_t> ReqdWorkGroupSize;
  std::vector<uint32_t> WorkGroupSizeHint;
  // This value cannot be StringRef since it is generated on the fly.
  std::string VecTypeHint;
  uint32_t KernelIndex;
  uint8_t NoPartialWorkGroups;
  std::vector<Arg> Args;
  Kernel() : KernelIndex(INVALID_KERNEL_INDEX), NoPartialWorkGroups(0) {}
};

struct Program {
  std::vector<uint8_t> MDVersionSeq;
  std::vector<StringRef> PrintfInfo;
  std::vector<Kernel> Kernels;

  Program(Module &M);
};
}
}

LLVM_YAML_IS_FLOW_SEQUENCE_VECTOR(uint8_t)
LLVM_YAML_IS_FLOW_SEQUENCE_VECTOR(uint32_t)
LLVM_YAML_IS_FLOW_SEQUENCE_VECTOR(StringRef)
LLVM_YAML_IS_SEQUENCE_VECTOR(Kernel)
LLVM_YAML_IS_SEQUENCE_VECTOR(Arg)

namespace llvm {
namespace yaml {

template <> struct MappingTraits<Arg> {
  static void mapping(IO &YamlIO, Arg &A) {
    YamlIO.mapRequired(::AMDGPU::RuntimeMD::KeyName::ArgSize,
        A.Size);
    YamlIO.mapRequired(::AMDGPU::RuntimeMD::KeyName::ArgAlign,
        A.Align);
    YamlIO.mapOptional(::AMDGPU::RuntimeMD::KeyName::ArgPointeeAlign,
        A.PointeeAlign, 0U);
    YamlIO.mapRequired(::AMDGPU::RuntimeMD::KeyName::ArgKind,
        A.Kind);
    YamlIO.mapRequired(::AMDGPU::RuntimeMD::KeyName::ArgValueType,
        A.ValueType);
    YamlIO.mapOptional(::AMDGPU::RuntimeMD::KeyName::ArgTypeName,
        A.TypeName, StringRef());
    YamlIO.mapOptional(::AMDGPU::RuntimeMD::KeyName::ArgName,
        A.Name, StringRef());
    YamlIO.mapOptional(::AMDGPU::RuntimeMD::KeyName::ArgAddrQual,
        A.AddrQual, INVALID_ADDR_QUAL);
    YamlIO.mapOptional(::AMDGPU::RuntimeMD::KeyName::ArgAccQual,
        A.AccQual, INVALID_ACC_QUAL);
    YamlIO.mapOptional(::AMDGPU::RuntimeMD::KeyName::ArgIsVolatile,
        A.IsVolatile, uint8_t(0));
    YamlIO.mapOptional(::AMDGPU::RuntimeMD::KeyName::ArgIsConst,
        A.IsConst, uint8_t(0));
    YamlIO.mapOptional(::AMDGPU::RuntimeMD::KeyName::ArgIsRestrict,
        A.IsRestrict, uint8_t(0));
    YamlIO.mapOptional(::AMDGPU::RuntimeMD::KeyName::ArgIsPipe,
        A.IsPipe, uint8_t(0));
  }
  static const bool flow = true;
};

template <> struct MappingTraits<Kernel> {
  static void mapping(IO &YamlIO, Kernel &K) {
    YamlIO.mapRequired(::AMDGPU::RuntimeMD::KeyName::KernelName,
        K.Name);
    YamlIO.mapOptional(::AMDGPU::RuntimeMD::KeyName::Language,
        K.Language);
    YamlIO.mapOptional(::AMDGPU::RuntimeMD::KeyName::LanguageVersion,
        K.LanguageVersion);
    YamlIO.mapOptional(::AMDGPU::RuntimeMD::KeyName::ReqdWorkGroupSize,
        K.ReqdWorkGroupSize);
    YamlIO.mapOptional(::AMDGPU::RuntimeMD::KeyName::WorkGroupSizeHint,
        K.WorkGroupSizeHint);
    YamlIO.mapOptional(::AMDGPU::RuntimeMD::KeyName::VecTypeHint,
        K.VecTypeHint, std::string());
    YamlIO.mapOptional(::AMDGPU::RuntimeMD::KeyName::KernelIndex,
        K.KernelIndex, INVALID_KERNEL_INDEX);
    YamlIO.mapOptional(::AMDGPU::RuntimeMD::KeyName::NoPartialWorkGroups,
        K.NoPartialWorkGroups, uint8_t(0));
    YamlIO.mapRequired(::AMDGPU::RuntimeMD::KeyName::Args,
        K.Args);
  }
  static const bool flow = true;
};

template <> struct MappingTraits<Program> {
  static void mapping(IO &YamlIO, Program &Prog) {
    YamlIO.mapRequired(::AMDGPU::RuntimeMD::KeyName::MDVersion,
        Prog.MDVersionSeq);
    YamlIO.mapOptional(::AMDGPU::RuntimeMD::KeyName::PrintfInfo,
        Prog.PrintfInfo);
    YamlIO.mapRequired(::AMDGPU::RuntimeMD::KeyName::Kernels,
      Prog.Kernels);
  }
  static const bool flow = true;
};

} // end namespace yaml
} // end namespace llvm


// Get a vector of three integer values from MDNode \p Node;
static std::vector<uint32_t> getThreeInt32(MDNode *Node) {
  assert(Node->getNumOperands() == 3);
  std::vector<uint32_t> V;
  for (const MDOperand &Op : Node->operands()) {
    const ConstantInt *CI = mdconst::extract<ConstantInt>(Op);
    V.push_back(CI->getZExtValue());
  }
  return V;
}

static std::string getOCLTypeName(Type *Ty, bool Signed) {
  switch (Ty->getTypeID()) {
  case Type::HalfTyID:
    return "half";
  case Type::FloatTyID:
    return "float";
  case Type::DoubleTyID:
    return "double";
  case Type::IntegerTyID: {
    if (!Signed)
      return (Twine('u') + getOCLTypeName(Ty, true)).str();
    unsigned BW = Ty->getIntegerBitWidth();
    switch (BW) {
    case 8:
      return "char";
    case 16:
      return "short";
    case 32:
      return "int";
    case 64:
      return "long";
    default:
      return (Twine('i') + Twine(BW)).str();
    }
  }
  case Type::VectorTyID: {
    VectorType *VecTy = cast<VectorType>(Ty);
    Type *EleTy = VecTy->getElementType();
    unsigned Size = VecTy->getVectorNumElements();
    return (Twine(getOCLTypeName(EleTy, Signed)) + Twine(Size)).str();
  }
  default:
    return "unknown";
  }
}

static RuntimeMD::KernelArg::ValueType getRuntimeMDValueType(
  Type *Ty, StringRef TypeName) {
  switch (Ty->getTypeID()) {
  case Type::HalfTyID:
    return RuntimeMD::KernelArg::F16;
  case Type::FloatTyID:
    return RuntimeMD::KernelArg::F32;
  case Type::DoubleTyID:
    return RuntimeMD::KernelArg::F64;
  case Type::IntegerTyID: {
    bool Signed = !TypeName.startswith("u");
    switch (Ty->getIntegerBitWidth()) {
    case 8:
      return Signed ? RuntimeMD::KernelArg::I8 : RuntimeMD::KernelArg::U8;
    case 16:
      return Signed ? RuntimeMD::KernelArg::I16 : RuntimeMD::KernelArg::U16;
    case 32:
      return Signed ? RuntimeMD::KernelArg::I32 : RuntimeMD::KernelArg::U32;
    case 64:
      return Signed ? RuntimeMD::KernelArg::I64 : RuntimeMD::KernelArg::U64;
    default:
      // Runtime does not recognize other integer types. Report as struct type.
      return RuntimeMD::KernelArg::Struct;
    }
  }
  case Type::VectorTyID:
    return getRuntimeMDValueType(Ty->getVectorElementType(), TypeName);
  case Type::PointerTyID:
    return getRuntimeMDValueType(Ty->getPointerElementType(), TypeName);
  default:
    return RuntimeMD::KernelArg::Struct;
  }
}

static RuntimeMD::KernelArg::AddressSpaceQualifer getRuntimeAddrSpace(
    AMDGPUAS::AddressSpaces A) {
  switch (A) {
  case AMDGPUAS::GLOBAL_ADDRESS:
    return RuntimeMD::KernelArg::Global;
  case AMDGPUAS::CONSTANT_ADDRESS:
    return RuntimeMD::KernelArg::Constant;
  case AMDGPUAS::LOCAL_ADDRESS:
    return RuntimeMD::KernelArg::Local;
  case AMDGPUAS::FLAT_ADDRESS:
    return RuntimeMD::KernelArg::Generic;
  case AMDGPUAS::REGION_ADDRESS:
    return RuntimeMD::KernelArg::Region;
  default:
    return RuntimeMD::KernelArg::Private;
  }
}

static yaml::Arg getRuntimeMDForKernelArg(const DataLayout &DL, Type *T,
    RuntimeMD::KernelArg::Kind Kind, StringRef BaseTypeName = "",
    StringRef TypeName = "", StringRef ArgName = "", StringRef TypeQual = "",
    StringRef AccQual = "") {

  yaml::Arg Arg;

  // Set ArgSize and ArgAlign.
  Arg.Size = DL.getTypeAllocSize(T);
  Arg.Align = DL.getABITypeAlignment(T);
  if (auto PT = dyn_cast<PointerType>(T)) {
    auto ET = PT->getElementType();
    if (PT->getAddressSpace() == AMDGPUAS::LOCAL_ADDRESS && ET->isSized())
      Arg.PointeeAlign = DL.getABITypeAlignment(ET);
  }

  // Set ArgTypeName.
  Arg.TypeName = TypeName;

  // Set ArgName.
  Arg.Name = ArgName;

  // Set ArgIsVolatile, ArgIsRestrict, ArgIsConst and ArgIsPipe.
  SmallVector<StringRef, 1> SplitQ;
  TypeQual.split(SplitQ, " ", -1, false /* Drop empty entry */);

  for (StringRef KeyName : SplitQ) {
    auto *P = StringSwitch<uint8_t *>(KeyName)
      .Case("volatile", &Arg.IsVolatile)
      .Case("restrict", &Arg.IsRestrict)
      .Case("const",    &Arg.IsConst)
      .Case("pipe",     &Arg.IsPipe)
      .Default(nullptr);
    if (P)
      *P = 1;
  }

  // Set ArgKind.
  Arg.Kind = Kind;

  // Set ArgValueType.
  Arg.ValueType = getRuntimeMDValueType(T, BaseTypeName);

  // Set ArgAccQual.
  if (!AccQual.empty())
    Arg.AccQual = StringSwitch<RuntimeMD::KernelArg::AccessQualifer>(AccQual)
      .Case("read_only",  RuntimeMD::KernelArg::ReadOnly)
      .Case("write_only", RuntimeMD::KernelArg::WriteOnly)
      .Case("read_write", RuntimeMD::KernelArg::ReadWrite)
      .Default(RuntimeMD::KernelArg::None);

  // Set ArgAddrQual.
  if (auto *PT = dyn_cast<PointerType>(T))
    Arg.AddrQual = getRuntimeAddrSpace(static_cast<AMDGPUAS::AddressSpaces>(
        PT->getAddressSpace()));

  return Arg;
}

static yaml::Kernel getRuntimeMDForKernel(const Function &F) {
  yaml::Kernel Kernel;
  Kernel.Name = F.getName();
  auto &M = *F.getParent();

  // Set Language and LanguageVersion.
  if (auto MD = M.getNamedMetadata("opencl.ocl.version")) {
    if (MD->getNumOperands() != 0) {
      auto Node = MD->getOperand(0);
      if (Node->getNumOperands() > 1) {
        Kernel.Language = "OpenCL C";
        uint16_t Major = mdconst::extract<ConstantInt>(Node->getOperand(0))
                         ->getZExtValue();
        uint16_t Minor = mdconst::extract<ConstantInt>(Node->getOperand(1))
                         ->getZExtValue();
        Kernel.LanguageVersion.push_back(Major);
        Kernel.LanguageVersion.push_back(Minor);
      }
    }
  }

  const DataLayout &DL = F.getParent()->getDataLayout();
  for (auto &Arg : F.args()) {
    unsigned I = Arg.getArgNo();
    Type *T = Arg.getType();
    auto TypeName = dyn_cast<MDString>(F.getMetadata(
        "kernel_arg_type")->getOperand(I))->getString();
    auto BaseTypeName = cast<MDString>(F.getMetadata(
        "kernel_arg_base_type")->getOperand(I))->getString();
    StringRef ArgName;
    if (auto ArgNameMD = F.getMetadata("kernel_arg_name"))
      ArgName = cast<MDString>(ArgNameMD->getOperand(I))->getString();
    auto TypeQual = cast<MDString>(F.getMetadata(
        "kernel_arg_type_qual")->getOperand(I))->getString();
    auto AccQual = cast<MDString>(F.getMetadata(
        "kernel_arg_access_qual")->getOperand(I))->getString();
    RuntimeMD::KernelArg::Kind Kind;
    if (TypeQual.find("pipe") != StringRef::npos)
      Kind = RuntimeMD::KernelArg::Pipe;
    else Kind = StringSwitch<RuntimeMD::KernelArg::Kind>(BaseTypeName)
      .Case("sampler_t", RuntimeMD::KernelArg::Sampler)
      .Case("queue_t",   RuntimeMD::KernelArg::Queue)
      .Cases("image1d_t", "image1d_array_t", "image1d_buffer_t",
             "image2d_t" , "image2d_array_t",  RuntimeMD::KernelArg::Image)
      .Cases("image2d_depth_t", "image2d_array_depth_t",
             "image2d_msaa_t", "image2d_array_msaa_t",
             "image2d_msaa_depth_t",  RuntimeMD::KernelArg::Image)
      .Cases("image2d_array_msaa_depth_t", "image3d_t",
             RuntimeMD::KernelArg::Image)
      .Default(isa<PointerType>(T) ?
                   (T->getPointerAddressSpace() == AMDGPUAS::LOCAL_ADDRESS ?
                   RuntimeMD::KernelArg::DynamicSharedPointer :
                   RuntimeMD::KernelArg::GlobalBuffer) :
                   RuntimeMD::KernelArg::ByValue);
    Kernel.Args.emplace_back(getRuntimeMDForKernelArg(DL, T, Kind,
        BaseTypeName, TypeName, ArgName, TypeQual, AccQual));
  }

  // Emit hidden kernel arguments for OpenCL kernels.
  if (F.getParent()->getNamedMetadata("opencl.ocl.version")) {
    auto Int64T = Type::getInt64Ty(F.getContext());
    Kernel.Args.emplace_back(getRuntimeMDForKernelArg(DL, Int64T,
        RuntimeMD::KernelArg::HiddenGlobalOffsetX));
    Kernel.Args.emplace_back(getRuntimeMDForKernelArg(DL, Int64T,
        RuntimeMD::KernelArg::HiddenGlobalOffsetY));
    Kernel.Args.emplace_back(getRuntimeMDForKernelArg(DL, Int64T,
        RuntimeMD::KernelArg::HiddenGlobalOffsetZ));
    if (F.getParent()->getNamedMetadata("llvm.printf.fmts")) {
      auto Int8PtrT = Type::getInt8PtrTy(F.getContext(),
          RuntimeMD::KernelArg::Global);
      Kernel.Args.emplace_back(getRuntimeMDForKernelArg(DL, Int8PtrT,
          RuntimeMD::KernelArg::HiddenPrintfBuffer));
    }
  }

  // Set ReqdWorkGroupSize, WorkGroupSizeHint, and VecTypeHint.
  if (auto RWGS = F.getMetadata("reqd_work_group_size"))
    Kernel.ReqdWorkGroupSize = getThreeInt32(RWGS);

  if (auto WGSH = F.getMetadata("work_group_size_hint"))
    Kernel.WorkGroupSizeHint = getThreeInt32(WGSH);

  if (auto VTH = F.getMetadata("vec_type_hint"))
    Kernel.VecTypeHint = getOCLTypeName(cast<ValueAsMetadata>(
      VTH->getOperand(0))->getType(), mdconst::extract<ConstantInt>(
      VTH->getOperand(1))->getZExtValue());

  return Kernel;
}

yaml::Program::Program(Module &M) {
  MDVersionSeq.push_back(::AMDGPU::RuntimeMD::MDVersion);
  MDVersionSeq.push_back(::AMDGPU::RuntimeMD::MDRevision);

  // Set PrintfInfo.
  if (auto MD = M.getNamedMetadata("llvm.printf.fmts")) {
    for (unsigned I = 0; I < MD->getNumOperands(); ++I) {
      auto Node = MD->getOperand(I);
      if (Node->getNumOperands() > 0)
        PrintfInfo.push_back(cast<MDString>(Node->getOperand(0))
            ->getString());
    }
  }

  // Set Kernels.
  for (auto &F: M.functions()) {
    if (!F.getMetadata("kernel_arg_type"))
      continue;
    Kernels.emplace_back(getRuntimeMDForKernel(F));
  }
}

std::string llvm::getRuntimeMD(Module &M) {
  std::string Text;
  raw_string_ostream Stream(Text);
  yaml::Output Output(Stream, nullptr, INT_MAX /* do not wrap line */);
  yaml::Program Prog(M);
  Output << Prog;
  DEBUG_WITH_TYPE("amdgpu-rtmd",
      llvm::dbgs() << "AMDGPU runtime metadata:\n" << Stream.str() << '\n');
  return Text;
}
