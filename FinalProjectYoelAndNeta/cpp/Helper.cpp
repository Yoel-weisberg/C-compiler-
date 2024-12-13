#include "../Header/Helper.h"

// Define static members
//std::vector<std::string> Helper::definedTypes = { FLOAT , INTEGER, CHAR};
std::vector<std::string> Helper::definedTypes = { "float" , "int", "char"};
std::vector<char> Helper::separetors = { SEMICOLON_LIT, LPAREN_LIT, RPAREN_LIT, EQUAL_SIGN_LIT };
std::vector<char> Helper::quotes = {SINGLE_QUOTE_LITERAL, DOUBLE_QUOTE_LITERAL};
SymbolTable Helper::symbolTable;

std::unique_ptr<llvm::LLVMContext> Helper::TheContext = nullptr;
std::unique_ptr<llvm::IRBuilder<>> Helper::Builder = nullptr;
std::unique_ptr<llvm::Module> Helper::TheModule = nullptr;

// Define the initialization method
void Helper::initializeModule()
{
    // Create the LLVM context and module
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("Yoel and neta JIT", *TheContext);

    // Create the IRBuilder for the module
    Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);

    createAnonymousFunction();
}


void Helper::HandleTopLevelExpression()
{

}

void Helper::createAnonymousFunction()
{
    // Create the function type (void function with no arguments)
    llvm::FunctionType* funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(getContext()), false);

    // Create the function in the module
    llvm::Function* anonFunc = llvm::Function::Create(
        funcType,
        llvm::Function::ExternalLinkage,
        "", // No name, makes it anonymous
        getModule()
    );

    // Add an entry block to the function
    llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(getContext(), "entry", anonFunc);

    // Set the builder's insertion point to the entry block
    getBuilder().SetInsertPoint(entryBlock);


    // Verify the function
    if (llvm::verifyFunction(*anonFunc, &llvm::errs())) {
        std::cerr << "Error: Invalid LLVM function generated.\n";
    }
}

bool Helper::checkIdentifier(const std::string& id)
{
    if (id.empty()) {
        return false; // Empty strings are not valid identifiers
    }

    // Check the first character: it must be a letter or underscore
    if (!(std::isalpha(id[0]) || id[0] == '_')) {
        return false;
    }

    // Check the rest of the characters: they must be letters, digits, or underscores
    for (size_t i = 1; i < id.length(); ++i) {
        if (!(std::isalnum(id[i]) || id[i] == '_')) {
            return false;
        }
    }

    return true;
}

bool Helper::isFloat(const std::string& num)
{
    std::regex decimalPattern(R"(^[-+]?[0-9]*\.[0-9]+$)");
    return std::regex_match(num, decimalPattern);
}

bool Helper::isInteger(const std::string& num)
{
    return std::all_of(num.begin(), num.end(), ::isdigit);
}

bool Helper::isChar(const std::string& ch)
{
    return (ch.length() == 3 && ch.front() == '\'' && ch.back() == '\'');
}

//uint64_t Helper::hexToDec(std::string& str)
//{
//    int stringLenght = str.length();
//
//    uint64_t uint64Value = 0x0;
//    for (int i = 0; i <= stringLenght - 1; i++)
//    {
//        char charValue = value.charAt(i);
//
//        uint64Value = 0x10 * uint64Value;
//        uint64Value += stringToHexInt(charValue);
//    }
//
//    return uint64Value;
//}

uint64_t Helper::hexToDec(std::string& str)
{
    uint64_t res = 0;
    int index = 0;
    for (int i = str.size() - 1; i >= 0; i--)
    {
        if (isalpha(str[index]))
        {
            res += (int(str[index]) - HEX_VAL) * std::pow(HEX_BASE, i);
        }
        else
        {
            res += (str[index]- '0') * std::pow(HEX_BASE, i);
        }
        index++;
    }
    return res;
}

llvm::Type* Helper::getLLVMptrType(std::string var_type, llvm::LLVMContext& Context, std::string var_name)
{
    llvm::PointerType* llvm_type;
    // Find address
    //auto symbolWrapped = symbolTable.findSymbol(var_name);
    //Symbol& pointed_to_var = symbolWrapped->get();
    //std::string address;
    //std::stringstream str_stream;
    //str_stream << pointed_to_var.getLLVMValue(); // LLVM ERROS FROM HERE!!!!!!!!!
    //unsigned long addrSpace = std::stoul(str_stream.str());
    // Identify pointer type (int, float or char)
    if (var_type == INTEGER)
    {
        return llvm_type = llvm::PointerType::get(llvm::IntegerType::get(Context, INTEGER_AND_FLOAT_SIZE), 0);
    }
    else if (var_type == FLOAT)
    {
        return llvm_type = llvm::PointerType::get(Context, 0);
    }
    else if (var_type == CHAR)
    {
        return llvm_type = llvm::PointerType::get(llvm::IntegerType::get(Context, CHAR_SIZE), 0);
    }
    else
    {
        std::cerr << "Error: Unsupported variable type '" << var_type << "'.\n";
        return nullptr;
    }
}

