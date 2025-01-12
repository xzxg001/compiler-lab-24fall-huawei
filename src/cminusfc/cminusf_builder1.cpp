#include "cminusf_builder.hpp"
#include "logging.hpp"
// use these macros to get constant value
#define CONST_INT(num) \
    ConstantInt::get(num, module.get()) /* 增加一个有关整型的宏 */
#define CONST_FP(num) \
    ConstantFP::get((float)num, module.get())
#define CONST_ZERO(var_type) \
    ConstantZero::get(var_type, module.get())
#define TYPTRINT \
    Type::get_int32_ptr_type(module.get())
#define TYPTRFLOAT \
    Type::get_float_ptr_type(module.get())
#define TYVOID \
    Type::get_void_type(module.get())
#define TYINT32 \
    Type::get_int32_type(module.get())
#define TYFLOAT \
    Type::get_float_type(module.get())
    
/*
        auto TyVoid = Type::get_void_type(module.get());
        auto TyInt32 = Type::get_int32_type(module.get());
        auto TyFloat = Type::get_float_type(module.get());

*/
#define checkInt(num)\
    num->get_type()->is_integer_type()
#define checkFloat(num) \
    num->get_type()->is_float_type() /* 浮点型判断 */
#define checkPointer(num) \
    num->get_type()->is_pointer_type() /* 指针类型判断 */

// You can define global variables here
// to store state

Value* res;
//ASTNum弄出来的值存在这里，因为所有函数的类型都是res的
Value *arg; //参数指针，表示参数列表 
bool pre_enter_scope = false;
bool if_return_notadd=false;//表示返回得到为地址还是返回值，用于后续判断
/*
 * use CMinusfBuilder::Scope to construct scopes
 * scope.enter: enter a new scope
 * scope.exit: exit current scope
 * scope.push: add a new binding to current scope
 * scope.find: find and return the value bound to the name
 */
// types
Type *VOID_T;
Type *INT1_T;
Type *INT32_T;
Type *INT32PTR_T;
Type *FLOAT_T;
Type *FLOATPTR_T;

//program->declaration-list
void CminusfBuilder::visit(ASTProgram &node) {
    LOG(DEBUG) << "Program\n";

    VOID_T = Type::get_void_type(module.get());
    INT1_T = Type::get_int1_type(module.get());
    INT32_T = Type::get_int32_type(module.get());
    INT32PTR_T = Type::get_int32_ptr_type(module.get());
    FLOAT_T = Type::get_float_type(module.get());
    FLOATPTR_T = Type::get_float_ptr_type(module.get());
    //语义检查
    if(node.declarations.size()==0){
        LOG(ERROR) << "程序缺少声明\n";
        return;
    }
    if(!(node.declarations.back()->id=="main"&&node.declarations.back()->type==TYPE_VOID))
{
    LOG(ERROR) << "程序最后一个声明不是void main(void)\n";
    return;
}
    for(auto decl : node.declarations){
        decl->accept(*this);//solve decl
    }


 }
//num
void CminusfBuilder::visit(ASTNum &node) { 
    LOG(DEBUG) << "Num\n";
    if(node.type==TYPE_INT)
    res=ConstantInt::get(node.i_val,module.get());
    else if(node.type==TYPE_FLOAT)
    res=ConstantFP::get(node.f_val,module.get());
    else LOG(ERROR) <<"缺少num\n";
}
//var-declaration -> type-specifier ID | type-specifier ID [INTEGER]
void CminusfBuilder::visit(ASTVarDeclaration &node) {
    LOG(DEBUG)<<"var-declration\n";
    Type *tmp;//类型指针
    if(node.type==TYPE_INT)tmp=TYINT32;
    else if(node.type==TYPE_FLOAT)tmp=TYFLOAT;
    else LOG(ERROR)<<"变量声明中只有整型和浮点型可以使用\n";
    //通过判断其是否具有num，区分为数组和非数组
    if(node.num!=nullptr){
        if(node.num->i_val<=0)LOG(ERROR)<<"数组长度必须大于0\n";
        auto *arrayType=ArrayType::get(tmp,node.num->i_val);//根据数组长度得到类型
        auto init=CONST_ZERO(tmp);//全局变量初始化为0
        Value *arrayAlloca;//存储申请到的数组的地址
        if(scope.in_global()){
            arrayAlloca=GlobalVariable::create(node.id,module.get(),arrayType,false,init);
            scope.push(node.id,arrayAlloca);
        }
        else arrayAlloca=builder->create_alloca(arrayType);
        scope.push(node.id,arrayAlloca);//将获得的数组变量加入域
    }
    else{
        auto init=CONST_ZERO(tmp);
        Value *varAlloca;//存储申请到的变量的地址
        if(scope.in_global()){
            varAlloca=GlobalVariable::create(node.id,module.get(),tmp,false,init);
            scope.push(node.id,varAlloca);
        }
        else {varAlloca=builder->create_alloca(tmp);
        scope.push(node.id,varAlloca);
        }
    }

 }
