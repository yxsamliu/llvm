; RUN: opt -instsimplify -S < %s | FileCheck %s

target datalayout = "p:32:32-p4:64:64"

; When a pointer is addrspacecasted to a wider pointer, there is no guarantee
; that the newly added high bits are zero.
; CHECK-NOT: ret i64 0
define i64 @test(i8* %p) {
  %g = addrspacecast i8* %p to i8 addrspace(4)*
  %i = ptrtoint i8 addrspace(4)* %g to i64
  %shift = lshr i64 %i, 32
  ret i64 %shift
}