llvm::Type* Helper::getLLVMarrType(std::string var_type, llvm::LLVMContext& Context, const int size)
{
    // Determine the type of the array elements from pTT
    llvm::Type* elementType;
    if (var_type == INTEGER) {
        elementType = llvm::Type::getInt32Ty(Context);
    }
    else if (var_type == FLOAT) {
        elementType = llvm::Type::getFloatTy(Context);
    }
    else if (var_type == CHAR) {
        elementType = llvm::Type::getInt8Ty(Context);
    }
    else {
        std::cerr << "Error: Unsupported array element type '" << var_type << "'.\n";
        return nullptr;
    }

    // Create an LLVM array type
    return llvm::ArrayType::get(elementType, size);
}
    
llvm::Value* Helper::allocForNewSymbol(std::string var_name, std::string var_type, const int size, const std::string& pTT)
{
    llvm::IRBuilder<>& Builder = Helper::getBuilder(); // Using the Builder from Helper
    llvm::LLVMContext& Context = Helper::getContext(); // Using the Context from Helper
    llvm::Type* llvmType;
    llvm::PointerType* llvmPtrType;
    bool is_a_pointer = false;

    // Map the variable type to LLVM types
    if (var_type == INTEGER) {
        llvmType = llvm::Type::getInt32Ty(Context);
    }
    else if (var_type == FLOAT) {
        llvmType = llvm::Type::getFloatTy(Context);
    }
    else if (var_type == CHAR)
    {
        llvmType = llvm::Type::getInt8Ty(Context);
    }
    // Pointer
    else if (var_type[var_type.size() -1] == MULTIPLICATION_LIT) 
    {
        // SOURCE FOR SOLUTION --> https://stackoverflow.com/questions/9434602/creating-new-types-in-llvm-in-particular-a-pointer-to-function-type
        is_a_pointer = true;
        llvmType = getLLVMptrType(var_type.substr(0, var_type.size() - 1), Context, var_name);
    }
    else if (var_type == ARRAY)
    {
        llvmType = getLLVMarrType(pTT, Context, size);
    }
    else
    {
        std::cerr << "Error: Unsupported variable type '" << var_type << "'.\n";
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
    return tempBuilder.CreateAlloca(llvmType, nullptr, var_name.c_str());
}

bool Helper::addSymbol(std::string var_name, std::string var_type, std::string val, const std::string& pTT, const int size)
{
    if (symbolTable.findSymbol(var_name)) { // NOTE: THIS STATEMENT DOESN'T CONSIDER SCOPES 
                                            // (YET TO BE IMPLEMENTED)
        std::cerr << "Error: Symbol '" << var_name << "' already exists.\n";
        return false; // Symbol already exists
    }
    llvm::Value* var_address = allocForNewSymbol(var_name, var_type, size, pTT); 
    symbolTable.add(var_name, var_type, var_address, val, pTT);
    symbolTable.printSymbols();
    // Get the value of the variable
    //if (var_type == INTEGER)
    //{
    //    const llvm::ConstantInt& in = (llvm::ConstantInt&)(var_type);
    //    //const llvm::APInt& value_as_int = in.getValue().getZExtValue();

    //   uint64_t value_as_int = in.getValue().getZExtValue();
    //   std::cout << value_as_int << std::endl;
    //   // std::string value_as_str = Conversion::llToStr(value_as_int);
    //    //const llvm::APInt& value = llvm::dyn_cast<llvm::ConstantInt>(var_type)->getValue();
    //    ////llvm::ConstantInt* constantInt = llvm::dyn_cast<llvm::ConstantInt*>(var_type);
    //    //const llvm::APInt& value = constantInt->getValue();
    //    //std::cout << "Value of integers is...... --> " <<  value.getZExtValue() << std::endl;
    //    ////symbolTable.add(var_name, var_type, var_address, std::to_string(value.getZExtValue()));
    //    //uint64_t value_as_int = value.getZExtValue();
    //    //std::string value_as_str = Conversion::llToStr(value_as_int);
    //    symbolTable.add(var_name, var_type, var_address, value_as_str);
    //    // FIND OUT HOW TO GET THE VALE FROM THE 'GET' METHOD 
    //                 
    //}
    //if (var_type == FLOAT)
    //{
    //    return true;
    //}
    ////if ()
    ////{

    ////}
    return true;
}

std::string Helper::removeSpecialCharacter(std::string str)
{
    for (int i = 0; i < str.size(); i++) {

        // Finding the character whose
        // ASCII value fall under this
        // range
        if (str[i] < 'A' || str[i] > 'Z' && str[i] < 'a'
            || str[i] > 'z') {
            // erase function to erase
            // the character
            str.erase(i, 1);
            i--;
        }
    }
    return str;
}