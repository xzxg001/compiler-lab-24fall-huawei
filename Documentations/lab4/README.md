# Lab4 实验文档

- [Lab4 实验文档](#lab4-实验文档)
  - [0. 前言](#0-前言)
    - [主要工作](#主要工作)
    - [毕昇循环优化](#毕昇循环优化)
    - [毕昇软件预取](#毕昇软件预取)
  - [1. 实验框架](#1-实验框架)
    - [生成 IR](#生成-ir)
  - [2. 运行与调试](#2-运行与调试)
    - [运行 cminusfc](#运行-cminusfc)
    - [运行循环优化](#运行循环优化)
    - [运行软件预取](#运行软件预取)
    - [自动测试](#自动测试)
    - [logging](#logging)
    - [建议](#建议)
  - [3. 提交要求](#3-提交要求)
    - [目录结构](#目录结构)
    - [提交要求和评分标准](#提交要求和评分标准)

## 0. 前言

本次实验和 Lab3 一样，需要使用 `LightIR` 框架自动产生 `cminus-f` 语言的LLVM IR。

经过 Lab3 的练手，相信大家已经掌握了 LightIR 的使用，并且对于 LLVM IR 也有了一定理解。在本次实验中，我们要使用访问者模式来实现 IR 自动生成。
对于产生的IR， 我们可以调用 clang 生成可执行文件，这样一个初级的 cminus-f 编译器就完成啦！

### 主要工作

1. 阅读[cminus-f 的语义规则](../common/cminusf.md)<del>成为语言律师</del>，我们将按照语义实现程度进行评分
2. 阅读[LightIR 核心类介绍](../common/LightIR.md)
3. 阅读[实验框架](#1-实验框架)，理解如何使用框架以及注意事项
4. 修改 `src/cminusfc/cminusf_builder.cpp` 来实现自动 IR 产生的算法，使得它能正确编译任何合法的 cminus-f 程序
5. 利用毕昇循环优化工具和软件预取机制进行优化
6. 在 `report.md` 中解释你们的设计，遇到的困难和解决方案

### 毕昇循环优化

使用毕昇编译器时在链接选项中同时加上 `-lksvml -fveclib=MATHLIB -lkm -lm`，并添加 `-O3` 或 `-O2` 选项以提供向量接口。其中 `-O3`/`-O2` 是为了开启向量化优化，这是向量化数学库起效的前提。

### 毕昇软件预取

**本项实验不需要修改任何文件**，遵照 [运行软件预取](#运行软件预取) 中的指引进行操作即可。

这个功能基于 arm64 的 `PRFM` 指令实现。数据预取通常可以观察热点函数中LDR等数据加载指令的上下文代码，在代码中嵌入数据预取操作，常见的是在循环当中进行。在C/C++代码中，通常使用内嵌汇编形式调用预取指令，函数声明为inline形式，举例如下：

```cpp
// 从ptr处预读取128字节数据 
void inline Prefetch(int *ptr)  
{
    __asm__ volatile("prfm PLDL1KEEP, [%0, #(%1)]"::"r"(ptr), "i"(128));
}
```

PLDL1KEEP表示将数据预取到L1 cache中，策略为keep模式。数据使用完后，将保留一定时间，适用于数据多次使用的场景。

## 1. 实验框架

### 生成 IR

本次实验使用了由C++编写的 LightIR 来生成 LLVM IR。为了便于大家进行实验，该框架自动完成了语法树到 C++ 上的抽象语法树的转换。
我们可以使用[访问者模式](/Documentations/lab3/README.md#31-%E4%BA%86%E8%A7%A3visitor-pattern)来设计抽象语法树中的算法。大家可以参考[打印抽象语法树的算法](/src/common/ast.cpp#L394-737)，以及运行 `test_ast` 来理解访问者模式下算法的执行流程。

在`include/cminusf_builder.hpp`中，我还定义了一个用于存储作用域的类`Scope`。它的作用是辅助我们在遍历语法树时，管理不同作用域中的变量。它提供了以下接口：

```cpp
// 进入一个新的作用域
void enter();
// 退出一个作用域
void exit();
// 往当前作用域插入新的名字->值映射
bool push(std::string name, Value *val);
// 根据名字，寻找到值
Value* find(std::string name);
// 判断当前是否在全局作用域内
bool in_global();
```

你们需要根据语义合理调用`enter`与`exit`，并且在变量声明和使用时正确调用`push`与`find`。在类`CminusfBuilder`中，有一个`Scope`类型的成员变量`scope`，它在初始化时已经将`input`、`output`等函数加入了作用域中。因此，你们在进行名字查找时不需要顾虑是否需要对特殊函数进行特殊操作。


## 2. 运行与调试

### 运行 cminusfc

```sh
mkdir build && cd build
cmake .. -DLLVM_DIR=/path/to/your/llvm/install/lib/cmake/llvm/ # 根据你的环境修改
make -j
# 安装它以便于链接 libcminus_io.a
make install
```

编译后会产生 `cminusfc` 程序，它能将cminus文件输出为LLVM IR，也可以利用clang将IR编译成二进制。程序逻辑写在`src/cminusfc/cminusfc.cpp`中。

当需要对 `.cminus` 文件测试时，可以这样使用：

```sh
# 假设 cminusfc 的路径在你的$PATH中
# 利用构建好的 Module 生成 test.ll
# 注意，如果调用了外部函数，如 input, output 等，则无法使用lli运行
cminusfc test.cminus -emit-llvm

# 假设libcminus_io.a的路径在$LD_LIBRARY_PATH中，clang的路径在$PATH中
# 1. 利用构建好的 Module 生成 test.ll
# 2. 调用 clang 来编译 IR 并链接上静态链接库 libcminus_io.a，生成二进制文件 test
cminusfc test.cminus
```
PS：由于非 root 用户的限制，`cminusfc` **很可能不在** `PATH` 里。你可以手动指定 `build` 文件夹里的 `cminusfc` 来运行。

PS：由于非 root 用户的限制，`cminusfc` **很可能不在** `PATH` 里。你可以手动指定 `build` 文件夹里的 `cminusfc` 来运行。

### 运行循环优化

**由于华为不开放其数学库使用，此处代码仅供用法示意。学生无需进行任何操作。**对于代码

```cpp
#include <km.h>
#include <stdio.h>
#include <stdlib.h>
int main()
{
        long loop = 1e7;
        int len = 8192;
        float *a = (float*)malloc(sizeof(float) * len);
        float *b = (float*)malloc(sizeof(float) * len);
        float *d = (float*)malloc(sizeof(float) * len);
        for (int i = 0; i < len; i++) {
                a[i] = rand() * 7.7680f - 6.3840f;
                b[i] = rand() * 8.7680f - 6.3840f;
                d[i] = 0;
        }
        for (int i = 0; i < loop; i++) {
                for (int j = 0; j < len; j++) {
                        d[j] = expf(a[j]);
                }
        }
        return 1;
}
```

使用编译指令 `clang test.c -lksvml -fveclib=MATHLIB -lkm -lm -O3`，即可生成带有毕昇数学库循环优化的二进制文件。

### 运行软件预取

生成 Makefile 后，在项目根目录 `build` 文件夹下，运行：

```sh
make prefetch
./prefetch
```

即可看到毕昇软件预取对性能的提升。你仅需截取运行结果，放入实验报告中即可。

### 自动测试

助教贴心地为大家准备了自动测试脚本，它在 `tests/lab4` 目录下，使用方法如下：

```sh
# 在 tests/lab4 目录下运行：
./lab4_test.py
```

如果生成的 IR 完全正确，它会输出：

```
===========TEST START===========
Case 01: Success
Case 02: Success
Case 03: Success
Case 04: Success
Case 05: Success
Case 06: Success
Case 07: Success
Case 08: Success
Case 09: Success
Case 10: Success
Case 11: Success
Case 12: Success
============TEST END============
```

通过修改脚本，还可以方便地添加自定义测试用例

 **请注意助教提供的测试样例仅涵盖了最基础的测试情况，请自行设计测试样例进行测试。**

### logging

[logging](../common/logging.md) 是帮助大家打印调试信息的工具，如有需求可以阅读文档后进行使用

### 建议

1. 比较你们编写的编译器产生的 IR 和 clang 产生的IR来找出可能的问题或发现新的思路
2. 使用 logging 工具来打印调试信息
3. 使用 GDB 等软件进行单步调试来检查错误的原因
4. 合理分工

## 3. 提交要求

### 目录结构

```
.
├── CMakeLists.txt
├── Documentations
│   ├── ...
│   ├── common
│   |   ├── LightIR.md                  <- LightIR 相关文档
│   |   ├── logging.md                  <- logging 工具相关文档
│   |   └── cminusf.md                  <- cminus-f 的语法和语义文档
│   └── lab4
│       └── README.md                   <- lab4 实验文档说明（你在这里）
├── include                             <- 实验所需的头文件
│   ├── ...
│   ├── lightir/*
│   ├── cminusf_builder.hpp
|   └── ast.hpp
├── Reports
│   ├── ...
│   └── lab4
│       └── report.md                   <- lab4 所需提交的实验报告，请详细说明你们的设计（需要上交）
├── src
│   ├── ...
│   └── cminusfc
│       ├── cminusfc.cpp                <- cminusfc 的主程序文件
│       └── cminusf_builder.cpp         <- lab4 需要修改的文件，你们要在该文件中用访问者模式实现自动 IR 生成的算法（需要上交）
└── tests
    ├── ...
    └── lab4
        ├── testcases                   <- 助教提供的测试样例
        └── lab4_test.py                <- 助教提供的测试脚本

```

### 提交要求和评分标准

- 提交要求  
  - 实验部分:
    - 需要填补 `./src/cminusfc/cminusf_builder.cpp`
    - 需要在 `./Reports/lab4/` 目录下撰写实验报告
    - 本次实验收取 `./src/cminusfc/cminusf_builder.cpp` 文件和 `./Reports/lab4` 目录

- 评分标准:
  - 实验完成分（总分 60 分）组成如下：
    - cminusf 程序测试 (45分)
    - `report.md`(15分)
  - 禁止执行恶意代码，违者本次实验0分处理

- 迟交规定
  - `Soft Deadline` : 【未定】
  - `Hard Deadline` : 【未定】
  - 迟交需要邮件通知TA:
    - 邮箱: 【各自班级助教邮箱】
    - 邮件主题: lab4迟交-学号
    - 内容: 包括迟交原因、最后版本commitID、迟交时间等

  - 迟交分数
    - x为迟交天数(对于`Soft Deadline`而言)，grade为满分

      ``` bash
      final_grade = grade, x = 0
      final_grade = grade * (0.9)^x, 0 < x <= 7
      final_grade = 0, x > 7 # 这一条严格执行,请对自己负责
      ```

- 关于抄袭和雷同
  经过助教和老师判定属于实验抄袭或雷同情况，所有参与方一律零分，不接受任何解释和反驳。
如有任何问题，欢迎提issue进行批判指正。
