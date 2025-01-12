#include "BasicBlock.h"
#include "Constant.h"
#include "Function.h"
#include "IRBuilder.h"
#include "Module.h"
#include "Type.h"

#include <iostream>
#include <memory>

#ifdef DEBUG  // 用于调试信息,大家可以在编译过程中通过" -DDEBUG"来开启这一选项
#define DEBUG_OUTPUT std::cout << __LINE__ << std::endl;  // 输出行号的简单示例
#else
#define DEBUG_OUTPUT
#endif

#define CONST_INT(num) \
    ConstantInt::get(num, module)

#define CONST_FP(num) \
    ConstantFP::get(num, module) // 得到常数值的表示,方便后面多次用到

int main() {
  auto module = new Module("fun code");  // 生成assign的代码
  auto builder = new IRBuilder(nullptr, module);
  Type *Int32Type = Type::get_int32_type(module);
  
  
  std::vector<Type *> Ints(1, Int32Type);
  auto callee_funtype=FunctionType::get(Int32Type,Ints);
  auto callee_fun=Function::create(FunctionType::get(Int32Type,{Int32Type}),"callee",module);
  auto bb =BasicBlock::create(module,"entry",callee_fun);
  builder->set_insert_point(bb);
  auto a_ca =builder->create_alloca(Int32Type);

  std::vector<Value *> args;  // 获取gcd函数的形参,通过Function中的iterator
  for (auto arg = callee_fun->arg_begin(); arg != callee_fun->arg_end(); arg++) {
    args.push_back(*arg);   // * 号运算符是从迭代器中取出迭代器当前指向的元素
  }
  builder->create_store(args[0],a_ca);
  auto aload=builder->create_load(a_ca);
  auto ans=builder->create_imul(CONST_INT(2),aload);
  auto retAlloca = builder->create_alloca(Int32Type);   // 在内存中分配返回值的位置
  builder->create_store(ans,retAlloca);
  auto retload=builder->create_load(retAlloca);
  builder->create_ret(retload);

  auto main_fun=Function::create(FunctionType::get(Int32Type,{ }),"main",module);
  auto bb1=BasicBlock::create(module,"main",main_fun);
  builder->set_insert_point(bb1);
  auto tmp=builder->create_alloca(Int32Type);
  builder->create_store(CONST_INT(110),tmp);
  auto tmpload=builder->create_load(tmp);
  auto res=builder->create_call(callee_fun,{tmpload});
  builder->create_ret(res);

  std::cout<<module->print();
  delete module;
  return 0;
    // BB的名字在生成中无所谓,但是可以方便阅读
}
