#include "ExprAST.h"


Value* FloatNumberExprAST::codegen()
{
    //Allocate memory + add to symbol table
    Helper::addSymbol(_name, "float", std::to_string(_val));
	return ConstantFP::get(Helper::getContext(), APFloat(_val));
}

Value* IntegerNumberExprAST::codegen()
{
    //Allocate memory + add to symbol table
    Helper::addSymbol(_name, "int", std::to_string(_val));
    return ConstantInt::get(Helper::getContext(), APInt(_size, _val));
}

Value* CharExprAST::codegen()
{    
    //Allocate memory + add to symbol table
    Helper::addSymbol(_name, "char", std::to_string(_val));
    return ConstantInt::get(Helper::getContext(), APInt(_size, _val));
}


Value* ptrExprAST::codegen()
{
    //Allocate memory + add to symbol table

    auto it = Helper::NamedValues.find(_valAsStr);
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

Value* arrExprAST::codegen() 
{
    //Allocate memory + add to symbol table
    // !NOTE!
    // Memeory allocation for arrays is done here, 
    // and not in the Helper method like the rest of the types
    Helper::addSymbol(_name, "arr", _val, "int", _size); // Check if implementation is nessecary and adjust!
    // Retrieve the pointer to the allocated array
    AllocaInst* arrayPtr = Helper::NamedValues[_name]; 
    if (!arrayPtr) {
        throw std::runtime_error("Array pointer not found in symbol table.");
    }

    llvm::IRBuilder<>& builder = Helper::getBuilder();
    llvm::LLVMContext& context = Helper::getContext();
    Value* elementPtr = nullptr;
    Value* elementValue = nullptr;
    
    // Initialize the array elements
    for (uint64_t i = 0; i < _size; i++)
    {
        if (_typeStr == "int")
        {
            Value* elementPtr = builder.CreateGEP(_type, arrayPtr, { builder.getInt32(0), builder.getInt32(i) }, _name + "_ptr");
            // Store value
            Value* elementValue = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), _data[i]);
        }
        else if (_typeStr == "float")
        {
            Value* elementPtr = builder.CreateGEP(_type, arrayPtr, { builder.getInt32(0), builder.getInt32(i) }, _name + "_ptr");
            // Store value
            Value* elementValue = llvm::ConstantFP::get(llvm::Type::getFloatTy(context), _data[i]);
        }
        else if (_typeStr == "char")
        {
            Value* elementPtr = builder.CreateGEP(_type, arrayPtr, { builder.getInt8(0), builder.getInt8(i) }, _name + "_ptr");
            // Store value
            Value* elementValue = llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), _data[i]);
        }
        else 
        {
            throw std::invalid_argument("Unknown type: " + _typeStr);
        }
        builder.CreateStore(elementValue, elementPtr);
    }

    return arrayPtr;
}