//fun-declaration—>type-specifier ID ( params ) compound-stmt
void CminusfBuilder::visit(ASTFunDeclaration &node) { 
    LOG(DEBUG) <<"fun-declaration\n";
    Type *retType;//设定返回值类型
    if(node.type==TYPE_INT)retType=TYINT32;
    else if(node.type==TYPE_FLOAT)retType=TYFLOAT;
    else if(node.type==TYPE_VOID)retType=TYVOID;
    else LOG(ERROR)<<"函数返回类型只能为int,float,void";

    //根据函数声明，构造形参类型列表
    std::vector<Type *>paramsType;
    for(auto param:node.params){
        if(param->isarray){
            if(param->type==TYPE_INT)paramsType.push_back(TYPTRINT);
            else paramsType.push_back(TYPTRFLOAT);
        }else{
            if(param->type==TYPE_INT)paramsType.push_back(TYINT32);
            else paramsType.push_back(TYFLOAT);
        }
    }
    auto funType =FunctionType::get(retType,paramsType);//retType返回结构
    auto function=Function::create(funType,node.id,module.get());//创建函数
    scope.push(node.id,function);
    
    auto bb=BasicBlock::create(module.get(),"entry",function);//创建基本块
    builder->set_insert_point(bb);
    scope.enter();
    pre_enter_scope = true;
    std::vector<Value *> args;//创建vector存储实参
    for(auto arg=function->arg_begin();arg != function->arg_end(); arg++)args.push_back(*arg);

    for (int i = 0; i < node.params.size(); i++)
    {                                /* 遍历形参列表 */
        auto param = node.params[i]; /* 取出对应形参 */
        arg = args[i];               /* 取出对应实参 */
        param->accept(*this);        /* 调用param的accept进行处理 */
    }
        node.compound_stmt->accept(*this); /* 处理函数体内语句compound-stmt */
    // 判断返回值的类型，根据对应的返回值类型，执行ret操作
    
    if (builder->get_insert_block()->get_terminator() == nullptr)//判断是否没有显式返回
    {
        if (function->get_return_type()->is_void_type())// 若没有显式返回，默认返回对应类型的0或void
            builder->create_void_ret();
        else if (function->get_return_type()->is_float_type())
            builder->create_ret(CONST_FP(0.0));
        else
            builder->create_ret(CONST_INT(0));
    }
    scope.exit(); //退出此函数作用域 

}
//param→type-specifier ID | type-specifier ID []
void CminusfBuilder::visit(ASTParam &node) { 
    LOG(DEBUG) <<"Param\n";

    Value *paramAlloca;//分配参数的存储空间
    if(node.isarray){//如果是数组
        if(node.type==TYPE_INT)paramAlloca=builder->create_alloca(TYPTRINT);
        else if(node.type==TYPE_FLOAT)paramAlloca=builder->create_alloca(TYPTRFLOAT);
    }else {//如果不是数组
        if(node.type==TYPE_INT)paramAlloca=builder->create_alloca(TYINT32);
        else if(node.type==TYPE_FLOAT)paramAlloca=builder->create_alloca(TYFLOAT);
    }

    builder->create_store(arg,paramAlloca);//arg通过全局变量传递
    scope.push(node.id,paramAlloca);//将参数压入域中
}
//compound-stmt→{ local-declarations statement-list}
void CminusfBuilder::visit(ASTCompoundStmt &node) { 
    LOG(DEBUG)<<"CompoundStmt\n";
    bool need_exit_scope = !pre_enter_scope;
    if (pre_enter_scope) {
        pre_enter_scope = false;
    } else {
        scope.enter();
    }

    for (auto& decl: node.local_declarations) {
        decl->accept(*this);
    }

    for (auto& stmt: node.statement_list) {
        stmt->accept(*this);
        if (builder->get_insert_block()->get_terminator() != nullptr)
            break;
    }

    if (need_exit_scope) {
        scope.exit();
    }

}
//expression-stmt→expression ; | ;
void CminusfBuilder::visit(ASTExpressionStmt &node) { 
    LOG(DEBUG)<<"Expreesionstmt\n";
    if(node.expression!=nullptr)node.expression->accept(*this);//如果表达式不为空，说明其存在，处理
}
//selection-stmt→ if ( expression ) statement| if ( expression ) statement else statement
void CminusfBuilder::visit(ASTSelectionStmt &node) {
    LOG(DEBUG)<<"selection-stmt";
    node.expression->accept(*this);//solve statement
    auto resType=res->get_type();//selection-stmt type
    auto ret =res ;
    auto function=builder->get_insert_block()->get_parent();
    auto trueBB = BasicBlock::create(module.get(), "", function);
    auto falseBB = BasicBlock::create(module.get(), "", function);
    auto contBB = BasicBlock::create(module.get(), "", function);
    Value *cond_val;
    if (ret->get_type()->is_integer_type())
        cond_val = builder->create_icmp_ne(ret, CONST_INT(0));
    else
        cond_val = builder->create_fcmp_ne(ret, CONST_FP(0.));

    if (node.else_statement == nullptr) {
        builder->create_cond_br(cond_val, trueBB, contBB);
    } else {
        builder->create_cond_br(cond_val, trueBB, falseBB);
    }
    builder->set_insert_point(trueBB);
    node.if_statement->accept(*this);

    if (builder->get_insert_block()->get_terminator() == nullptr)
        builder->create_br(contBB);

    if (node.else_statement == nullptr) {
        falseBB->erase_from_parent();
    } else {
        builder->set_insert_point(falseBB);
        node.else_statement->accept(*this);
        if (builder->get_insert_block()->get_terminator() == nullptr)
            builder->create_br(contBB);
    }

    builder->set_insert_point(contBB);

 }
