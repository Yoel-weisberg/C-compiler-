#include "ExprAST.h"


Value* FloatNumberExprAST::codegen()
{
	return ConstantFP::get(Helper::getContext(), APFloat(_val));
}

Value* IntegerNumberExprAST::codegen()
{
    return ConstantInt::get(Helper::getContext(), APInt(_size, _val));
}

Value* CharExprAST::codegen()
{
    return ConstantInt::get(Helper::getContext(), APInt(_size, _val));
}


Value* ptrExprAST::codegen()
{
    auto it = Helper::SymbolTable.find(_valAsStr);
    llvm::AllocaInst* allocaInst = it->second;

    if (!allocaInst) {
        std::cerr << "Error: AllocaInst for variable '" << _valAsStr << "' is null.\n";
        return nullptr;
    }

    llvm::Type* pointerType = llvm::PointerType::getUnqual(allocaInst->getAllocatedType());

    // Generate a pointer value
    llvm::IRBuilder<>& Builder = Helper::getBuilder();
    return Builder.CreatePointerCast(allocaInst, pointerType, _valAsStr + "_ptr");
}




arrExprAST::arrExprAST(const std::string& type, std::string& size, const std::string& val, const std::string& name) :
    _val(val), _name(name)
{
    assignLLVMType(type);
    _size = Helper::hexToDec(size);

    // Parse 'init' into `_data`
    std::vector<uint64_t> parsedData;
    std::stringstream ss(val);
    std::string token;
    while (std::getline(ss, token, COMMA_LITERAL)) {
        parsedData.push_back(std::stoull(token));
    }
    _data = llvm::ArrayRef<uint64_t>(parsedData);
}

Value* arrExprAST::codegen() {
    // Retrieve the pointer to the allocated array
    AllocaInst* arrayPtr = Helper::SymbolTable[_name]; 
    if (!arrayPtr) {
        throw std::runtime_error("Array pointer not found in symbol table.");
    }

    llvm::IRBuilder<>& builder = Helper::getBuilder();
    
    // Initialize the array elements
    for (uint64_t i = 0; i < _size; i++) {
        Value* elementPtr = builder.CreateGEP(
            _type, arrayPtr,
            { builder.getInt32(0), builder.getInt32(i) }, _name + "_ptr");

        // Store value
        Value* elementValue = builder.getInt32(_data[i]); // Adjust for type
        // builder.CreateStore(elementValue, elementPtr);
    }

    return arrayPtr;
}





void arrExprAST::assignLLVMType(const std::string& type) {
    LLVMContext& context = Helper::getContext(); // Access the global LLVM context

    if (type == INT_TYPE_LIT ) {
        _type = llvm::Type::getInt32Ty(context); // 32-bit integer
    }
    else if (type == FLOAT_TYPE_LIT) {
        _type = llvm::Type::getFloatTy(context); // 32-bit float
    }
    else if (type == CHAR_TYPE_LIT) {
        _type = llvm::Type::getInt8Ty(context); // 8-bit integer
    }
    else {
        throw std::invalid_argument("Unknown type: " + type);
    }
}


void arrExprAST::initArrayRef(const std::string& val, const std::string& type)
{
    // create vector for array inner values by type
    std::vector<std::variant<int, float, char>> values;
    for (size_t i = 0; i < val.size(); i++) // No need to check types or validation,
    {                                       // So we can just add it to 'values'
        if (val[i] == COMMA_LITERAL)
        {
            continue;
        }
        else
        {
            values.push_back(val[i]);
        }
    }
    std::reverse(values.begin(), values.end());

    // Convert to uint64_t
    std::vector<uint64_t> transformedValues;
    for (const auto& v : values) {
        if (std::holds_alternative<int>(v)) {
            transformedValues.push_back(static_cast<uint64_t>(std::get<int>(v)));
        }
        else if (std::holds_alternative<float>(v)) {
            transformedValues.push_back(static_cast<uint64_t>(std::get<float>(v)));
        }
        else if (std::holds_alternative<char>(v)) {
            transformedValues.push_back(static_cast<uint64_t>(std::get<char>(v)));
        }
    }

    // Use arrayRef C'tor that uses the iteration through values
    _data = llvm::ArrayRef<uint64_t>(transformedValues);
}



