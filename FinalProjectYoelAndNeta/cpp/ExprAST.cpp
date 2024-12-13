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
    Constant* con = ConstantInt::get(Type::getInt64Ty(Helper::getContext()), Helper::hexToDec(_valAsStr));
    return ConstantExpr::getIntToPtr(con, PointerType::getUnqual(Helper::getBuilder().getInt32Ty()));
}



Value* arrExprAST::codegen()
{
    // Need to specify type and size 
    llvm::ArrayType* arrT = llvm::ArrayType::get(_type, _size);
    return ConstantInt::get(arrT, APInt(_size, _data));
}

void arrExprAST::assignLLVMType(const std::string& type)
{
    //llvm::Types
    ////_type = 
    //if (type == INTEGER)
    //{
    //    _type = llvm::IntegerType::get(Helper::getContext(), 0); // Only for testing!!! do not keep it zero!!!!
    //}
    _type = llvm::IntegerType::get(Helper::getContext(), 0); // Only for testing!!! do not keep it zero!!!!
}

void arrExprAST::initArrayRef(const std::string& val, const std::string& type)
{
    // create vector for array inner values by type
    std::vector<std::variant<int, float, char>> values;
    for (size_t i = 0; i < val.size(); i++) // No need to check types or validation,
    {                                    // So we can just add it to 'values'
        if (val[i] == COMMA_LIT)
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
        else {
            // Handle unexpected case, if necessary
        }
    }

    // Use arrayRef C'tor that uses the iteration through values
    _data = llvm::ArrayRef<uint64_t>(transformedValues);
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
