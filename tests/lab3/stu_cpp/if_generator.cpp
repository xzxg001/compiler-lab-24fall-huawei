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
  auto module = new Module("if code");  // 生成assign的代码
  auto builder = new IRBuilder(nullptr,module);
  Type* Int32Type = Type::get_int32_type(module);
  Type* floatType = Type::get_float_type(module);



    // BB的名字在生成中无所谓,但是可以方便阅读
  
    // main函数
  auto mainFun = Function::create(FunctionType::get(Int32Type, {}),
                                  "main", module);
  auto bb = BasicBlock::create(module, "entry", mainFun);                                
  
  // BasicBlock的名字在生成中无所谓,但是可以方便阅读
  builder->set_insert_point(bb);
  auto retAlloca = builder->create_alloca(Int32Type);
  auto ret_ca=builder->create_alloca(Int32Type);
  auto a_ca=builder->create_alloca(floatType);
  builder->create_store(CONST_FP(5.555),a_ca);
  auto aload=builder->create_load(a_ca);
  auto cmp=builder->create_fcmp_gt(aload,CONST_FP(1.000));
  auto true_bb=BasicBlock::create(module,"trueBB",mainFun);
  auto false_bb=BasicBlock::create(module,"false_BB",mainFun);
  auto ret_bb=BasicBlock::create(module,"retBB",mainFun);
  auto br=builder->create_cond_br(cmp,true_bb,false_bb);

  builder->set_insert_point(true_bb);
  builder->create_store(CONST_INT(233),ret_ca);
  builder->create_br(ret_bb);

  builder->set_insert_point(false_bb);
  builder->create_store(CONST_INT(0),ret_ca);
  builder->create_br(ret_bb);

  builder->set_insert_point(ret_bb);
  auto retload=builder->create_load(ret_ca);
  builder->create_ret(retload);

  std::cout<<module->print();
  delete module; 
  return 0;
}