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
    ////return ConstantInt::get(Helper::getContext(), )
    //const int addr = std::stoi(_valAsStr);
    //llvm::Type* = 
    ////return ConstantExpr::getIntToPtr(, llvm::PointerType::getUnqual(Type::getInt32Ty));
    //llvm::ConstantInt * beg = ConstantInt::get(llvm::Type::getInt32Ty, (uint64_t)&addr);
    //ConstantInt::get()
    //return ConstantInt::get(Helper::getContext(), APInt(_size, std::stoi(_valAsStr)));
    ////return ConstantPointerNull::get(llvm::PointerType::getInt32Ty);
    //uint64_t value;
    //_valAsStr >> value;
    //iss >> value;
    //std::hex(_valAsStr);
    //auto s = reinterpret_cast<uintptr_t>(_valAsStr);
    uint64_t res = Helper::hexToDec(_valAsStr);
    std::cout << " " << _valAsStr << " String to int --> " << std::stoi(_valAsStr) << std::endl;
    Constant* con = ConstantInt::get(Type::getInt64Ty(Helper::getContext()), (uint64_t)std::stoi(_valAsStr)); 
    //Constant* Sb = ConstantInt::get(Type::getInt64Ty(Helper::getContext()),value);

    return ConstantExpr::getIntToPtr(con, PointerType::getUnqual(Type::getInt64Ty(Helper::getContext())));
    //return PointerType::get(Helper::getContext(), Type::getPointerAddressSpace());
}


Value* VariableExprAST::codegen()
{
	// Look this variable up in the function.
    Value* V = Helper::symbolTable.findSymbol(_name)->get().getLLVMValue();
	if (!V)
		throw SyntaxError("Not defined variable");
	return V;
}



// code like int a = 5;
Value* AssignExprAST::codegen() {
    // Check if the variable already exists in the symbol table
    auto symbolOpt = Helper::symbolTable.findSymbol(_varName);
    //if (!symbolOpt) {
    //    //// Variable is not declared, allocate and add it to the symbol table
    //    //Value* varAddress = Helper::allocForNewSymbol(_varName, _varType);

    //    //// Add the variable to the symbol table
    //    //Helper::symbolTable.add(_varName, _varType, varAddress);
    //    //AssignExprAST* x = std::make_unique<AssignExprAST>(_value)->getValue();
    //    //Helper::addSymbol(_varName, _varType);

    //    // Re-fetch the symbol from the symbol table
    //    symbolOpt = Helper::symbolTable.findSymbol(_varName);
    //}

    if (!symbolOpt) {
        std::cerr << "Error: Failed to retrieve the variable '" << _varName << "' after adding it.\n";
        return nullptr;
    }

    Symbol& symbol = symbolOpt->get();

    // Generate code for the right-hand side expression (only once)
    llvm::Value* rhsValue = _value->codegen();
    if (!rhsValue) {
        std::cerr << "Error: Failed to generate code for the assigned value.\n";
        return nullptr;
    }

    // Retrieve the LLVM value (address) of the variable
    llvm::Value* varAddress = symbol.getLLVMValue();
    if (!varAddress) {
        std::cerr << "Error: Variable '" << _varName << "' has no allocated memory.\n";
        return nullptr;
    }

    // Store the RHS value in the variable
    llvm::IRBuilder<>& Builder = Helper::getBuilder(); // Use the Builder from Helper
    Builder.CreateStore(rhsValue, varAddress);

    return rhsValue; // Return the RHS value for chaining if needed
}