//iteration-stmt→while ( expression ) statement
void CminusfBuilder::visit(ASTIterationStmt &node) {
    LOG(DEBUG)<<"iteration-stmt";
    auto function=builder->get_insert_block()->get_parent();//获得当前块对应的函数
    auto exprBB = BasicBlock::create(module.get(), "", function);
    
    if (builder->get_insert_block()->get_terminator() == nullptr)
        builder->create_br(exprBB);
    builder->set_insert_point(exprBB);
    node.expression->accept(*this);
    auto ret_val = res;
    auto trueBB = BasicBlock::create(module.get(), "", function);
    auto contBB = BasicBlock::create(module.get(), "", function);
    Value *cond_val;
    if (ret_val->get_type()->is_integer_type())
        cond_val = builder->create_icmp_ne(ret_val, CONST_INT(0));
    else
        cond_val = builder->create_fcmp_ne(ret_val, CONST_FP(0.));

    builder->create_cond_br(cond_val, trueBB, contBB);
    builder->set_insert_point(trueBB);
    node.statement->accept(*this);
    if (builder->get_insert_block()->get_terminator() == nullptr)
        builder->create_br(exprBB);
    builder->set_insert_point(contBB);
 }
//return-stmt→return ; | return expression
void CminusfBuilder::visit(ASTReturnStmt &node) {
    LOG(DEBUG)<<"return-stmt";
    auto function = builder->get_insert_block()->get_parent(); //得到函数所在范围
    auto retType = function->get_return_type();//得到函数return的类型
    if(node.expression==nullptr){builder->create_void_ret();
    return;}
    }
    else{
    node.expression->accept(*this); //处理expression条件判断对应的表达式
    auto resType = res->get_type(); //获取表达式得到的结果类型
    // 处理expression返回的结果
    if (retType->is_integer_type() && resType->is_float_type()){res = builder->create_fptosi(res, TYINT32);LOG(WARNING) << "强制类型转换 float转int，返回值需要与定义一致";}    
    if (retType->is_float_type() && resType->is_integer_type()){res = builder->create_sitofp(res, TYFLOAT);LOG(WARNING) << "强制类型转换 int转float，返回值需要与定义一致";}    
    }
    builder->create_ret(res);
 }

