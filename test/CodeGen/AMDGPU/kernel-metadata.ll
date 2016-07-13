; RUN: llc -mtriple=amdgcn--amdhsa < %s | FileCheck %s

%struct.A = type { i8, float }
%opencl.image1d_t = type opaque
%opencl.image2d_t = type opaque
%opencl.image3d_t = type opaque
%opencl.queue_t = type opaque
%opencl.pipe_t = type opaque

; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .byte 1
; CHECK-NEXT: .short  256

; CHECK-LABEL:{{^}}test_char:
; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .long	9
; CHECK-NEXT: .ascii	"test_char"
; CHECK-NEXT: .long	1
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	1
; CHECK-NEXT: .long	1
; CHECK-NEXT: .long	4
; CHECK-NEXT: .ascii	"char"
; CHECK-NEXT: .long	0

define amdgpu_kernel void @test_char(i8 %a) !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !9 !kernel_arg_base_type !9 !kernel_arg_type_qual !4 {
  ret void
}

; CHECK-LABEL:{{^}}test_ushort2:
; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .long	12
; CHECK-NEXT: .ascii	"test_ushort2"
; CHECK-NEXT: .long	1
; CHECK-NEXT: .long	32
; CHECK-NEXT: .long	4
; CHECK-NEXT: .long	4
; CHECK-NEXT: .long	7
; CHECK-NEXT: .ascii	"ushort2"
; CHECK-NEXT: .long	0

define amdgpu_kernel void @test_ushort2(<2 x i16> %a) !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !10 !kernel_arg_base_type !10 !kernel_arg_type_qual !4 {
  ret void
}

; CHECK-LABEL:{{^}}test_int3:
; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .long	9
; CHECK-NEXT: .ascii	"test_int3"
; CHECK-NEXT: .long	1
; CHECK-NEXT: .long	48
; CHECK-NEXT: .long	16
; CHECK-NEXT: .long	16
; CHECK-NEXT: .long	4
; CHECK-NEXT: .ascii	"int3"
; CHECK-NEXT: .long	0

define amdgpu_kernel void @test_int3(<3 x i32> %a) !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !11 !kernel_arg_base_type !11 !kernel_arg_type_qual !4 {
  ret void
}

; CHECK-LABEL:{{^}}test_ulong4:
; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .long	11
; CHECK-NEXT: .ascii	"test_ulong4"
; CHECK-NEXT: .long	1
; CHECK-NEXT: .long	80
; CHECK-NEXT: .long	32
; CHECK-NEXT: .long	32
; CHECK-NEXT: .long	6
; CHECK-NEXT: .ascii	"ulong4"
; CHECK-NEXT: .long	0

define amdgpu_kernel void @test_ulong4(<4 x i64> %a) !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !12 !kernel_arg_base_type !12 !kernel_arg_type_qual !4 {
  ret void
}

; CHECK-LABEL:{{^}}test_half8:
; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .long	10
; CHECK-NEXT: .ascii	"test_half8"
; CHECK-NEXT: .long	1
; CHECK-NEXT: .long	40
; CHECK-NEXT: .long	16
; CHECK-NEXT: .long	16
; CHECK-NEXT: .long	5
; CHECK-NEXT: .ascii	"half8"
; CHECK-NEXT: .long	0

define amdgpu_kernel void @test_half8(<8 x half> %a) !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !13 !kernel_arg_base_type !13 !kernel_arg_type_qual !4 {
  ret void
}

; CHECK-LABEL:{{^}}test_float16:
; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .long	12
; CHECK-NEXT: .ascii	"test_float16"
; CHECK-NEXT: .long	1
; CHECK-NEXT: .long	64
; CHECK-NEXT: .long	64
; CHECK-NEXT: .long	64
; CHECK-NEXT: .long	7
; CHECK-NEXT: .ascii	"float16"
; CHECK-NEXT: .long	0

define amdgpu_kernel void @test_float16(<16 x float> %a) !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !14 !kernel_arg_base_type !14 !kernel_arg_type_qual !4 {
  ret void
}

; CHECK-LABEL:{{^}}test_double16:
; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .long	13
; CHECK-NEXT: .ascii	"test_double16"
; CHECK-NEXT: .long	1
; CHECK-NEXT: .long	88
; CHECK-NEXT: .long	128
; CHECK-NEXT: .long	128
; CHECK-NEXT: .long	8
; CHECK-NEXT: .ascii	"double16"
; CHECK-NEXT: .long	0

define amdgpu_kernel void @test_double16(<16 x double> %a) !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !15 !kernel_arg_base_type !15 !kernel_arg_type_qual !4 {
  ret void
}

; CHECK-LABEL:{{^}}test_pointer:
; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .long	12
; CHECK-NEXT: .ascii	"test_pointer"
; CHECK-NEXT: .long	1
; CHECK-NEXT: .long	16433
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	5
; CHECK-NEXT: .ascii	"int *"
; CHECK-NEXT: .long	0

