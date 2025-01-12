; ModuleID = '/home/zoulinzhuang/compiler-lab-24fall/tests/lab3/ta_gcd/assign.c'
source_filename = "/home/zoulinzhuang/compiler-lab-24fall/tests/lab3/ta_gcd/assign.c"
target datalayout = "e-m:e-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128"
target triple = "aarch64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca float, align 4
  store float 0x40163851E0000000, ptr %1, align 4
  %2 = alloca i32 ,align 4
  store i32 233 ,ptr %2,align 4
  %3= alloca i32 ,align 4
  store i32 0 ,ptr %3,align 4
  %4= load float,ptr %1,align 4
  %5 = fcmp contract ogt float %4, 1.000000e+00
  br i1 %5,label %6,label %8

6:
  %7=load i32,ptr %2,align 4
  ret i32 %7 
  
8:
  %9=load i32,ptr %3,align 4
  ret i32 %9
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+fp-armv8,+neon,+outline-atomics,+v8a,-fmv" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 1}
!5 = !{!"BiSheng Enterprise 4.0.0.B014 clang version 17.0.6 (0ac87bc45325)"}