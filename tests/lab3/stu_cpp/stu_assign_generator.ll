define i32 @main() {
label_entry:
  %op0 = alloca i32
  %op1 = alloca [10 x i32]
  %op2 = getelementptr [10 x i32], [10 x i32]* %op1, i32 0, i32 0
  store i32 10, i32* %op2
  %op3 = load i32, i32* %op2
  %op4 = mul i32 %op3, 2
  %op5 = getelementptr [10 x i32], [10 x i32]* %op1, i32 0, i32 1
  store i32 %op4, i32* %op5
  %op6 = load i32, i32* %op5
  %op7 = alloca i32
  store i32 %op6, i32* %op7
  %op8 = load i32, i32* %op7
  ret i32 %op8
}