define amdgpu_kernel void @test_pointer(i32 addrspace(1)* %a) !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !16 !kernel_arg_base_type !16 !kernel_arg_type_qual !4 {
  ret void
}

; CHECK-LABEL:{{^}}test_image:
; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .long	10
; CHECK-NEXT: .ascii	"test_image"
; CHECK-NEXT: .long	1
; CHECK-NEXT: .long	16386
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	9
; CHECK-NEXT: .ascii	"image2d_t"
; CHECK-NEXT: .long	0

define amdgpu_kernel void @test_image(%opencl.image2d_t addrspace(1)* %a) !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !17 !kernel_arg_base_type !17 !kernel_arg_type_qual !4 {
  ret void
}

; CHECK-LABEL:{{^}}test_sampler:
; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .long	12
; CHECK-NEXT: .ascii	"test_sampler"
; CHECK-NEXT: .long	1
; CHECK-NEXT: .long	51
; CHECK-NEXT: .long	4
; CHECK-NEXT: .long	4
; CHECK-NEXT: .long	9
; CHECK-NEXT: .ascii	"sampler_t"
; CHECK-NEXT: .long	0

define amdgpu_kernel void @test_sampler(i32 %a) !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !18 !kernel_arg_base_type !18 !kernel_arg_type_qual !4 {
  ret void
}

; CHECK-LABEL:{{^}}test_queue:
; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .long	10
; CHECK-NEXT: .ascii	"test_queue"
; CHECK-NEXT: .long	1
; CHECK-NEXT: .long	16388
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	7
; CHECK-NEXT: .ascii	"queue_t"
; CHECK-NEXT: .long	0

define amdgpu_kernel void @test_queue(%opencl.queue_t addrspace(1)* %a) !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !19 !kernel_arg_base_type !19 !kernel_arg_type_qual !4 {
  ret void
}

; CHECK-LABEL:{{^}}test_struct:
; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .long	11
; CHECK-NEXT: .ascii	"test_struct"
; CHECK-NEXT: .long	1
; CHECK-NEXT: .long	1
; CHECK-NEXT: .long	4
; CHECK-NEXT: .long	4
; CHECK-NEXT: .long	8
; CHECK-NEXT: .ascii	"struct A"
; CHECK-NEXT: .long	0

define amdgpu_kernel void @test_struct(%struct.A* byval %a) !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !20 !kernel_arg_base_type !20 !kernel_arg_type_qual !4 {
  ret void
}

; CHECK-LABEL:{{^}}test_i128:
; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .long	9
; CHECK-NEXT: .ascii	"test_i128"
; CHECK-NEXT: .long	1
; CHECK-NEXT: .long	0
; CHECK-NEXT: .long	16
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	4
; CHECK-NEXT: .ascii	"i128"
; CHECK-NEXT: .long	0

define amdgpu_kernel void @test_i128(i128 %a) !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !21 !kernel_arg_base_type !21 !kernel_arg_type_qual !4 {
  ret void
}

; CHECK-LABEL:{{^}}test_multi_arg:
; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .long	14
; CHECK-NEXT: .ascii	"test_multi_arg"
; CHECK-NEXT: .long	3
; CHECK-NEXT: .long	48
; CHECK-NEXT: .long	4
; CHECK-NEXT: .long	4
; CHECK-NEXT: .long	3
; CHECK-NEXT: .ascii	"int"
; CHECK-NEXT: .long	24
; CHECK-NEXT: .long	4
; CHECK-NEXT: .long	4
; CHECK-NEXT: .long	6
; CHECK-NEXT: .ascii	"short2"
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	4
; CHECK-NEXT: .long	4
; CHECK-NEXT: .long	5
; CHECK-NEXT: .ascii	"char3"
; CHECK-NEXT: .long	0

define amdgpu_kernel void @test_multi_arg(i32 %a, <2 x i16> %b, <3 x i8> %c) !kernel_arg_addr_space !22 !kernel_arg_access_qual !23 !kernel_arg_type !24 !kernel_arg_base_type !24 !kernel_arg_type_qual !25 {
  ret void
}

; CHECK-LABEL:{{^}}test_addr_space:
; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .long	15
; CHECK-NEXT: .ascii	"test_addr_space"
; CHECK-NEXT: .long	3
; CHECK-NEXT: .long	16433
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	5
; CHECK-NEXT: .ascii	"int *"
; CHECK-NEXT: .long	32817
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	5
; CHECK-NEXT: .ascii	"int *"
; CHECK-NEXT: .long	49201
; CHECK-NEXT: .long	4
; CHECK-NEXT: .long	4
; CHECK-NEXT: .long	5
; CHECK-NEXT: .ascii	"int *"
; CHECK-NEXT: .long	0

define amdgpu_kernel void @test_addr_space(i32 addrspace(1)* %g, i32 addrspace(2)* %c, i32 addrspace(3)* %l) !kernel_arg_addr_space !50 !kernel_arg_access_qual !23 !kernel_arg_type !51 !kernel_arg_base_type !51 !kernel_arg_type_qual !25 {
  ret void
}

