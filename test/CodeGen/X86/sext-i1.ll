; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc < %s -mtriple=i386-unknown-unknown -disable-cgp-branch-opts    | FileCheck %s --check-prefix=CHECK --check-prefix=X32
; RUN: llc < %s -mtriple=x86_64-unknown-unknown -disable-cgp-branch-opts  | FileCheck %s --check-prefix=CHECK --check-prefix=X64

; rdar://7573216
; PR6146

define i32 @t1(i32 %x) nounwind readnone ssp {
; X32-LABEL: t1:
; X32:       # BB#0:
; X32-NEXT:    cmpl $1, {{[0-9]+}}(%esp)
; X32-NEXT:    sbbl %eax, %eax
; X32-NEXT:    retl
;
; X64-LABEL: t1:
; X64:       # BB#0:
; X64-NEXT:    cmpl $1, %edi
; X64-NEXT:    sbbl %eax, %eax
; X64-NEXT:    retq
  %t0 = icmp eq i32 %x, 0
  %if = select i1 %t0, i32 -1, i32 0
  ret i32 %if
}

define i32 @t2(i32 %x) nounwind readnone ssp {
; X32-LABEL: t2:
; X32:       # BB#0:
; X32-NEXT:    cmpl $1, {{[0-9]+}}(%esp)
; X32-NEXT:    sbbl %eax, %eax
; X32-NEXT:    retl
;
; X64-LABEL: t2:
; X64:       # BB#0:
; X64-NEXT:    cmpl $1, %edi
; X64-NEXT:    sbbl %eax, %eax
; X64-NEXT:    retq
  %t0 = icmp eq i32 %x, 0
  %if = sext i1 %t0 to i32
  ret i32 %if
}

define i32 @t3() nounwind readonly {
; X32-LABEL: t3:
; X32:       # BB#0: # %entry
; X32-NEXT:    cmpl $1, %eax
; X32-NEXT:    sbbl %eax, %eax
; X32-NEXT:    cmpl %eax, %eax
; X32-NEXT:    sbbl %eax, %eax
; X32-NEXT:    xorl %eax, %eax
; X32-NEXT:    retl
;
; X64-LABEL: t3:
; X64:       # BB#0: # %entry
; X64-NEXT:    xorl %eax, %eax
; X64-NEXT:    testl %eax, %eax
; X64-NEXT:    sete %al
; X64-NEXT:    negq %rax
; X64-NEXT:    cmpq %rax, %rax
; X64-NEXT:    xorl %eax, %eax
; X64-NEXT:    retq
entry:
  %not.tobool = icmp eq i32 undef, 0
  %cond = sext i1 %not.tobool to i32
  %conv = sext i1 %not.tobool to i64
  %add13 = add i64 0, %conv
  %cmp = icmp ult i64 undef, %add13
  br i1 %cmp, label %if.then, label %if.end

if.then:
  br label %if.end

if.end:
  %xor27 = xor i32 undef, %cond
  ret i32 0
}

define i32 @t4(i64 %x) nounwind readnone ssp {
; X32-LABEL: t4:
; X32:       # BB#0:
; X32-NEXT:    movl {{[0-9]+}}(%esp), %ecx
; X32-NEXT:    xorl %eax, %eax
; X32-NEXT:    orl {{[0-9]+}}(%esp), %ecx
; X32-NEXT:    sete %al
; X32-NEXT:    negl %eax
; X32-NEXT:    retl
;
; X64-LABEL: t4:
; X64:       # BB#0:
; X64-NEXT:    cmpq $1, %rdi
; X64-NEXT:    sbbl %eax, %eax
; X64-NEXT:    retq
  %t0 = icmp eq i64 %x, 0
  %t1 = sext i1 %t0 to i32
  ret i32 %t1
}

define i64 @t5(i32 %x) nounwind readnone ssp {
; X32-LABEL: t5:
; X32:       # BB#0:
; X32-NEXT:    cmpl $1, {{[0-9]+}}(%esp)
; X32-NEXT:    sbbl %eax, %eax
; X32-NEXT:    movl %eax, %edx
; X32-NEXT:    retl
;
; X64-LABEL: t5:
; X64:       # BB#0:
; X64-NEXT:    cmpl $1, %edi
; X64-NEXT:    sbbq %rax, %rax
; X64-NEXT:    retq
  %t0 = icmp eq i32 %x, 0
  %t1 = sext i1 %t0 to i64
  ret i64 %t1
}

; sext (xor Bool, -1) --> sub (zext Bool), 1

define i32 @select_0_or_1s(i1 %cond) {
; X32-LABEL: select_0_or_1s:
; X32:       # BB#0:
; X32-NEXT:    movzbl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    andl $1, %eax
; X32-NEXT:    decl %eax
; X32-NEXT:    retl
;
; X64-LABEL: select_0_or_1s:
; X64:       # BB#0:
; X64-NEXT:    # kill: %EDI<def> %EDI<kill> %RDI<def>
; X64-NEXT:    andl $1, %edi
; X64-NEXT:    leal -1(%rdi), %eax
; X64-NEXT:    retq
  %not = xor i1 %cond, 1
  %sext = sext i1 %not to i32
  ret i32 %sext
}

; sext (xor Bool, -1) --> sub (zext Bool), 1

define i32 @select_0_or_1s_zeroext(i1 zeroext %cond) {
; X32-LABEL: select_0_or_1s_zeroext:
; X32:       # BB#0:
; X32-NEXT:    movzbl {{[0-9]+}}(%esp), %eax
; X32-NEXT:    decl %eax
; X32-NEXT:    retl
;
; X64-LABEL: select_0_or_1s_zeroext:
; X64:       # BB#0:
; X64-NEXT:    movzbl %dil, %eax
; X64-NEXT:    decl %eax
; X64-NEXT:    retq
  %not = xor i1 %cond, 1
  %sext = sext i1 %not to i32
  ret i32 %sext
}

; sext (xor Bool, -1) --> sub (zext Bool), 1

define i32 @select_0_or_1s_signext(i1 signext %cond) {
; X32-LABEL: select_0_or_1s_signext:
; X32:       # BB#0:
; X32-NEXT:    movb {{[0-9]+}}(%esp), %al
; X32-NEXT:    andb $1, %al
; X32-NEXT:    movzbl %al, %eax
; X32-NEXT:    decl %eax
; X32-NEXT:    retl
;
; X64-LABEL: select_0_or_1s_signext:
; X64:       # BB#0:
; X64-NEXT:    andb $1, %dil
; X64-NEXT:    movzbl %dil, %eax
; X64-NEXT:    decl %eax
; X64-NEXT:    retq
  %not = xor i1 %cond, 1
  %sext = sext i1 %not to i32
  ret i32 %sext
}

