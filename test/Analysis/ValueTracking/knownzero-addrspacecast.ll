; RUN: opt -instsimplify -S < %s | FileCheck %s

target datalayout = "e-p:32:32-p1:64:64-p2:64:64-p3:32:32-p4:64:64-p5:32:32-i64:64-v16:16-v24:32-v32:32-v48:64-v96:128-v192:256-v256:256-v512:512-v1024:1024-v2048:2048-n32:64"
target triple = "amdgcn-amd-amdhsa-opencl"

define i64 @test() {
  %p = alloca i8, align 4
  %g = addrspacecast i8* %p to i8 addrspace(4)*
  %i = ptrtoint i8 addrspace(4)* %g to i64
  %shift = lshr i64 %i, 32
  ret i64 %shift
}