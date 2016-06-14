; RUN: llc -march=amdgcn -verify-machineinstrs -asm-verbose < %s | FileCheck -check-prefix=SI %s
; RUN: llc -march=amdgcn -mtriple=amdgcn-unknown-amdhsa -verify-machineinstrs -asm-verbose -mattr=-flat-for-global < %s | FileCheck -check-prefix=SI %s

; SI-LABEL: {{^}}kernel1:
; SI: .section	.Kernel.Attributes
; SI: ; kernel1.VTH=int
; SI: ; kernel1.RWGS=1 2 4

define void @kernel1(i32 %a) !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !3 !kernel_arg_base_type !3 !kernel_arg_type_qual !4 !vec_type_hint !5 !reqd_work_group_size !6 {
  ret void
}

; SI-LABEL: {{^}}kernel2:
; SI: ; kernel1.VTH=uint4
; SI: ; kernel1.WGSH=8 16 32

define void @kernel2(i32 %a) !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !3 !kernel_arg_base_type !3 !kernel_arg_type_qual !4 !vec_type_hint !7 !work_group_size_hint !8 {
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
