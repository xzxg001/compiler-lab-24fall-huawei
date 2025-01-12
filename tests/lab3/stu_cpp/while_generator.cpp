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
  auto module = new Module("while code");  // 生成assign的代码
  auto builder = new IRBuilder(nullptr,module);
  Type* Int32Type = Type::get_int32_type(module);

  
    // main函数
  auto mainFun = Function::create(FunctionType::get(Int32Type, {}),
                                  "main", module);
  auto bb = BasicBlock::create(module, "entry", mainFun);                                
  
  // BasicBlock的名字在生成中无所谓,但是可以方便阅读
  builder->set_insert_point(bb);
  auto retAlloca = builder->create_alloca(Int32Type);
  auto ret_ca=builder->create_alloca(Int32Type);
  auto a_ca=builder->create_alloca(Int32Type);
  builder->create_store(CONST_INT(10),a_ca);
  auto i_ca=builder->create_alloca(Int32Type);
  builder->create_store(CONST_INT(0),i_ca);
  
  //
  
  auto true_bb=BasicBlock::create(module,"true_bb",mainFun);
  auto false_bb=BasicBlock::create(module,"false_bb",mainFun);
  auto run_bb=BasicBlock::create(module,"run_bb",mainFun); 
  builder->create_br(run_bb);

  builder->set_insert_point(run_bb);
  auto iload=builder->create_load(i_ca);
  auto cmp=builder->create_icmp_lt(iload,CONST_INT(10));
  auto br=builder->create_cond_br(cmp,true_bb,false_bb);
  
  builder->set_insert_point(true_bb);
  auto aload=builder->create_load(a_ca);
  auto ans=builder->create_iadd(aload,CONST_INT(1));
  builder->create_store(ans,a_ca);
  auto res=builder->create_iadd(iload,CONST_INT(1));
  builder->create_store(res,i_ca);
  builder->create_br(run_bb);

  builder->set_insert_point(false_bb);
  auto retload=builder->create_load(a_ca);
  builder->create_ret(retload);

  std::cout<<module->print();
  delete module; 
  return 0;
}