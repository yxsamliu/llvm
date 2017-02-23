; RUN: llvm-as < %s | llvm-dis | FileCheck %s

target datalayout = "A1"
; CHECK: target datalayout = "A1"
