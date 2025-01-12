define i32 @callee(i32 %arg0) {
label_entry:
  %op1 = alloca i32
  store i32 %arg0, i32* %op1
  %op2 = load i32, i32* %op1
  %op3 = mul i32 2, %op2
  %op4 = alloca i32
  store i32 %op3, i32* %op4
  %op5 = load i32, i32* %op4
  ret i32 %op5
}
define i32 @main() {
label_main:
  %op0 = alloca i32
  store i32 110, i32* %op0
  %op1 = load i32, i32* %op0
  %op2 = call i32 @callee(i32 %op1)
  ret i32 %op2
}
