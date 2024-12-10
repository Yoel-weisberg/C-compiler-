#include "../Header/Helper.h"

// Define static members
std::vector<std::string> Helper::definedTypes = { "float" };
std::map<std::string, Tokens_type> Helper::literalToType = {
    {")", RPAREN},
    {"(", LPAREN},
    {"+", ADDITION},
    {"*", MULTIPLICATION},
    {"/", DIVISION},
    {"-", SUBTRACTION},
    {"=", EQUEL_SIGN},
    {";", SEMICOLUMN},
    {"&&", AND},
    {"||", OR},
    {"{", L_CURLY_PRAN},
    {"}", R_CURLY_PRAN},
    {">", HIGHER_THEN},
    {"<", LOWER_THEN},
    {"==", EQUELS_CMP},
    {",", COMMA}
};

SymbolTable Helper::symbolTable;

std::unique_ptr<llvm::LLVMContext> Helper::TheContext = nullptr;
std::unique_ptr<llvm::Module> Helper::TheModule = nullptr;
std::unique_ptr<llvm::IRBuilder<>> Helper::Builder = nullptr;
std::map<std::string, llvm::Value*> Helper::NamedValues = {};
std::unique_ptr<llvm::orc::KaleidoscopeJIT> Helper::TheJIT = nullptr;
std::unique_ptr<llvm::FunctionPassManager> Helper::TheFPM = nullptr;
std::unique_ptr<llvm::LoopAnalysisManager> Helper::TheLAM = nullptr;
std::unique_ptr<llvm::FunctionAnalysisManager> Helper::TheFAM = nullptr;
std::unique_ptr<llvm::CGSCCAnalysisManager> Helper::TheCGAM = nullptr;
std::unique_ptr<llvm::ModuleAnalysisManager> Helper::TheMAM = nullptr;
std::unique_ptr<llvm::PassInstrumentationCallbacks> Helper::ThePIC = nullptr;
std::unique_ptr<llvm::StandardInstrumentations> Helper::TheSI = nullptr;
std::map<std::string, std::unique_ptr<PrototypeAST>> Helper::FunctionProtos;
llvm::ExitOnError Helper::ExitOnErr;

void Helper::InitializeModuleAndManagers()
{
    // Open a new context and module.
    TheContext = std::make_unique<LLVMContext>();
    TheModule = std::make_unique<Module>("Yoel and neta JIT", *TheContext);
    TheModule->setDataLayout(TheJIT->getDataLayout());

    // Create a new builder for the module.
    Builder = std::make_unique<IRBuilder<>>(*TheContext);

    // Create new pass and analysis managers.
    TheFPM = std::make_unique<FunctionPassManager>();
    TheLAM = std::make_unique<LoopAnalysisManager>();
    TheFAM = std::make_unique<FunctionAnalysisManager>();
    TheCGAM = std::make_unique<CGSCCAnalysisManager>();
    TheMAM = std::make_unique<ModuleAnalysisManager>();
    ThePIC = std::make_unique<PassInstrumentationCallbacks>();
    TheSI = std::make_unique<StandardInstrumentations>(*TheContext,
        /*DebugLogging*/ true);
    TheSI->registerCallbacks(*ThePIC, TheMAM.get());

    // Add transform passes.
    // Do simple "peephole" optimizations and bit-twiddling optzns.
    TheFPM->addPass(InstCombinePass());
    // Reassociate expressions.
    TheFPM->addPass(ReassociatePass());
    // Eliminate Common SubExpressions.
    TheFPM->addPass(GVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).
    TheFPM->addPass(SimplifyCFGPass());

    // Register analysis passes used in these transform passes.
    PassBuilder PB;
    PB.registerModuleAnalyses(*TheMAM);
    PB.registerFunctionAnalyses(*TheFAM);
    PB.crossRegisterProxies(*TheLAM, *TheFAM, *TheCGAM, *TheMAM);
}



Function* Helper::getFunction(std::string Name)
{
    // First, see if the function has already been added to the current module.
    if (auto* F = TheModule->getFunction(Name))
        return F;

    // If not, check whether we can codegen the declaration from some existing
    // prototype.
    auto FI = FunctionProtos.find(Name);
    if (FI != FunctionProtos.end())
        return FI->second->codegen();

    // If no existing prototype exists, return null.
    return nullptr;
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
