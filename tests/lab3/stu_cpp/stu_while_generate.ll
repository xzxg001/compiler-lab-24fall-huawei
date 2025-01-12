define i32 @main() {
label_entry:
  %op0 = alloca i32
  %op1 = alloca i32
  %op2 = alloca i32
  store i32 10, i32* %op2
  %op3 = alloca i32
  store i32 0, i32* %op3
  br label %label_run_bb
label_true_bb:                                                ; preds = %label_run_bb
  %op4 = load i32, i32* %op2
  %op5 = add i32 %op4, 1
  store i32 %op5, i32* %op2
  %op6 = add i32 %op8, 1
  store i32 %op6, i32* %op3
  br label %label_run_bb
label_false_bb:                                                ; preds = %label_run_bb
  %op7 = load i32, i32* %op2
  ret i32 %op7
label_run_bb:                                                ; preds = %label_entry, %label_true_bb
  %op8 = load i32, i32* %op3
  %op9 = icmp slt i32 %op8, 10
  br i1 %op9, label %label_true_bb, label %label_false_bb
}
