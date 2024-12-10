#include "ExprAST.h"
#include "SyntexError.h"
Value* FloatNumberExprAST::codegen()
{
	return ConstantFP::get(Helper::getContext(), APFloat(Val));
}

Value* VariableExprAST::codegen()
{
	// Look this variable up in the function.
    Value* V = Helper::symbolTable.findSymbol(Name)->get().getLLVMValue();
	if (!V)
		throw SyntaxError("Not defined varieble");
	return V;
}

// code like int a = 5;
llvm::Value* AssignExprAST::codegen() {
    llvm::IRBuilder<>& Builder = Helper::getBuilder(); // Using the active Builder
    llvm::LLVMContext& Context = Helper::getContext(); // Using the Context from Helper
    llvm::Type* llvmType = nullptr;

    // Map the variable type to LLVM types
    if (_varType == "int") {
        llvmType = llvm::Type::getInt32Ty(Context);
    }
    else if (_varType == "float") {
        llvmType = llvm::Type::getFloatTy(Context);
    }
    else {
        std::cerr << "Error: Unsupported variable type '" << _varType << "'.\n";
        return nullptr;
    }

    // Check if the variable exists in the symbol table
    auto symbolOpt = Helper::symbolTable.findSymbol(_VarName);
    llvm::Value* varAddress = nullptr;

    if (!symbolOpt) {
        // Ensure we are in a function context
        llvm::Function* currentFunction = Builder.GetInsertBlock()->getParent();
        if (!currentFunction) {
            std::cerr << "Error: Attempting to allocate variable outside of a function.\n";
            return nullptr;
        }

        // Allocate memory for the variable in the entry block
        llvm::IRBuilder<> tempBuilder(&currentFunction->getEntryBlock(),
            currentFunction->getEntryBlock().begin());
        varAddress = tempBuilder.CreateAlloca(llvmType, nullptr, _VarName.c_str());

        // Add the variable to the symbol table
        Helper::symbolTable.addSymbol(_VarName, _varType, varAddress);
    }
    else {
        // Retrieve the variable address from the symbol table
        varAddress = symbolOpt->get().getLLVMValue();
        if (!varAddress) {
            std::cerr << "Error: Variable '" << _VarName << "' has no allocated memory.\n";
            return nullptr;
        }
    }

    // Generate code for the RHS expression
    llvm::Value* rhsValue = _Value->codegen();
    if (!rhsValue) {
        std::cerr << "Error: Failed to generate code for the assigned value.\n";
        return nullptr;
    }

    // Store the RHS value in the allocated variable
    Builder.CreateStore(rhsValue, varAddress);

    // Return the address of the variable or the RHS value as needed
    return varAddress;
}

Value* IfExprAST::codegen() {
    Value* CondV = Cond->codegen();
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
    if (Else) {
        ElseBB = BasicBlock::Create(*Helper::TheContext, "else", TheFunction);
    }

    // Create conditional branch.
    if (Else) {
        Helper::Builder->CreateCondBr(CondV, ThenBB, ElseBB);
    }
    else {
        Helper::Builder->CreateCondBr(CondV, ThenBB, MergeBB);
    }

    // Emit then value.
    Helper::Builder->SetInsertPoint(ThenBB);
    Value* ThenV = Then->codegen();
    if (!ThenV)
        return nullptr;

    Helper::Builder->CreateBr(MergeBB);
    ThenBB = Helper::Builder->GetInsertBlock();

    // Emit else block if it exists.
    Value* ElseV = nullptr;
    if (Else) {
        TheFunction->insert(TheFunction->end(), ElseBB);
        Helper::Builder->SetInsertPoint(ElseBB);
        ElseV = Else->codegen();
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
    if (Else)
        PN->addIncoming(ElseV, ElseBB);

    return PN;
}


Function* PrototypeAST::codegen()
{
    // TO-DO make that it would not only work eith double
    // Make the function type:  double(double,double) etc.
    std::vector<Type*> Doubles(Args.size(), Type::getDoubleTy(Helper::getContext()));
    FunctionType* FT =
        FunctionType::get(Type::getDoubleTy(Helper::getContext()), Doubles, false);

    Function* F =
        Function::Create(FT, Function::ExternalLinkage, Name, Helper::getModule());

    // Set names for all arguments.
    unsigned Idx = 0;
    for (auto& Arg : F->args())
        Arg.setName(Args[Idx++]);

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

    if (Value* RetVal = Body->codegen()) {
        Helper::TheModule->print(llvm::errs(), nullptr); // Print IR here
        // Finish off the function.
       Helper::Builder->CreateRet(RetVal);

        // Validate the generated code, checking for consistency.
        verifyFunction(*TheFunction);

        // Run the optimizer on the function.
        Helper::TheFPM->run(*TheFunction, *Helper::TheFAM);

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

    return Helper::Builder->CreateCall(CalleeF, ArgsV, "calltmp");
}
