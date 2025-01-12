define i32 @main() {
label_entry:
  %op0 = alloca i32
  %op1 = alloca i32
  %op2 = alloca float
  store float 0x40163851e0000000, float* %op2
  %op3 = load float, float* %op2
  %op4 = fcmp ugt float %op3,0x3ff0000000000000
  br i1 %op4, label %label_trueBB, label %label_false_BB
label_trueBB:                                                ; preds = %label_entry
  store i32 233, i32* %op1
  br label %label_retBB
label_false_BB:                                                ; preds = %label_entry
  store i32 0, i32* %op1
  br label %label_retBB
label_retBB:                                                ; preds = %label_trueBB, %label_false_BB
  %op5 = load i32, i32* %op1
  ret i32 %op5
}
