cmake_minimum_required(VERSION 3.10)
project(BishengProject)

# 设置 Clang 为编译器
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# ------------------------
# Prefetch 和 Loop Vectorization 目标
# ------------------------

# 普通的预取执行目标
add_executable(prefetch prefetch.cpp)
target_compile_options(prefetch PRIVATE -O3)

# 启用和禁用矢量化的循环目标
add_executable(loop_with_vectorization loop_vectorize.cpp)
target_compile_options(loop_with_vectorization PRIVATE -O3)

add_executable(loop_without_vectorization loop_vectorize.cpp)
target_compile_options(loop_without_vectorization PRIVATE -O3 -fno-vectorize)

# ------------------------
# Feedback 目标
# ------------------------

# feedback_noOpt 目标，未优化
add_executable(feedback_noOpt feedback.c)
target_compile_options(feedback_noOpt PRIVATE -O0)

# feedback_base 目标，使用 -O2
add_executable(feedback_base feedback.c)
target_compile_options(feedback_base PRIVATE -O2)

# feedback_pgo 插桩目标，使用 -O2 并启用 PGO 数据收集
add_executable(feedback_pgo_instrumented feedback.c)
target_compile_options(feedback_pgo_instrumented PRIVATE -O2 -fprofile-instr-generate)
target_link_options(feedback_pgo_instrumented PRIVATE -fprofile-instr-generate)

# 定义合并 .profraw 文件的目标
add_custom_target(merge_profdata
    COMMAND llvm-profdata merge -output=${CMAKE_BINARY_DIR}/feedback.profdata ${CMAKE_BINARY_DIR}/feedback-*.profraw
    COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_BINARY_DIR}/feedback-*.profraw
    DEPENDS feedback_pgo_instrumented
)

# feedback_pgo 目标，使用反馈数据的最终编译
add_executable(feedback_pgo feedback.c)
add_dependencies(feedback_pgo merge_profdata)  # 添加依赖，确保 merge_profdata 先完成
target_compile_options(feedback_pgo PRIVATE -O2 -fprofile-instr-use=${CMAKE_BINARY_DIR}/feedback.profdata)

set_target_properties(feedback_noOpt PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(feedback_base PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(feedback_pgo_instrumented PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(feedback_pgo PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(merge_profdata PROPERTIES EXCLUDE_FROM_ALL TRUE)

# ------------------------
# Floating Point Optimization (fpopt) 目标
# ------------------------

# fpopt 示例代码的不同优化级别目标
add_executable(fpopt_O2 fpopt.cpp)
target_compile_options(fpopt_O2 PRIVATE -O2)

add_executable(fpopt_Ofast fpopt.cpp)
target_compile_options(fpopt_Ofast PRIVATE -Ofast)

add_executable(fpopt_Ofast_fp_control fpopt.cpp)
target_compile_options(fpopt_Ofast_fp_control PRIVATE -Ofast -ffp-contract=off -fno-fast-math)

# ------------------------
# Memory Layout 优化目标
# ------------------------

# memlayout_optimized 目标，启用结构剥离优化
add_executable(memlayout_optimized memlayout.c)
target_compile_options(memlayout_optimized PRIVATE -O3 -flto)
target_link_options(memlayout_optimized PRIVATE -fuse-ld=lld)

# memlayout_unoptimized 目标，禁用结构剥离优化
add_executable(memlayout_unoptimized memlayout.c)
target_compile_options(memlayout_unoptimized PRIVATE -O3 -flto -Wl,-mllvm,-enable-struct-peel=false)
target_link_options(memlayout_unoptimized PRIVATE -fuse-ld=lld)

# 优化效果示意
add_executable(memlayout_optimized_example memlayout_opt.c)
target_compile_options(memlayout_optimized_example PRIVATE -O3 -flto)
target_link_options(memlayout_optimized_example PRIVATE -fuse-ld=lld)