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
  auto module = new Module("assign code");  // 生成assign的代码
  auto builder = new IRBuilder(nullptr, module);
  Type *Int32Type = Type::get_int32_type(module);


    // BB的名字在生成中无所谓,但是可以方便阅读
  
    // main函数
  auto mainFun = Function::create(FunctionType::get(Int32Type, {}),
                                  "main", module);
  auto bb = BasicBlock::create(module, "entry", mainFun);                                
  
  // BasicBlock的名字在生成中无所谓,但是可以方便阅读
  builder->set_insert_point(bb);
  auto retAlloca = builder->create_alloca(Int32Type);

  auto *arrayType = ArrayType::get(Int32Type,10);
  auto a_array = builder->create_alloca(arrayType);
  auto aa = builder->create_gep(a_array,{CONST_INT(0),CONST_INT(0)});
  builder->create_store(CONST_INT(10),aa);
  auto a0load = builder->create_load(aa);
  auto a1data = builder->create_imul(a0load,CONST_INT(2));
  auto a1p = builder->create_gep(a_array,{CONST_INT(0),CONST_INT(1)});
  builder->create_store(a1data,a1p);
  auto a1load =builder->create_load(a1p);
  auto retp = builder->create_alloca(Int32Type);
  builder->create_store(a1load, retp); 
  auto retdata=builder->create_load(retp);
  builder->create_ret(retdata);

  std::cout<<module->print();
  delete module; 
  return 0;
}