Value* VariableExprAST::codegen()
{
    // Lookup the variable in the symbol table
    AllocaInst* variable = Helper::SymbolTable[_name];
    if (!variable)
    {
        throw std::runtime_error("Unknown variable name: " + _name);
    }

    // Generate a load instruction to load the value of the variable
    return Helper::getBuilder().CreateLoad(variable->getAllocatedType(), variable, _name);
}



// code like int a = 5;
Value* AssignExprAST::codegen()
{
    Helper::addSymbol(_varName, _varType, _varType);

    llvm::Value* varAlloca = Helper::getSymbolValue(_varName);
    if (!varAlloca)
    {
        std::cerr << "Error: Variable '" << _varName << "' not found in symbol table.\n";
        return nullptr;
    }

    llvm::Value* value = _value->codegen();
    if (!value)
    {
        std::cerr << "Error: Unable to generate code for assigned value.\n";
        return nullptr;
    }

    Helper::getBuilder().CreateStore(value, varAlloca);
    return value;
}

Value* IfExprAST::codegen() 
{
    Value* CondV = Cond->codegen();
    if (!CondV)
    {
        return nullptr;
    }

    // Convert condition to a bool by comparing non-equal to 0.0.
    CondV = Helper::Builder->CreateFCmpONE(
        CondV, ConstantFP::get(*Helper::TheContext, APFloat(0.0)), "ifcond");

    Function* TheFunction = Helper::Builder->GetInsertBlock()->getParent();

    // Create blocks for the then and merge cases. Insert the 'then' block at the end of the function.
    BasicBlock* ThenBB = BasicBlock::Create(*Helper::TheContext, "then", TheFunction);
    BasicBlock* MergeBB = BasicBlock::Create(*Helper::TheContext, "ifcont");

    BasicBlock* ElseBB = nullptr;
    if (Else) 
    {
        ElseBB = BasicBlock::Create(*Helper::TheContext, "else", TheFunction);
    }

    // Create conditional branch.
    if (Else) 
    {
        Helper::Builder->CreateCondBr(CondV, ThenBB, ElseBB);
    }
    else 
    {
        Helper::Builder->CreateCondBr(CondV, ThenBB, MergeBB);
    }

    // Emit then value.
    Helper::Builder->SetInsertPoint(ThenBB);
    Value* ThenV = Then->codegen();
    if (!ThenV)
    {
        return nullptr;
    }

    Helper::Builder->CreateBr(MergeBB);
    ThenBB = Helper::Builder->GetInsertBlock();

    // Emit else block if it exists.
    Value* ElseV = nullptr;
    if (Else) 
    {
        TheFunction->insert(TheFunction->end(), ElseBB);
        Helper::Builder->SetInsertPoint(ElseBB);
        ElseV = Else->codegen();
        if (!ElseV)
            return nullptr;

        Helper::Builder->CreateBr(MergeBB);
        ElseBB = Helper::Builder->GetInsertBlock();
    }

    // Emit merge block
    TheFunction->insert(TheFunction->end(), MergeBB);
    Helper::Builder->SetInsertPoint(MergeBB);

    // Get the type from the Then value
    Type* thenType = ThenV->getType();
    PHINode* PN = Helper::Builder->CreatePHI(thenType, 2, "iftmp");
    PN->addIncoming(ThenV, ThenBB);
    if (Else)
        PN->addIncoming(ElseV, ElseBB);

    return PN;
}

