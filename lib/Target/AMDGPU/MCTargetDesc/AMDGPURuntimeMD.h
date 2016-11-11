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

#include <string>

namespace llvm {
class Module;

std::string getRuntimeMD(Module &M);

}
#endif
