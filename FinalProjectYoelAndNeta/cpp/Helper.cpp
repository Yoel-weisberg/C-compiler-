#include "../Header/Helper.h"

// Define static members
std::vector<std::string> Helper::definedTypes = { "float" };
std::vector<char> Helper::separeters = { SEMICOLUMN_LITERSL, LPAREN_LITERAL, RPAREN_LITERAL, EQUEL_SIGN_LITERAL };
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
}


void Helper::HandleTopLevelExpression()
{

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