/*
* Generates LLVM IR code for a while loop:
* Starting with the condition and moving on to the inner part,
* which is processed similarly to a function.
*/
llvm::Value* WhileLoopAST::codegen() {
    llvm::Function* currentFunction = Helper::Builder->GetInsertBlock()->getParent();
    // Create the basic blocks for the loop 
    llvm::BasicBlock* conditionB = llvm::BasicBlock::Create(*Helper::TheContext, "while.cond", currentFunction);
    llvm::BasicBlock* bodyB = llvm::BasicBlock::Create(*Helper::TheContext, "while.body", currentFunction);
    llvm::BasicBlock* exitB = llvm::BasicBlock::Create(*Helper::TheContext, "while.exit", currentFunction);

    // Branch to the condition block to start the loop
    Helper::Builder->CreateBr(conditionB);

    // Insert code into the condition block 
    Helper::Builder->SetInsertPoint(conditionB);

    // Generate condition code
    llvm::Value* condition = _condition->codegen();
    if (!condition) {
        return nullptr;
    }

    // Convert condition to a bool expression based on its type
    if (condition->getType()->isFloatingPointTy()) {
        // For floating point, compare with 0.0
        condition = Helper::Builder->CreateFCmpONE(
            condition,
            ConstantFP::get(*Helper::TheContext, APFloat(0.0)),
            "whilecond"
        );
    }
    else if (condition->getType()->isIntegerTy()) {
        // For integer, compare with 0
        condition = Helper::Builder->CreateICmpNE(
            condition,
            ConstantInt::get(condition->getType(), 0),
            "whilecond"
        );
    }
    else {
        // Handle other types or report an error
        std::cerr << "Error: Condition must be a numeric type\n";
        return nullptr;
    }

    // Branch based on the condition: true -> body, false -> exit 
    Helper::Builder->CreateCondBr(condition, bodyB, exitB);

    // Generate code for loop body
    Helper::Builder->SetInsertPoint(bodyB);

    llvm::Value* lastBodyValue = nullptr;
    // Process all expressions in the body
    for (auto& expr : _body) {
        lastBodyValue = expr->codegen();
        if (!lastBodyValue) {
            return nullptr;
        }
    }

    // Jump back to condition block 
    Helper::Builder->CreateBr(conditionB);

    // Insert code into exit block 
    Helper::Builder->SetInsertPoint(exitB);

    // Return a placeholder value if needed (depends on your language semantics)
    return llvm::ConstantInt::get(*Helper::TheContext, llvm::APInt(32, 0));
}

llvm::Value* DoWhileLoopAST::codegen()
{
    llvm::Function* currentFunction = Helper::Builder->GetInsertBlock()->getParent();

    // Create basic blocks for loops' body, condition and exit
    llvm::BasicBlock* bodyB = llvm::BasicBlock::Create(*Helper::TheContext, "dowhile.body", currentFunction);
    llvm::BasicBlock* conditionB = llvm::BasicBlock::Create(*Helper::TheContext, "dowhile.cond", currentFunction);
    llvm::BasicBlock* exitB = llvm::BasicBlock::Create(*Helper::TheContext, "dowhile.exit", currentFunction);

    // Create banch for loops' body
    Helper::Builder->CreateBr(bodyB);

    // Insert body code
    Helper::Builder->SetInsertPoint(bodyB);

    // Generate body code
    llvm::Value* bodyV = _body[0]->codegen(); // TEMPORARY!!!!!!!!!!!!!!!!
    if (!bodyB)
    {
        return nullptr;
    }
    llvm::Type* bodyT = bodyV->getType();

    llvm::PHINode* result = Helper::Builder->CreatePHI(bodyT, 2, "dolooptmp");
    result->addIncoming(bodyV, Helper::Builder->GetInsertBlock());

    // Handle condition 
    // Create branch for loops' condition
    Helper::Builder->CreateBr(conditionB);

    // Insert code
    Helper::Builder->SetInsertPoint(conditionB);

    // Generate code
    llvm::Value* conditionV = _condition->codegen();
    if (!conditionB)
    {
        return nullptr;
    }

    conditionV = Helper::Builder->CreateFCmpONE(conditionV, ConstantFP::get(*Helper::TheContext, APFloat(0.0)), "dowhilecond");

    // Branch based on the condition: true -> body, false -> exit 
    Helper::Builder->CreateCondBr(conditionV, bodyB, exitB);

    // Insert code into exit code 
    Helper::Builder->SetInsertPoint(exitB);

    return result;
}

