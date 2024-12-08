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
    {"==", EQUELS_CMP}
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

    // Create the IRBuilder for the module
    Helper::Builder = std::make_unique<llvm::IRBuilder<>>(Helper::getContext());


    // Create new pass and analysis managers.
    Helper::TheFPM = std::make_unique<llvm::FunctionPassManager>();
    Helper::TheLAM = std::make_unique<llvm::LoopAnalysisManager>();
    Helper::TheFAM = std::make_unique<llvm::FunctionAnalysisManager>();
    Helper::TheCGAM = std::make_unique<llvm::CGSCCAnalysisManager>();
    Helper::TheMAM = std::make_unique<llvm::ModuleAnalysisManager>();
    Helper::ThePIC = std::make_unique<llvm::PassInstrumentationCallbacks>();
    Helper::TheSI = std::make_unique<llvm::StandardInstrumentations>(Helper::getContext(),
        /*DebugLogging*/ true);
    Helper::TheSI->registerCallbacks(*Helper::ThePIC, Helper::TheMAM.get());

    // Add transform passes.
    // Do simple "peephole" optimizations and bit-twiddling optzns.
    Helper::TheFPM->addPass(llvm::InstCombinePass());
    // Reassociate expressions.
    Helper::TheFPM->addPass(llvm::ReassociatePass());
    // Eliminate Common SubExpressions.
    Helper::TheFPM->addPass(llvm::GVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).
    Helper::TheFPM->addPass(llvm::SimplifyCFGPass());

    // Register analysis passes used in these transform passes.
    llvm::PassBuilder PB;
    PB.registerModuleAnalyses(*Helper::TheMAM);
    PB.registerFunctionAnalyses(*Helper::TheFAM);
    PB.crossRegisterProxies(*Helper::TheLAM, *Helper::TheFAM, *Helper::TheCGAM, *Helper::TheMAM);
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
