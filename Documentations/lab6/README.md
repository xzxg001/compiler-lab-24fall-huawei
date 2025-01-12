# Lab 6 实验文档

鸣谢：USTC 张昱老师的毕昇编译课程实验。<http://staff.ustc.edu.cn/~yuzhang/compiler/lectures/bishengLab.pdf>

- [Lab 6 实验文档](#lab-6-实验文档)
  - [0. 前言](#0-前言)
  - [1. 实验框架](#1-实验框架)
  - [2. 运行与调试](#2-运行与调试)
    - [运行内存布局优化](#运行内存布局优化)
    - [运行反馈优化](#运行-PGO-反馈优化)
  - [3. 提交要求](#3-提交要求)

## 0. 前言

结构体、数组的内存布局由编译器控制，编译器可以结合结构体声明、访问特征以及微架构特征来灵活设计新的数据布局来改善数据局部性，从而提升缓存利用率。

假设结构体的字段 data1, data2, next 在一个结构体中顺序排列，对于结构体数组 array，其内存布局为 data1, data2,next, data1, data2, next, … 等字段交替排列。而当 array 被访问时，仅仅访问每个结构体的 data1 字段，此时每个对 data1 的访问都不是连续的，中间间隔了 data2, next 字段。这样的访存模式往往内存局部性较差，会引发较高的 cache miss。毕昇编译器的结构体内存布局优化可以将结构体数组重新布局为数组的结构体，从而保证每个 data1 字段的访问是内存连续的。

&nbsp;

PGO 是 Profile Guided Optimization 的缩写，即档案导引优化。是一种自适应优化手段，不需要对软件代码本身做出改进，即可获得性能的提升，目前已经应用于 GCC、VC++、Clang 等常见的编译器中。
配置文件信息可以帮助编译器更好地优化。例如，知道分支被非常频繁地执行，有助于编译器在排序基本块时做出更好的决策；知道函数 foo 的调用频率比另一个函数条的调用频率更高，有助于内联。此外，配置文件还能指导编译器更好的进行冷热分区、函数分区、寄存器分配、循环展开和剥离、增强代码和数据的局部相关性、Switch 语句扩展。建议使用-O2 以上的优
化等级使用 PGO。
毕昇支持两种类型的 PGO 采样。一种是使用采样分析器，可以在牺牲很小运行时开销情况下生成配置文件。二是重新构建代码获取一个插桩版本的二进制以收集更详细的配置文件信息。这两种配置文件都可以提供代码中指令的执行计数，以及关于所采取的分支和函数调用的信息。
使用时需要注意，插桩编译和反馈编译时的源代码要完全一致，有时编译选项也要一致，否则可能导致无法关联 profile 数据或做出糟糕的优化选择，最好是在代码通过了单元测试和回归测试后再使用。另外，若程序大部分时间花在函数调用和分支跳转时，PGO 优化收益大，若大部分时间在循环和计算时，PGO 收益较小。

## 1. 实验框架

本次实验中，内存布局优化和反馈优化分别使用了 `src/bisheng/memlayout.cpp` 和 `src/bisheng/feedback.cpp` 两个文件。

## 2. 运行与调试

### 运行内存布局优化

生成 Makefile 后，在项目根目录 `build` 文件夹下，运行：

```bash
make memlayout_optimized memlayout_unoptimized
```

然后运行，并计算时间：

```bash
time ./memlayout_optimized
time ./memlayout_unoptimized
```

如果发现时间差异不到 10%，可以尝试反汇编。如果反汇编未发现明显差异，可能我们拿到的编译器未实装该功能，可以构建并运行 `memlayout_optimized_example`，取代 `memlayout_optimized`。

PS：跑不出来明显差距是正常的，助教也没跑出来。当你看到这段文字的时候，关于此事还未得到华为方的回复。

### 运行 PGO 反馈优化

作为对比，我们设计了两个构建目标，分别是未优化的 `feedback_noOpt` 和使用 `-O2` 选项的 `feedback_base`。生成 Makefile 后，在项目根目录 `build` 文件夹下，运行：

```bash
make feedback_noOpt feedback_base
./feedback_noOpt
./feedback_base
```

分别观察时间。然后使用 PGO 优化：

```bash
make feedback_pgo_instrumented
LLVM_PROFILE_FILE="feedback-%p.profraw" ./feedback_pgo_instrumented
make merge_profdata
make feedback_pgo
./feedback_pgo
```

可以看到 PGO 反馈优化效果。

## 3. 提交要求

### 目录结构

TODO

### 提交要求和评分标准

**提交要求**：本实验仅需运行上面给出的代码，并在 `Reports/lab6` 中填写实验报告。

**评分标准**：实验报告中能体现两个优化效果即给满分，不必须做出解释。
