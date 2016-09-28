//===-- AMDGPURuntimeMetadata.h - AMDGPU Runtime Metadata -------*- C++ -*-===//
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
/// Enums and structure types used by runtime metadata.
///
/// Runtime requests certain information (metadata) about kernels to be able
/// to execute the kernels and answer the queries about the kernels.
/// The metadata is represented as a byte stream in an ELF section of a
/// binary (code object). The byte stream consists of key-value pairs.
/// Each key is an 8 bit unsigned integer. Each value can be an integer,
/// a string, or a stream of key-value pairs. There are 3 levels of key-value
/// pair streams. At the beginning of the ELF section is the top level
/// key-value pair stream. A kernel-level key-value pair stream starts after
/// encountering KeyKernelBegin and ends immediately before encountering
/// KeyKernelEnd. A kernel-argument-level key-value pair stream starts
/// after encountering KeyArgBegin and ends immediately before encountering
/// KeyArgEnd. A kernel-level key-value pair stream can only appear in a top
/// level key-value pair stream. A kernel-argument-level key-value pair stream
/// can only appear in a kernel-level key-value pair stream.
///
/// The format should be kept backward compatible. New enum values and bit
/// fields should be appended at the end. It is suggested to bump up the
/// revision number whenever the format changes and document the change
/// in the revision in this header.
///
//
//===----------------------------------------------------------------------===//
//
#ifndef LLVM_LIB_TARGET_AMDGPU_AMDGPURUNTIMEMETADATA_H
#define LLVM_LIB_TARGET_AMDGPU_AMDGPURUNTIMEMETADATA_H

#include <stdint.h>

namespace AMDGPU {

namespace RuntimeMD {

  // Version and revision of runtime metadata
  const unsigned char MDVersion   = 2;
  const unsigned char MDRevision  = 0;

  // ELF section name containing runtime metadata
  const char SectionName[] = ".AMDGPU.runtime_metadata";

  // Name of keys for runtime metadata.
  namespace KeyName {
    const char MDVersion[]                = "amd.MDVersion";            // Runtime metadata version
    const char Language[]                 = "amd.Language";             // Language
    const char LanguageVersion[]          = "amd.LanguageVersion";      // Language version
    const char Kernels[]                  = "amd.Kernels";              // Kernels
    const char KernelName[]               = "amd.KernelName";           // Kernel name
    const char Args[]                     = "amd.Args";                 // Kernel arguments
    const char ArgSize[]                  = "amd.ArgSize";              // Kernel arg size
    const char ArgAlign[]                 = "amd.ArgAlign";             // Kernel arg alignment
    const char ArgTypeName[]              = "amd.ArgTypeName";          // Kernel type name
    const char ArgName[]                  = "amd.ArgName";              // Kernel name
    const char ArgKind[]                  = "amd.ArgKind";              // Kernel argument kind
    const char ArgValueType[]             = "amd.ArgValueType";         // Kernel argument value type
    const char ArgAddrQual[]              = "amd.ArgAddrQual";          // Kernel argument address qualifier
    const char ArgAccQual[]               = "amd.ArgAccQual";           // Kernel argument access qualifier
    const char ArgIsConst[]               = "amd.ArgIsConst";           // Kernel argument is const qualified
    const char ArgIsRestrict[]            = "amd.ArgIsRestrict";        // Kernel argument is restrict qualified
    const char ArgIsVolatile[]            = "amd.ArgIsVolatile";        // Kernel argument is volatile qualified
    const char ArgIsPipe[]                = "amd.ArgIsPipe";            // Kernel argument is pipe qualified
    const char ReqdWorkGroupSize[]        = "amd.ReqdWorkGroupSize";    // Required work group size
    const char WorkGroupSizeHint[]        = "amd.WorkGroupSizeHint";    // Work group size hint
    const char VecTypeHint[]              = "amd.VecTypeHint";          // Vector type hint
    const char KernelIndex[]              = "amd.KernelIndex";          // Kernel index for device enqueue
    const char NoPartialWorkGroups[]      = "amd.NoPartialWorkGroups";  // No partial work groups
    const char PrintfInfo[]               = "amd.PrintfInfo";           // Prinf function call information
    const char ArgActualAcc[]             = "amd.ArgActualAcc";         // The actual kernel argument access qualifier
    const char ArgPointeeAlign[]          = "amd.ArgPointeeAlign";      // Alignment of pointee type
  };

  namespace KernelArg {
    enum Kind : uint8_t {
      ByValue                 = 0,
      GlobalBuffer            = 1,
      DynamicSharedPointer    = 2,
      Sampler                 = 3,
      Image                   = 4,
      Pipe                    = 5,
      Queue                   = 6,
      HiddenGlobalOffsetX     = 7,
      HiddenGlobalOffsetY     = 8,
      HiddenGlobalOffsetZ     = 9,
      HiddenNone              = 10,
      HiddenPrintfBuffer      = 11,
      HiddenDefaultQueue      = 12,
      HiddenCompletionAction  = 13,
    };

    enum ValueType : uint16_t {
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

    enum AccessQualifer : uint8_t {
      None       = 0,
      ReadOnly   = 1,
      WriteOnly  = 2,
      ReadWrite  = 3,
    };

    enum AddressSpaceQualifer : uint8_t {
      Private    = 0,
      Global     = 1,
      Constant   = 2,
      Local      = 3,
      Generic    = 4,
      Region     = 5,
    };
  } // namespace KernelArg
} // namespace RuntimeMD
} // namespace AMDGPU

#endif // LLVM_LIB_TARGET_AMDGPU_AMDGPURUNTIMEMETADATA_H