llvm::Value* ForLoopAST::codegen()
{
    llvm::Function* currentFunction = Helper::Builder->GetInsertBlock()->getParent();

    // Create basic blocks for loops' body, condition and exit
    llvm::BasicBlock* headerB = llvm::BasicBlock::Create(*Helper::TheContext, "for.header", currentFunction);
    llvm::BasicBlock* exitB = llvm::BasicBlock::Create(*Helper::TheContext, "for.exit", currentFunction);
    llvm::BasicBlock* bodyB = llvm::BasicBlock::Create(*Helper::TheContext, "for.body", currentFunction);
    llvm::BasicBlock* latchB = llvm::BasicBlock::Create(*Helper::TheContext, "for.latch", currentFunction);

    // Create branch for loop
    Helper::Builder->CreateBr(headerB);
    // Insert entry code
    Helper::Builder->SetInsertPoint(headerB);

    // Generate code for loops' header: initilization and condition
    llvm::Value* initV = _condInit->codegen();
    if (!initV)
    {
        return nullptr;
    }

    // Create PHI node for loop iterator
    llvm::Type* initT = initV->getType();
    llvm::PHINode* iterator = Helper::Builder->CreatePHI(initT, 2, "fortmpit");
    iterator->addIncoming(initV, headerB); // Initial value comes from entry block 


    llvm::Value* condV = _condC->codegen();
    if (!condV)
    {
        return nullptr;
    }

    // Convert condition to a boolean value
    condV = Helper::Builder->CreateFCmpONE(condV, ConstantFP::get(*Helper::TheContext, APFloat(0.0)), "forcond");


    // Create conditional branch: true -> body, false -> exit
    Helper::Builder->CreateCondBr(condV, bodyB, exitB);

    // Insert code into the body block
    Helper::Builder->SetInsertPoint(bodyB);

    llvm::Value* bodyV = _body[0]->codegen(); // TEMPORARY!!!!
    if (!bodyV)
    {
        return nullptr;
    }

    // After executing body, go to latch block
    Helper::Builder->CreateBr(latchB);

    // Insert code into latch block 
    Helper::Builder->SetInsertPoint(latchB);

    llvm::Value* condStepV = _condStep->codegen();
    if (!condStepV)
    {
        return nullptr;
    }

    // Compute the new iterator value
    llvm::Value* nextVal = Helper::Builder->CreateAdd(iterator, condStepV, "nextVal"); // !NOTE! Only supports increment for now
        
    // Add new value to PHI node
    iterator->addIncoming(nextVal, latchB);
    
    // Jump back to header to re-evaluate the condition
    Helper::Builder->CreateBr(headerB);

    // Insert code into exit block 
    Helper::Builder->SetInsertPoint(exitB);
    // Handle return value!!! need to return something in order to print anything!
    return bodyV;
}


llvm::Function* PrototypeAST::codegen()
{
    // TO-DO make that it would not only work eith double
    // Make the function type:  double(double,double) etc.
    std::vector<Type*> ArgsList;

    // handle args list:
    for (auto arg : Args)
    {
        ArgsList.push_back(Helper::getTypeFromString(arg.Type));
    }
    Type* ReturnType = Helper::getTypeFromString(returnType);
    FunctionType* FT =
        FunctionType::get(ReturnType, ArgsList, false);

    Function* F =
        Function::Create(FT, Function::ExternalLinkage, Name, Helper::getModule());

    // Set names for all arguments.
    unsigned Idx = 0;
    for (auto& Arg : F->args())
        Arg.setName(Args[Idx].Name);

    return F;
}

