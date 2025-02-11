; ModuleID = 'Yoel and neta JIT'
source_filename = "Yoel and neta JIT"

declare ptr @malloc(i32)

declare i32 @scanf(ptr)

define i32 @foo() {
entry:
  %hey = alloca i32, align 4
  store i32 3, ptr %hey, align 4
  %hey1 = load i32, ptr %hey, align 4
  ret i32 %hey1
}

define void @main() {
entry:
  %calltmp = call i32 @foo()
  ret void
}