; CHECK-LABEL:{{^}}test_type_qual:
; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .long	14
; CHECK-NEXT: .ascii	"test_type_qual"
; CHECK-NEXT: .long	3
; CHECK-NEXT: .long	17457
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	5
; CHECK-NEXT: .ascii	"int *"
; CHECK-NEXT: .long	17201
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	5
; CHECK-NEXT: .ascii	"int *"
; CHECK-NEXT: .long	18433
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	5
; CHECK-NEXT: .ascii	"int *"
; CHECK-NEXT: .long	0

define amdgpu_kernel void @test_type_qual(i32 addrspace(1)* %a, i32 addrspace(1)* %b, %opencl.pipe_t addrspace(1)* %c) !kernel_arg_addr_space !22 !kernel_arg_access_qual !23 !kernel_arg_type !51 !kernel_arg_base_type !51 !kernel_arg_type_qual !70 {
  ret void
}

; CHECK-LABEL:{{^}}test_access_qual:
; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .long	16
; CHECK-NEXT: .ascii	"test_access_qual"
; CHECK-NEXT: .long	3
; CHECK-NEXT: .long	20482
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	9
; CHECK-NEXT: .ascii	"image1d_t"
; CHECK-NEXT: .long	24578
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	9
; CHECK-NEXT: .ascii	"image2d_t"
; CHECK-NEXT: .long	28674
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	8
; CHECK-NEXT: .long	9
; CHECK-NEXT: .ascii	"image3d_t"
; CHECK-NEXT: .long	0

define amdgpu_kernel void @test_access_qual(%opencl.image1d_t addrspace(1)* %ro, %opencl.image2d_t addrspace(1)* %wo, %opencl.image3d_t addrspace(1)* %rw) !kernel_arg_addr_space !60 !kernel_arg_access_qual !61 !kernel_arg_type !62 !kernel_arg_base_type !62 !kernel_arg_type_qual !25 {
  ret void
}

; CHECK-LABEL:{{^}}test_reqd_wgs_vec_type_hint:
; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .long	27
; CHECK-NEXT: .ascii	"test_reqd_wgs_vec_type_hint"
; CHECK-NEXT: .long   1
; CHECK-NEXT: .long   48
; CHECK-NEXT: .long   4
; CHECK-NEXT: .long   4
; CHECK-NEXT: .long   3
; CHECK-NEXT: .ascii  "int"
; CHECK-NEXT: .long   5
; CHECK-NEXT: .long   1
; CHECK-NEXT: .long   2
; CHECK-NEXT: .long   4
; CHECK-NEXT: .long   3
; CHECK-NEXT: .ascii  "int"

define amdgpu_kernel void @test_reqd_wgs_vec_type_hint(i32 %a) !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !3 !kernel_arg_base_type !3 !kernel_arg_type_qual !4 !vec_type_hint !5 !reqd_work_group_size !6 {
  ret void
}

; CHECK-LABEL:{{^}}test_wgs_hint_vec_type_hint:
; CHECK: .section        .AMDGPU.runtime_metadata
; CHECK-NEXT: .long	27
; CHECK-NEXT: .ascii	"test_wgs_hint_vec_type_hint"
; CHECK-NEXT: .long   1
; CHECK-NEXT: .long   48
; CHECK-NEXT: .long   4
; CHECK-NEXT: .long   4
; CHECK-NEXT: .long   3
; CHECK-NEXT: .ascii  "int"
; CHECK-NEXT: .long   6
; CHECK-NEXT: .long   8
; CHECK-NEXT: .long   16
; CHECK-NEXT: .long   32
; CHECK-NEXT: .long   5
; CHECK-NEXT: .ascii  "uint4"

define amdgpu_kernel void @test_wgs_hint_vec_type_hint(i32 %a) !kernel_arg_addr_space !1 !kernel_arg_access_qual !2 !kernel_arg_type !3 !kernel_arg_base_type !3 !kernel_arg_type_qual !4 !vec_type_hint !7 !work_group_size_hint !8 {
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
!9 = !{!"char"}
!10 = !{!"ushort2"}
!11 = !{!"int3"}
!12 = !{!"ulong4"}
!13 = !{!"half8"}
!14 = !{!"float16"}
!15 = !{!"double16"}
!16 = !{!"int *"}
!17 = !{!"image2d_t"}
!18 = !{!"sampler_t"}
!19 = !{!"queue_t"}
!20 = !{!"struct A"}
!21 = !{!"i128"}
!22 = !{i32 0, i32 0, i32 0}
!23 = !{!"none", !"none", !"none"}
!24 = !{!"int", !"short2", !"char3"}
!25 = !{!"", !"", !""}
!50 = !{i32 1, i32 2, i32 3}
!51 = !{!"int *", !"int *", !"int *"}
!60 = !{i32 1, i32 1, i32 1}
!61 = !{!"read_only", !"write_only", !"read_write"}
!62 = !{!"image1d_t", !"image2d_t", !"image3d_t"}
!70 = !{!"volatile", !"const restrict", !"pipe"}
