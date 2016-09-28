//===- AMDGPURuntimeMDYamlMapping.h - Map runtime metadata to YAML -*- C++ -*-//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the mapping between various AMDGPU runtime metadata
// structures and their corresponding YAML representation.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AMDGPU_AMDGPURUNTIMEMDYAMLMAPPING_H
#define LLVM_LIB_TARGET_AMDGPU_AMDGPURUNTIMEMDYAMLMAPPING_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/YAMLTraits.h"
#include "AMDGPURuntimeMetadata.h"
#include <vector>

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
LLVM_YAML_IS_FLOW_SEQUENCE_VECTOR(Kernel)
LLVM_YAML_IS_FLOW_SEQUENCE_VECTOR(Arg)

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


#endif
