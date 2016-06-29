; RUN: llc -mtriple=amdgcn--amdhsa < %s | FileCheck %s

; CHECK:.section        .OpenCL.Metadata
; CHECK-NEXT:.long   2

; CHECK-LABEL:{{^}}kernel1:
; CHECK:.section        .OpenCL.Metadata
; CHECK-NEXT:.long   1
; CHECK-NEXT:.long   12337
; CHECK-NEXT:.long   4
; CHECK-NEXT:.long   4
; CHECK-NEXT:.long   3
; CHECK-NEXT:.ascii  "int"
; CHECK-NEXT:.long   5
; CHECK-NEXT:.long   1
; CHECK-NEXT:.long   2
; CHECK-NEXT:.long   4
; CHECK-NEXT:.long   3
; CHECK-NEXT:.ascii  "int"

define amdgpu_kernel void @kernel1(i32 %a) !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !3 !kernel_arg_base_type !3 !kernel_arg_type_qual !4 !vec_type_hint !5 !reqd_work_group_size !6 {
  ret void
}

; CHECK-LABEL:{{^}}kernel2:
; CHECK:.section        .OpenCL.Metadata
; CHECK-NEXT:.long   1
; CHECK-NEXT:.long   12337
; CHECK-NEXT:.long   4
; CHECK-NEXT:.long   4
; CHECK-NEXT:.long   3
; CHECK-NEXT:.ascii  "int"
; CHECK-NEXT:.long   6
; CHECK-NEXT:.long   8
; CHECK-NEXT:.long   16
; CHECK-NEXT:.long   32
; CHECK-NEXT:.long   5
; CHECK-NEXT:.ascii  "uint4"

define amdgpu_kernel void @kernel2(i32 %a) !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !3 !kernel_arg_base_type !3 !kernel_arg_type_qual !4 !vec_type_hint !7 !work_group_size_hint !8 {
  ret void
}

!1 = !{i32 0}
!2 = !{!"none"}
!3 = !{!"int"}
!4 = !{!""}
!5 = !{i32 undef, i32 1}
!6 = !{i32 1, i32 2, i32 4}
!7 = !{<4 x i32> undef, i32 0}
!8 = !{i32 8, i32 16, i32 32}
