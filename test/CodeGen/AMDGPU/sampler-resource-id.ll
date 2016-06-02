; RUN: llc -march=r600 -mcpu=juniper < %s | FileCheck -check-prefix=EG -check-prefix=FUNC %s

; FUNC-LABEL: {{^}}test_0:
; EG: MEM_RAT_CACHELESS STORE_RAW [[VAL:T[0-9]+\.X]]
; EG: MOV [[VAL]], literal.x
; EG-NEXT: LSHR
; EG-NEXT: 0(
define void @test_0(i32 %in0, i32 addrspace(1)* %out) !kernel_arg_addr_space !10 !kernel_arg_access_qual !20 !kernel_arg_type !30 !kernel_arg_base_type !40 !kernel_arg_type_qual !50 {
entry:
  %0 = call i32 @llvm.OpenCL.sampler.get.resource.id(i32 %in0) #0
  store i32 %0, i32 addrspace(1)* %out
  ret void
}

; FUNC-LABEL: {{^}}test_1:
; EG: MEM_RAT_CACHELESS STORE_RAW [[VAL:T[0-9]+\.X]]
; EG: MOV [[VAL]], literal.x
; EG-NEXT: LSHR
; EG-NEXT: 1(
define void @test_1(i32 %in0, i32 %in1, i32 addrspace(1)* %out) !kernel_arg_addr_space !11 !kernel_arg_access_qual !21 !kernel_arg_type !31 !kernel_arg_base_type !41 !kernel_arg_type_qual !51 {
entry:
  %0 = call i32 @llvm.OpenCL.sampler.get.resource.id(i32 %in1) #0
  store i32 %0, i32 addrspace(1)* %out
  ret void
}

; FUNC-LABEL: {{^}}test_2:
; EG: MEM_RAT_CACHELESS STORE_RAW [[VAL:T[0-9]+\.X]]
; EG: MOV [[VAL]], literal.x
; EG-NEXT: LSHR
; EG-NEXT: 2(
define void @test_2(i32 %in0, i32 %in1, i32 %in2, i32 addrspace(1)* %out) !kernel_arg_addr_space !12 !kernel_arg_access_qual !22 !kernel_arg_type !32 !kernel_arg_base_type !42 !kernel_arg_type_qual !52 {

entry:
  %0 = call i32 @llvm.OpenCL.sampler.get.resource.id(i32 %in2) #0
  store i32 %0, i32 addrspace(1)* %out
  ret void
}


declare i32 @llvm.OpenCL.sampler.get.resource.id(i32) #0

attributes #0 = { readnone }

!10 = !{i32 0, i32 1}
!20 = !{!"none", !"none"}
!30 = !{!"sampler_t", !"int*"}
!40 = !{!"sampler_t", !"int*"}
!50 = !{!"", !""}

!11 = !{i32 0, i32 0, i32 1}
!21 = !{!"none", !"none", !"none"}
!31 = !{!"sampler_t", !"sampler_t", !"int*"}
!41 = !{!"sampler_t", !"sampler_t", !"int*"}
!51 = !{!"", !"", !""}

!12 = !{i32 0, i32 0, i32 0, i32 1}
!22 = !{!"none", !"none", !"none", !"none"}
!32 = !{!"sampler_t", !"sampler_t", !"sampler_t", !"int*"}
!42 = !{!"sampler_t", !"sampler_t", !"sampler_t", !"int*"}
!52 = !{!"", !"", !"", !""}