/*
* Helper method for array type variables codegen. 
*/
void arrExprAST::assignLLVMType(const std::string& type) {
    LLVMContext& context = Helper::getContext(); 

    if (type == "int") {
        _type = llvm::Type::getInt32Ty(context); // 32-bit integer
    }
    else if (type == "float") {
        _type = llvm::Type::getFloatTy(context); // 32-bit float
    }
    else if (type == "char") {
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
        if (val[i] == ',')
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
    AllocaInst* variable = Helper::NamedValues[_name];
    if (!variable) {
        throw std::runtime_error("Unknown variable name: " + _name);
    }

    // Generate a load instruction to load the value of the variable
    return Helper::getBuilder().CreateLoad(variable->getAllocatedType(), variable, _name);
}



// code like int a = 5;
Value* AssignExprAST::codegen() {
    Helper::addSymbol(_varName, _varType, _varType);

    llvm::Value* varAlloca = Helper::getSymbolValue(_varName);
    if (!varAlloca) {
        std::cerr << "Error: Variable '" << _varName << "' not found in symbol table.\n";
        return nullptr;
    }

    llvm::Value* value = _value->codegen();
    if (!value) {
        std::cerr << "Error: Unable to generate code for assigned value.\n";
        return nullptr;
    }

    Helper::getBuilder().CreateStore(value, varAlloca);
    return value;
}

Value* IfExprAST::codegen() {
    Value* CondV = _cond->codegen();
    if (!CondV)
        return nullptr;

    // Convert condition to a bool by comparing non-equal to 0.0.
    CondV = Helper::Builder->CreateFCmpONE(
        CondV, ConstantFP::get(*Helper::TheContext, APFloat(0.0)), "ifcond");

    Function* TheFunction = Helper::Builder->GetInsertBlock()->getParent();

    // Create blocks for the then and merge cases. Insert the 'then' block at the end of the function.
    BasicBlock* ThenBB = BasicBlock::Create(*Helper::TheContext, "then", TheFunction);
    BasicBlock* MergeBB = BasicBlock::Create(*Helper::TheContext, "ifcont");

    BasicBlock* ElseBB = nullptr;
    if (_else) {
        ElseBB = BasicBlock::Create(*Helper::TheContext, "else", TheFunction);
    }

    // Create conditional branch.
    if (_else) {
        Helper::Builder->CreateCondBr(CondV, ThenBB, ElseBB);
    }
    else {
        Helper::Builder->CreateCondBr(CondV, ThenBB, MergeBB);
    }

    // Emit then value.
    Helper::Builder->SetInsertPoint(ThenBB);
    Value* ThenV = _then->codegen();
    if (!ThenV)
        return nullptr;

    Helper::Builder->CreateBr(MergeBB);
    ThenBB = Helper::Builder->GetInsertBlock();

    // Emit else block if it exists.
    Value* ElseV = nullptr;
    if (_else) {
        TheFunction->insert(TheFunction->end(), ElseBB);
        Helper::Builder->SetInsertPoint(ElseBB);
        ElseV = _else->codegen();
        if (!ElseV)
            return nullptr;

        Helper::Builder->CreateBr(MergeBB);
        ElseBB = Helper::Builder->GetInsertBlock();
    }

    // Emit merge block.
    TheFunction->insert(TheFunction->end(), MergeBB);
    Helper::Builder->SetInsertPoint(MergeBB);

    PHINode* PN = Helper::Builder->CreatePHI(Type::getDoubleTy(*Helper::TheContext), 2, "iftmp");
    PN->addIncoming(ThenV, ThenBB);
    if (_else)
        PN->addIncoming(ElseV, ElseBB);

    return PN;
}


Function* PrototypeAST::codegen()
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

Function* FunctionAST::codegen()
{
    // Transfer ownership of the prototype to the FunctionProtos map, but keep a
   // reference to it for use below.
    auto& P = *Proto;
    Helper::FunctionProtos[Proto->getName()] = std::move(Proto);
    Function* TheFunction = Helper::getFunction(P.getName());
    if (!TheFunction)
        return nullptr;

    // Create a new basic block to start insertion into.
    BasicBlock* BB = BasicBlock::Create(Helper::getContext(), "entry", TheFunction);
    Helper::getBuilder().SetInsertPoint(BB);


    // Record the function arguments in the NamedValues map.
    // TODO - add scope to variebles
    //for (auto& Arg : TheFunction->args())
    //    Symbol newSymbol = Symbol(std::string(Arg.getName()), &Arg)
    //    SymbolTable   = &Arg;

    if (Value* functionBodeyIR = Body->codegen()) {

        Value* RetVal = ReturnValue->codegen();

        // Finish off the function.
        if (returnType == "void")
        {
            Helper::Builder->CreateRetVoid();
        }
        else
        {
            Helper::Builder->CreateRet(RetVal);
        }

        // Validate the generated code, checking for consistency.
        verifyFunction(*TheFunction);

        // Run the optimizer on the function.
        // not now for debugging porpuses
        // Helper::TheFPM->run(*TheFunction, *Helper::TheFAM);

        return TheFunction;
    }

    // Error reading body, remove function.
    TheFunction->eraseFromParent();
    return nullptr;
}

Value* BinaryExprAST::codegen()
{
    Value* L = LHS->codegen();
    Value* R = RHS->codegen();
    if (!L || !R)
        return nullptr;

    switch (Op) {
    case ADDITION:
        return Helper::Builder->CreateFAdd(L, R, "addtmp");
    case SUBTRACTION:
        return Helper::Builder->CreateFSub(L, R, "subtmp");
    case MULTIPLICATION:
        return Helper::Builder->CreateFMul(L, R, "multmp");
    case LOWER_THEN:
        L = Helper::Builder->CreateFCmpULT(L, R, "cmptmp");
        // Convert bool 0/1 to double 0.0 or 1.0
        return Helper::Builder->CreateUIToFP(L, Type::getDoubleTy(Helper::getContext()), "booltmp");
    case HIGHER_THEN:
        L = Helper::Builder->CreateFCmpUGT(L, R, "cmptmp");
        return Helper::Builder->CreateUIToFP(L, Type::getDoubleTy(Helper::getContext()), "booltmp");
    case EQUELS_CMP:
        L = Helper::Builder->CreateFCmpUEQ(L, R, "cmptmp");
        return Helper::Builder->CreateUIToFP(L, Type::getDoubleTy(Helper::getContext()), "booltmp");
    default:
        throw SyntaxError("invalid binary operator");
    }
}

Value* CallExprAST::codegen()
{
    // Look up the name in the global module table.
    Function* CalleeF = Helper::getFunction(Callee);
    if (!CalleeF)
        throw SyntaxError("Unknown function name");

    // If argument mismatch error.
    if (CalleeF->arg_size() != Args.size())
        throw SyntaxError("Incorrect # arguments passed");

    std::vector<Value*> ArgsV;
    for (unsigned i = 0, e = Args.size(); i != e; ++i) {
        ArgsV.push_back(Args[i]->codegen());
        if (!ArgsV.back())
            return nullptr;
    }
    if (CalleeF->getReturnType()->isVoidTy())
    {
        return Helper::Builder->CreateCall(CalleeF, ArgsV);

    }
    return Helper::Builder->CreateCall(CalleeF, ArgsV, "calltmp");
}

Value* VoidAst::codegen()
{
    return nullptr;
}