//var→ID | ID [expression]
void CminusfBuilder::visit(ASTVar &node) {
    LOG(DEBUG)<<"var\n";
    auto var = scope.find(node.id);      //从域中取出对应变量 ID
    bool if_return_lvalue = if_return_notadd; // 判断是否需要返回地址
    if_return_notadd = false;                 // 重置

    Value *index = CONST_INT(0);         //init index for array
    if (node.expression != nullptr)//带有表达式,为数组
    {                                                     
        node.expression->accept(*this);                   // 处理expression，得到res 
        auto res1 = res;                                   // 存储结果 
        if (checkFloat(res1)){res1 = builder->create_fptosi(res1, TYINT32); LOG(WARNING)<<"下标应为整数，float改为int";}// 若是浮点数，则矫正为整数                               
            
        index = res1;                                      //表示数组下标
        // 判断下标是否为负数
        auto function = builder->get_insert_block()->get_parent();                    // 获取当前函数
        auto indexTest = builder->create_icmp_lt(index, CONST_ZERO(TYINT32));          // 测试index是否为负数 
        auto failBB = BasicBlock::create(module.get(), node.id + "_failTest", function); // 创建fail块 
        auto passBB = BasicBlock::create(module.get(), node.id + "_passTest", function); // 创建pass块 
        builder->create_cond_br(indexTest, failBB, passBB);                              // 如果为负，则跳转到fail块 

        // 下标为负数，调用neg_idx_except函数进行处理
        // 一个负的下标会导致程序终止，需要调用框架中的内置函数`neg_idx_except` （该内部函数会主动退出程序，只需要调用该函数即可），但是对于上界并不做检查。
        builder->set_insert_point(failBB);                       // fail块，即下标为负数 
        auto fail = scope.find("neg_idx_except");                // 取出neg_idx_except函数 
        builder->create_call(static_cast<Function *>(fail), {}); // 调用neg_idx_except函数进行处理 
        builder->create_br(passBB);                              // 跳转到pass块 

        // 下标合法
        builder->set_insert_point(passBB);                                // pass块 
        if (var->get_type()->get_pointer_element_type()->is_array_type()) // 若为指向数组的指针 
            var = builder->create_gep(var, {CONST_INT(0), index});        // 得到对应数组元素的指针，即指向元素 
        else
        {
            if (var->get_type()->get_pointer_element_type()->is_pointer_type()) // 若为指针 
                var = builder->create_load(var);                                // 则取出指针指向的元素 
            var = builder->create_gep(var, {index});                            // 得到对应元素的指针 
        }
        //expression中有内容
        if (if_return_lvalue) // 若要返回值 
        {                            
            res = var;               // 则返回var对应的地址 
            if_return_notadd = false; 
        }
        else res = builder->create_load(var); // 否则则进行load
        return;
    }
    else{ // 处理不是数组的情况 var->ID
        if (if_return_lvalue)
        {                             
            res = var;              // 若要返回值,则返回var对应的地址  
            if_return_notadd = false;     //重置
        }
        else
        {   // 数组的指针即a[]类型就返回数组的起始地址，否则load取值
            if (var->get_type()->get_pointer_element_type()->is_array_type()) // 若指向数组，需要两个偏移取地址 
                res = builder->create_gep(var, {CONST_INT(0), CONST_INT(0)}); //寻址，找到对应数组的首地址
            else
                res = builder->create_load(var); // 否则则是普通指针，直接进行load获得其指向元素 
        }
    }
}
 
