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
Value* AssignExprAST::codegen() {
    // Check if the variable already exists in the symbol table
    auto symbolOpt = Helper::symbolTable.findSymbol(_VarName);
    if (!symbolOpt) {
        // Variable is not declared, allocate and add it to the symbol table
        llvm::IRBuilder<>& Builder = Helper::getBuilder(); // Using the Builder from Helper
        llvm::LLVMContext& Context = Helper::getContext(); // Using the Context from Helper
        llvm::Type* llvmType;

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

        // Ensure we are in the context of a function
        llvm::Function* currentFunction = Builder.GetInsertBlock()->getParent();
        if (!currentFunction) {
            std::cerr << "Error: Attempting to allocate variable outside of a function.\n";
            return nullptr;
        }

        // Move the builder's insertion point to the function's entry block
        llvm::IRBuilder<> tempBuilder(&currentFunction->getEntryBlock(),
            currentFunction->getEntryBlock().begin());

        // Allocate memory for the variable at the entry block
        llvm::Value* varAddress = tempBuilder.CreateAlloca(llvmType, nullptr, _VarName.c_str());


        // Add the variable to the symbol table
        Helper::symbolTable.addSymbol(_VarName, _varType, varAddress);

        // Re-fetch the symbol from the symbol table
        symbolOpt = Helper::symbolTable.findSymbol(_VarName);
    }

    if (!symbolOpt) {
        std::cerr << "Error: Failed to retrieve the variable '" << _VarName << "' after adding it.\n";
        return nullptr;
    }

    Symbol& symbol = symbolOpt->get();

    // Generate code for the right-hand side expression (only once)
    llvm::Value* rhsValue = _Value->codegen();
    if (!rhsValue) {
        std::cerr << "Error: Failed to generate code for the assigned value.\n";
        return nullptr;
    }

    // Retrieve the LLVM value (address) of the variable
    llvm::Value* varAddress = symbol.getLLVMValue();
    if (!varAddress) {
        std::cerr << "Error: Variable '" << _VarName << "' has no allocated memory.\n";
        return nullptr;
    }

    // Store the RHS value in the variable
    llvm::IRBuilder<>& Builder = Helper::getBuilder(); // Use the Builder from Helper
    Builder.CreateStore(rhsValue, varAddress);

    return rhsValue; // Return the RHS value for chaining if needed
}

Value* IfExprAST::codegen()
{
    llvm::IRBuilder<>& Builder = Helper::getBuilder(); // Using the Builder from Helper
    llvm::LLVMContext& Context = Helper::getContext(); // Using the Context from Helper

    Value* CondV = Cond->codegen();
    if (!CondV)
        return nullptr;

    // Convert condition to a bool by comparing non-equal to 0.0.
    CondV = Builder.CreateFCmpONE(
        CondV, ConstantFP::get(Context, APFloat(0.0)), "ifcond");

    Function* TheFunction = Builder.GetInsertBlock()->getParent();

    // Create blocks for the then and else cases.  Insert the 'then' block at the
    // end of the function.
    BasicBlock* ThenBB = BasicBlock::Create(Context, "then", TheFunction);
    BasicBlock* ElseBB = BasicBlock::Create(Context, "else");
    BasicBlock* MergeBB = BasicBlock::Create(Context, "ifcont");

    Builder.CreateCondBr(CondV, ThenBB, ElseBB);

    // Emit then value.
    Builder.SetInsertPoint(ThenBB);

    Value* ThenV = Then->codegen();
    if (!ThenV)
        return nullptr;

    Builder.CreateBr(MergeBB);
    // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
    ThenBB = Builder.GetInsertBlock();

    // Emit else block.
    TheFunction->insert(TheFunction->end(), ElseBB);
    Builder.SetInsertPoint(ElseBB);

    Value* ElseV = Else->codegen();
    if (!ElseV)
        return nullptr;

    Builder.CreateBr(MergeBB);
    // Codegen of 'Else' can change the current block, update ElseBB for the PHI.
    ElseBB = Builder.GetInsertBlock();

    // Emit merge block.
    TheFunction->insert(TheFunction->end(), MergeBB);
    Builder.SetInsertPoint(MergeBB);
    PHINode* PN = Builder.CreatePHI(Type::getDoubleTy(Context), 2, "iftmp");

    PN->addIncoming(ThenV, ThenBB);
    PN->addIncoming(ElseV, ElseBB);
    return PN;
}