const std::string PrototypeAST::getParamName(unsigned idx) const
{
    if (idx < Args.size()) {
        return Args[idx].Name;
    }
    return "";
}

Function* FunctionAST::codegen()
{
    // Transfer ownership of the prototype to the FunctionProtos map, but keep a
    // reference to it for use below.
    auto& P = *Proto;
    Helper::FunctionProtos[Proto->getName()] = std::move(Proto);
 
    Function* TheFunction = Helper::getFunction(P.getName());
    if (!TheFunction)
    {
        return nullptr;
    }

    // Create a new basic block to start insertion into.
    BasicBlock* BB = BasicBlock::Create(Helper::getContext(), "entry", TheFunction);
    Helper::getBuilder().SetInsertPoint(BB);

    // Create allocas for function parameters
    // This ensures we properly handle parameter variables
    unsigned idx = 0;
    for (auto& Arg : TheFunction->args()) {
        // Create an alloca for this argument
        AllocaInst* Alloca = Helper::getBuilder().CreateAlloca(
            Arg.getType(), nullptr, P.getParamName(idx));

        // Store the argument value into the alloca
        Helper::getBuilder().CreateStore(&Arg, Alloca);

        // Add to symbol table
        Helper::SymbolTable[P.getParamName(idx)] = Alloca;
        idx++;
    }

    // Generate code for all expressions in the function body
    for (const auto& expr : Body)
    {
        if (!expr->codegen())
        {
            TheFunction->eraseFromParent();
            return nullptr;
        }
    }

    // Handle return value
    llvm::Value* retVal = nullptr;
    if (ReturnValue)
    {
        retVal = ReturnValue->codegen();
        if (!retVal)
        {
            TheFunction->eraseFromParent();
            return nullptr;
        }
    }

    // Add appropriate return instruction based on function type
    if (returnType == "void")
    {
        Helper::getBuilder().CreateRetVoid();
    }
    else if (retVal)
    {
        // Ensure the return value matches the function's return type
        Type* funcRetType = TheFunction->getReturnType();
        Type* retValType = retVal->getType();

        if (funcRetType != retValType) {
            // Convert the return value if types don't match
            if (funcRetType->isIntegerTy() && retValType->isFloatingPointTy()) {
                retVal = Helper::getBuilder().CreateFPToSI(retVal, funcRetType, "conv");
            }
            else if (funcRetType->isFloatingPointTy() && retValType->isIntegerTy()) {
                retVal = Helper::getBuilder().CreateSIToFP(retVal, funcRetType, "conv");
            }
            // Add more conversions if needed
        }

        Helper::getBuilder().CreateRet(retVal);
    }
    else
    {
        // Ensure any basic block has a terminator
        if (!Helper::getBuilder().GetInsertBlock()->getTerminator()) {
            if (returnType == "void") {
                Helper::getBuilder().CreateRetVoid();
            }
            else {
                // Create a default return value of the appropriate type
                Type* returnTy = TheFunction->getReturnType();
                if (returnTy->isIntegerTy()) {
                    Helper::getBuilder().CreateRet(
                        ConstantInt::get(returnTy, 0));
                }
                else if (returnTy->isFloatingPointTy()) {
                    Helper::getBuilder().CreateRet(
                        ConstantFP::get(returnTy, 0.0));
                }
                else {
                    TheFunction->eraseFromParent();
                    throw std::runtime_error("Unsupported return type");
                }
            }
        }
    }

    // Verify the function
    try {
        llvm::verifyFunction(*TheFunction);
    }
    catch (const std::exception& e) {
        std::cerr << "Function verification failed: " << e.what() << std::endl;
        std::cerr << "Function dump: " << std::endl;
        TheFunction->print(llvm::errs());
        TheFunction->eraseFromParent();
        return nullptr;
    }

    return TheFunction;
}