//var = expression 赋值语句
void CminusfBuilder::visit(ASTAssignExpression &node) {
    LOG(DEBUG) << "AssignExpression\n";
    if_return_notadd = true; //表示需要返回值 
    
    // 获取左值，右值
    node.var->accept(*this);        // 处理var 
    auto left = res;                // left为左侧地址 
    node.expression->accept(*this); // 处理右侧expression 
    auto right = res;               // 获得右侧结果 

    // 处理左值，右值类型冲突问题
    auto leftType = left->get_type()->get_pointer_element_type(); // 获取var的类型
    /* 若赋值语句左右类型不匹配，则进行匹配 */
    if (leftType == TYFLOAT && checkInt(right)){right = builder->create_sitofp(right, TYFLOAT);LOG(WARNING)<<"赋值语句左右类型不匹配，右侧改为float";}
    if (leftType == TYINT32 && checkFloat(right)){right = builder->create_fptosi(right, TYINT32);LOG(WARNING)<<"赋值语句左右类型不匹配，右侧改为int";}
    // 赋值
    builder->create_store(right, left);
    res=right;
 }

//simple-expression→additive-expression relop additive-expression | additive-expression
void CminusfBuilder::visit(ASTSimpleExpression &node) {
    LOG(DEBUG) << "SimpleExpression\n";
    node.additive_expression_l->accept(*this); // 处理左边的expression 
    auto lres = res;                           // 获取结果存到lres中 
    if (node.additive_expression_r == nullptr)// 若不存在右expression，则直接返回
    {
        return;
    }                                           
    node.additive_expression_r->accept(*this); // 处理右边的expression 
    auto rres = res;                           // 结果存到rres中 

    // 需要确保两个表达式的类型相同，
    //若存在浮点和整型混存，全部转换为浮点型
    
    if (checkInt(lres) && checkInt(rres))//两边都是整数则进行int型操作 
    { 
        switch (node.op)//根据op构建代码
        {
        //调用icmp进行处理
        // 比较的返回结果
        // ast.hh中enum RelOp：
        // <= 对应 OP_LE,
        // < 对应 OP_LT,
        // > 对应 OP_GT,
        // >= 对应 OP_GE,
        // == 对应 OP_EQ,
        // != 对应 OP_NEQ
        case OP_LE:
            res = builder->create_icmp_le(lres, rres);
            break;
        case OP_LT:
            res = builder->create_icmp_lt(lres, rres);
            break;
        case OP_GT:
            res = builder->create_icmp_gt(lres, rres);
            break;
        case OP_GE:
            res = builder->create_icmp_ge(lres, rres);
            break;
        case OP_EQ:
            res = builder->create_icmp_eq(lres, rres);
            break;
        case OP_NEQ:
            res = builder->create_icmp_ne(lres, rres);
            break;
        }
    }
    else
    {   LOG(WARNING)<<"表达式类型不匹配，强制转换为float";                    
            // 存在浮点类型，若存在其中一个为int型，需要将其转为浮点数，再进行比较 
        if (checkInt(lres)) // 若左边是整数，则将其转为浮点数 
            lres = builder->create_sitofp(lres, TYFLOAT);
        if (checkInt(rres)) // 若右边是整数，则将其转为浮点数 
            rres = builder->create_sitofp(rres, TYFLOAT);
        switch (node.op)
        {
        /* 根据不同的比较操作，调用fcmp进行处理 */
        case OP_LE:
            res = builder->create_fcmp_le(lres, rres);
            break;
        case OP_LT:
            res = builder->create_fcmp_lt(lres, rres);
            break;
        case OP_GT:
            res = builder->create_fcmp_gt(lres, rres);
            break;
        case OP_GE:
            res = builder->create_fcmp_ge(lres, rres);
            break;
        case OP_EQ:
            res = builder->create_fcmp_eq(lres, rres);
            break;
        case OP_NEQ:
            res = builder->create_fcmp_ne(lres, rres);
            break;
        }
    }


    res = builder->create_zext(res, TYINT32); // 将得到的结果作为整数保存（可作为判断语句中的判断条件）
 }
