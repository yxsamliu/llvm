//===-- AMDGPUOpenCLMetadata.h - AMDGPU OpenCL Metadata Header File -------===//
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
/// Enums and structure types used by OpenCL metadata.
//
//===----------------------------------------------------------------------===//
//
#ifndef LLVM_LIB_TARGET_AMDGPU_AMDGPUOPENCLMETADATA_H
#define LLVM_LIB_TARGET_AMDGPU_AMDGPUOPENCLMETADATA_H

#include <string>
#include <vector>

namespace AMDGPU {

  namespace KernelArg {
    enum Type : char {
      Pointer   = 0,
      Value     = 1,
      Image     = 2,
      Sampler   = 3,
      Queue     = 4,
    };

    enum DataType : char {
      Struct  = 0,
      I8      = 1,
      U8      = 2,
      I16     = 3,
      U16     = 4,
      F16     = 5,
      I32     = 6,
      U32     = 7,
      F32     = 8,
      I64     = 9,
      U64     = 10,
      F64     = 11,
    };

    enum TypeQualifier : char {
      Volatile = 1,
      Restrict = 2,
      Pipe     = 4,
      Const    = 8,
    };

    enum AccessQualifer : char {
      ReadOnly   = 0,
      WriteOnly  = 1,
      ReadWrite  = 2,
      None       = 3,
    };

    struct Flag {
      unsigned TypeKind : 3;
      unsigned DataType : 4;
      unsigned HasName  : 1;  // Whether the argument has name
      unsigned TypeQual : 4;  // Type qualifier
      unsigned AccQual  : 2;  // Access qualifier
      unsigned AddrQual : 2;  // Address qualifier

      unsigned getAsUnsignedInt() {
        return TypeKind
          | DataType << 3
          | HasName  << 7
          | TypeQual << 8
          | AccQual  << 12
          | AddrQual << 14;
      }
    };
  } // namespace KernelArg

  namespace Kernel {
    struct Flag {
      unsigned HasReqdWorkGroupSize : 1; // Has reqd_work_group_size attribute
      unsigned HasWorkGroupSizeHint : 1; // Has work_group_size_hint attribute
      unsigned HasVecTypeHint       : 1; // Has vec_type_hint attribute
      unsigned IsDevEnqKernel       : 1; // Is device enqueue kernel
      unsigned getAsUnsignedInt() {
        return HasReqdWorkGroupSize
          | HasWorkGroupSizeHint << 1
          | HasVecTypeHint << 2
          | IsDevEnqKernel << 3;
      }
    };
  } // namespace Kernel
}

#endif // LLVM_LIB_TARGET_AMDGPU_AMDGPUOPENCLMETADATA_H
