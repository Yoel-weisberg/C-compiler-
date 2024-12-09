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
    // Create the LLVM context and module
    Helper::TheContext = std::make_unique<llvm::LLVMContext>();
    Helper::TheModule = std::make_unique<llvm::Module>("Yoel and neta JIT", Helper::getContext());

    // Initialize the JIT
    if (!Helper::TheJIT) {
        auto JITOrError = llvm::orc::KaleidoscopeJIT::Create();
        if (!JITOrError) {
            llvm::logAllUnhandledErrors(JITOrError.takeError(), llvm::errs(),
                "Error creating JIT: ");
            return;
        }
        Helper::TheJIT = std::move(*JITOrError);
    }

    // Set the Module's DataLayout to match the JIT's DataLayout
    Helper::TheModule->setDataLayout(Helper::TheJIT->getDataLayout());


    // Create the IRBuilder for the module
    Helper::Builder = std::make_unique<llvm::IRBuilder<>>(Helper::getContext());

    // Create new pass and analysis managers
    Helper::TheFPM = std::make_unique<llvm::FunctionPassManager>();
    Helper::TheLAM = std::make_unique<llvm::LoopAnalysisManager>();
    Helper::TheFAM = std::make_unique<llvm::FunctionAnalysisManager>();
    Helper::TheCGAM = std::make_unique<llvm::CGSCCAnalysisManager>();
    Helper::TheMAM = std::make_unique<llvm::ModuleAnalysisManager>();
    Helper::ThePIC = std::make_unique<llvm::PassInstrumentationCallbacks>();
    Helper::TheSI = std::make_unique<llvm::StandardInstrumentations>(Helper::getContext(),
        /*DebugLogging*/ true);
    Helper::TheSI->registerCallbacks(*Helper::ThePIC, Helper::TheMAM.get());

    // Add transform passes
    Helper::TheFPM->addPass(llvm::InstCombinePass());      // Do simple optimizations
    Helper::TheFPM->addPass(llvm::ReassociatePass());      // Reassociate expressions
    Helper::TheFPM->addPass(llvm::GVNPass());             // Eliminate common subexpressions
    Helper::TheFPM->addPass(llvm::SimplifyCFGPass());     // Simplify the control flow graph

    // Register analysis passes used in these transform passes
    llvm::PassBuilder PB;
    PB.registerModuleAnalyses(*Helper::TheMAM);
    PB.registerFunctionAnalyses(*Helper::TheFAM);
    PB.crossRegisterProxies(*Helper::TheLAM, *Helper::TheFAM, *Helper::TheCGAM, *Helper::TheMAM);
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