Value* BinaryExprAST::codegen() {
    Value* L = LHS->codegen();
    Value* R = RHS->codegen();
    if (!L || !R)
        return nullptr;

    // Ensure the operands are of the same type
    Type* LType = L->getType();
    Type* RType = R->getType();

    // If types don't match, perform necessary conversions
    if (LType != RType) {
        // Choose the "higher" type (e.g., float is higher than int)
        if (LType->isFloatingPointTy() && RType->isIntegerTy()) {
            // Convert R to float
            R = Helper::Builder->CreateSIToFP(R, LType, "convtmp");
        }
        else if (LType->isIntegerTy() && RType->isFloatingPointTy()) {
            // Convert L to float
            L = Helper::Builder->CreateSIToFP(L, RType, "convtmp");
        }
        else if (LType->isIntegerTy() && RType->isIntegerTy()) {
            // If both are integers but of different bit widths, convert to the wider one
            unsigned LWidth = LType->getIntegerBitWidth();
            unsigned RWidth = RType->getIntegerBitWidth();

            if (LWidth < RWidth) {
                L = Helper::Builder->CreateIntCast(L, RType, true, "convtmp");
            }
            else {
                R = Helper::Builder->CreateIntCast(R, LType, true, "convtmp");
            }
        }
        // Add more type conversion cases as needed
    }

    // Now perform the operation with operands of the same type
    switch (Op) {
    case ADDITION:
        if (L->getType()->isFloatingPointTy())
            return Helper::Builder->CreateFAdd(L, R, "addtmp");
        else
            return Helper::Builder->CreateAdd(L, R, "addtmp");
    case SUBTRACTION:
        if (L->getType()->isFloatingPointTy())
            return Helper::Builder->CreateFSub(L, R, "subtmp");
        else
            return Helper::Builder->CreateSub(L, R, "subtmp");
    case MULTIPLICATION:
        if (L->getType()->isFloatingPointTy())
            return Helper::Builder->CreateFMul(L, R, "multmp");
        else
            return Helper::Builder->CreateMul(L, R, "multmp");
    case DIVISION:
        if (L->getType()->isFloatingPointTy())
            return Helper::Builder->CreateFDiv(L, R, "divtmp");
        else
            return Helper::Builder->CreateSDiv(L, R, "divtmp");
    case LOWER_THEN:
        if (L->getType()->isFloatingPointTy())
            L = Helper::Builder->CreateFCmpOLT(L, R, "cmptmp");
        else
            L = Helper::Builder->CreateICmpSLT(L, R, "cmptmp");
        break;
    case HIGHER_THEN:
        if (L->getType()->isFloatingPointTy())
            L = Helper::Builder->CreateFCmpOGT(L, R, "cmptmp");
        else
            L = Helper::Builder->CreateICmpSGT(L, R, "cmptmp");
        break;
    case EQUELS_CMP:
        if (L->getType()->isFloatingPointTy())
            L = Helper::Builder->CreateFCmpOEQ(L, R, "cmptmp");
        else
            L = Helper::Builder->CreateICmpEQ(L, R, "cmptmp");
        break;
    default:
        throw SyntaxError("invalid binary operator");
    }

    // For comparison operators, convert the boolean result to the appropriate type
    if (Op >= LOWER_THEN && Op <= EQUELS_CMP) {
        // Check if we need to convert to float or int32
        if (L->getType()->isIntegerTy(1)) { // Boolean type in LLVM is i1
            if (LType->isFloatingPointTy() || RType->isFloatingPointTy()) {
                return Helper::Builder->CreateUIToFP(L, Type::getDoubleTy(Helper::getContext()), "booltmp");
            }
            else {
                return Helper::Builder->CreateZExt(L, Type::getInt32Ty(Helper::getContext()), "booltmp");
            }
        }
    }

    return L;
}

