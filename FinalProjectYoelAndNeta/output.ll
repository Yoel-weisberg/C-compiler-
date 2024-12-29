; ModuleID = 'Yoel and neta JIT'
source_filename = "Yoel and neta JIT"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"

declare i32 @malloc(i32)

define i32 @malloc.1(i32 %amountToAllocate) {
entry:
  %heap_int = call i32 @malloc(i32 %amountToAllocate)
  ret i32 %heap_int
}
