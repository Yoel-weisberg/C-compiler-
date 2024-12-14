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



Value* arrExprAST::codegen() {
    // Retrieve the pointer to the allocated array
    AllocaInst* arrayPtr = Helper::namedValues[_name]; // Replace "array_name" with your actual variable
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
        builder.CreateStore(elementValue, elementPtr);
    }

    return arrayPtr;
}





void arrExprAST::assignLLVMType(const std::string& type) {
    LLVMContext& context = Helper::getContext(); // Access the global LLVM context

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
    // Lookup the variable in the symbol table
    AllocaInst* variable = Helper::namedValues[_name];
    if (!variable) {
        throw std::runtime_error("Unknown variable name: " + _name);
    }

    // Generate a load instruction to load the value of the variable
    return Helper::getBuilder().CreateLoad(variable->getAllocatedType(), variable, _name);
}



// code like int a = 5;
Value* AssignExprAST::codegen() {
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