llvm::Value* UnaryOpExprAST::codegen() {
    llvm::Value* L = _LHS->codegen();
    if (!L) {
        return nullptr;
    }

    switch (_op) {
    case ADDITION:
        return Helper::Builder->CreateAdd(L, llvm::ConstantInt::get(L->getType(), 1), "increment");
    case SUBTRACTION:
        return Helper::Builder->CreateSub(L, llvm::ConstantInt::get(L->getType(), 1), "decrement");
    default:
        throw SyntaxError("Invalid unary operator");
    }
    return nullptr;
}

Value* CallExprAST::codegen() {
    // Look up the name in the global module table.
    Function* CalleeF = Helper::getFunction(Callee);
    if (!CalleeF)
        throw SyntaxError("Unknown function name");

    // Check if function has varargs
    bool isVarArg = CalleeF->getFunctionType()->isVarArg();

    // If argument mismatch error (only if not varargs)
    if (!isVarArg && CalleeF->arg_size() != Args.size())
        throw SyntaxError("Incorrect # arguments passed");

    std::vector<Value*> ArgsV;
    for (unsigned i = 0, e = Args.size(); i != e; ++i) {
        Value* argVal = Args[i]->codegen();
        if (!argVal)
            return nullptr;

        // For printf/scanf, convert integers to the expected type if needed
        if ((Callee == "printf" || Callee == "scanf") && i > 0) {
            if (argVal->getType()->isIntegerTy() &&
                argVal->getType()->getIntegerBitWidth() < 32) {
                argVal = Helper::getBuilder().CreateZExt(
                    argVal,
                    Type::getInt32Ty(Helper::getContext()),
                    "zext");
            }
        }

        ArgsV.push_back(argVal);
    }

    if (CalleeF->getReturnType()->isVoidTy()) {
        return Helper::getBuilder().CreateCall(CalleeF, ArgsV);
    }

    return Helper::getBuilder().CreateCall(CalleeF, ArgsV, CALL_TMP);
}

Value* VoidAst::codegen()
{
    return nullptr;
}

llvm::Value* StructDefinitionExprAST::codegen()
{
    llvm::LLVMContext& Context = Helper::getContext();

    // Collect field types
    std::vector<llvm::Type*> memberTypes;
    for (const auto& field : _members) {
        llvm::Type* fieldType = Helper::getLLVMType(field.Type, Context);
        memberTypes.push_back(fieldType);
    }

    // Create the struct type
    llvm::StructType* structType = llvm::StructType::create(Context, memberTypes, _name);

    // Store in symbol table (for future reference)
    Helper::StructTable[_name] = structType;
    structType->print(errs());
    return nullptr;
}

llvm::Value* StructDeclerationExprAST::codegen()
{
    llvm::LLVMContext& Context = Helper::getContext();
    llvm::IRBuilder<>& builder = Helper::getBuilder();

    auto it = Helper::StructTable.find(_type);
    llvm::StructType* type = it->second;

    llvm::AllocaInst* alloca = builder.CreateAlloca(type, nullptr, _name);

    Helper::SymbolTable[_name] = alloca;
    return alloca;
}

Value* StringExprAST::codegen()
{
    // Create a global string constant
    llvm::Constant* strConstant = llvm::ConstantDataArray::getString(
        Helper::getContext(),
        _str,
        true);  // Add null terminator

    // Create a global variable to hold the string
    llvm::GlobalVariable* gVar = new llvm::GlobalVariable(
        Helper::getModule(),
        strConstant->getType(),
        true,  // isConstant
        llvm::GlobalValue::PrivateLinkage,
        strConstant,
        ".str");

    // Get a pointer to the beginning of the string
    llvm::Value* zero = llvm::ConstantInt::get(
        llvm::Type::getInt32Ty(Helper::getContext()), 0);

    std::vector<llvm::Value*> indices;
    indices.push_back(zero);
    indices.push_back(zero);

    return llvm::GetElementPtrInst::Create(
        strConstant->getType(),
        gVar,
        indices,
        "str",
        Helper::getBuilder().GetInsertBlock());
}