//additive-expression→additive-expression addop term | term
void CminusfBuilder::visit(ASTAdditiveExpression &node) {    
    LOG(DEBUG) << "AdditiveExpression\n";
    if (node.additive_expression == nullptr)
    { // 若无加减法运算 
        node.term->accept(*this);
        return; // 则做乘除法 
    }

    node.additive_expression->accept(*this); // 处理左expression 
    auto lres = res;                         // 结果保存在lres中 
    node.term->accept(*this);                // 处理右term 
    auto rres = res;                         // 结果保存在rres中 

    // 分为整型-整型，和存在浮点数类型，这两种情况进行讨论
    // 若存在浮点数，则全部强制转换为浮点数实现
    if (checkInt(lres) && checkInt(rres))
    { //all int
        switch (node.op)
        { /* 根据对应加法或是减法，调用iadd或是isub进行处理 */
        case OP_PLUS:
            res = builder->create_iadd(lres, rres);
            break;
        case OP_MINUS:
            res = builder->create_isub(lres, rres);
            break;
        }
    }
    else
    {   LOG(WARNING)<<"需要强制类型转换为float";      
                      /* 若有一边是浮点类型，则需要先将另一边也转为浮点数，再进行处理 */
        if (checkInt(lres)) /* 若左边是整数，则将其转为浮点数 */
            lres = builder->create_sitofp(lres, TYFLOAT);
        if (checkInt(rres)) /* 若右边是整数，则将其转为浮点数 */
            rres = builder->create_sitofp(rres, TYFLOAT);
        switch (node.op)
        { /* 根据对应加法或是减法，调用fadd或是fsub进行处理 */
        case OP_PLUS:
            res = builder->create_fadd(lres, rres);
            break;
        case OP_MINUS:
            res = builder->create_fsub(lres, rres);
            break;
        }
    } }
//term→term mulop factor | factor
void CminusfBuilder::visit(ASTTerm &node) {     
    LOG(DEBUG) << "Term\n";
    if (node.term == nullptr)
    { //no term, only factor
        node.factor->accept(*this);
        return; //则处理元素 
    }

    node.term->accept(*this);   // 处理左term 
    auto lres = res;            // 结果保存在lres中 
    node.factor->accept(*this); // 处理右factor 
    auto rres = res;            // 结果保存在rres中
    if (checkInt(lres) && checkInt(rres))
    { //all int 
        switch (node.op)
        { /* 根据对应乘法或是除法，调用imul或是idiv进行处理 */
        case OP_MUL:
            res = builder->create_imul(lres, rres);
            break;
        case OP_DIV:
            res = builder->create_isdiv(lres, rres);
            break;
        }
    }
    else
    {   LOG(WARNING)<<"需要强制类型转换为float"; 
                            //将int转为float，再进行处理
        if (checkInt(lres)) 
            lres = builder->create_sitofp(lres, TYFLOAT);
        if (checkInt(rres)) 
            rres = builder->create_sitofp(rres, TYFLOAT);
        switch (node.op)
        { // 根据对应乘法或是除法，调用fmul或是fdiv进行处理
        case OP_MUL:
            res = builder->create_fmul(lres, rres);
            break;
        case OP_DIV:
            res = builder->create_fdiv(lres, rres);
            break;
        }
    }}

//call→ID ( args)
void CminusfBuilder::visit(ASTCall &node) {
    LOG(DEBUG) << "Call\n";
    auto function = static_cast<Function *>(scope.find(node.id));  // 获取需要调用的函数 
    auto paramType = function->get_function_type()->param_begin(); // 获取函数参数类型 

    if (function == nullptr)LOG(ERROR)<<"函数"<<node.id<<"未定义\n";

    // 处理参数列表
    std::vector<Value *> args; // 创建args用于存储函数参数的值，构建调用函数的参数列表
    for (auto &arg: node.args) {
        arg->accept(*this);
        if (!res->get_type()->is_pointer_type() &&
            *paramType != res->get_type()) {
            if (res->get_type()->is_integer_type())
                res = builder->create_sitofp(res, FLOAT_T);
            else
                res = builder->create_fptosi(res, INT32_T);
        }
        args.push_back(res);
        paramType++;
    }
    res = builder->create_call(static_cast<Function *>(function), args); /* 创建函数调用 */
 }
