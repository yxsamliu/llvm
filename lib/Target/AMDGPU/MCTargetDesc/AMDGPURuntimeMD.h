//===- AMDGPURuntimeMD.h - Generate runtime metadata ---------------*- C++ -*-//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares functions for generating runtime metadata.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AMDGPU_MCTARGETDESC_AMDGPURUNTIMEMD_H
#define LLVM_LIB_TARGET_AMDGPU_MCTARGETDESC_AMDGPURUNTIMEMD_H

#include <string>

namespace llvm {
class Module;

std::string getRuntimeMD(Module &M);

}
#endif
