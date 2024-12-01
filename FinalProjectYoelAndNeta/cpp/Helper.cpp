#include "../Header/Helper.h"

// Define static members
std::vector<std::string> Helper::definedTypes = { FLOAT , INTEGER, CHAR};
std::vector<char> Helper::separetors = { SEMICOLON_LITERAL, LPAREN_LITERAL, RPAREN_LITERAL, EQUEL_SIGN_LITERAL };
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



