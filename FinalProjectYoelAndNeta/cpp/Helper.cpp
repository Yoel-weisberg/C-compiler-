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


    // Create new pass and analysis managers.
    auto TheFPM = std::make_unique<llvm::FunctionPassManager>();
    auto TheLAM = std::make_unique<llvm::LoopAnalysisManager>();
    auto TheFAM = std::make_unique<llvm::FunctionAnalysisManager>();
    auto TheCGAM = std::make_unique<llvm::CGSCCAnalysisManager>();
    auto TheMAM = std::make_unique<llvm::ModuleAnalysisManager>();
    auto ThePIC = std::make_unique<llvm::PassInstrumentationCallbacks>();
    auto TheSI = std::make_unique<llvm::StandardInstrumentations>(*TheContext,
        /*DebugLogging*/ true);
    TheSI->registerCallbacks(*ThePIC, TheMAM.get());

    // Add transform passes.
    // Do simple "peephole" optimizations and bit-twiddling optzns.
    TheFPM->addPass(llvm::InstCombinePass());
    // Reassociate expressions.
    TheFPM->addPass(llvm::ReassociatePass());
    // Eliminate Common SubExpressions.
    TheFPM->addPass(llvm::GVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).
    TheFPM->addPass(llvm::SimplifyCFGPass());

    // Register analysis passes used in these transform passes.
    llvm::PassBuilder PB;
    PB.registerModuleAnalyses(*TheMAM);
    PB.registerFunctionAnalyses(*TheFAM);
    PB.crossRegisterProxies(*TheLAM, *TheFAM, *TheCGAM, *TheMAM);
}


void Helper::HandleTopLevelExpression()
{
    // Evaluate a top-level expression into an anonymous function.
    if (auto FnAST = ParseTopLevelExpr()) {
        if (FnAST->codegen()) {
            // Create a ResourceTracker to track JIT'd memory allocated to our
            // anonymous expression -- that way we can free it after executing.
            auto RT = TheJIT->getMainJITDylib().createResourceTracker();

            auto TSM = ThreadSafeModule(std::move(TheModule), std::move(TheContext));
            ExitOnErr(TheJIT->addModule(std::move(TSM), RT));
            InitializeModuleAndManagers();

            // Search the JIT for the __anon_expr symbol.
            auto ExprSymbol = ExitOnErr(TheJIT->lookup("__anon_expr"));

            // Get the symbol's address and cast it to the right type (takes no
            // arguments, returns a double) so we can call it as a native function.
            double (*FP)() = ExprSymbol.getAddress().toPtr<double (*)()>();
            fprintf(stderr, "Evaluated to %f\n", FP());

            // Delete the anonymous expression module from the JIT.
            ExitOnErr(RT->remove());
        }
    }
    else {
        // Skip token for error recovery.
        getNextToken();
    }
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
