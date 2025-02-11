#include "../Header/Helper.h"

// Define static members
std::vector<std::string> Helper::definedTypes = { FLOAT_TYPE_LIT , VOID_TYPE_LIT, INT_TYPE_LIT , CHAR_TYPE_LIT};
std::map<std::string, Tokens_type> Helper::literalToType = {
    {")", RPAREN},
    {"(", LPAREN},
    {"+", ADDITION},
    {"*", MULTIPLICATION},
    {"/", DIVISION},
    {"-", SUBTRACTION},
    {"=", EQUAL_SIGN},
    {";", SEMICOLUMN},
    {"&&", AND},
    {"||", OR},
    {"{", L_CURLY_BRACK},
    {"}", R_CURLY_BRACK},
    {">", HIGHER_THEN},
    {"<", LOWER_THEN},
    {"==", EQUELS_CMP},
    {",", COMMA},
    {"[", SQR_BR_L},
    {"]", SQR_BR_R},
    {".", DOT},
    // Unary Operators:
    {"&", AMPERSAND},
    {"++", INCREMENT},
    {"--", DECREMENT}
};


std::map<std::string, Tokens_type> Helper::Keywords = {
    {"if", IF_WORD},
    {"else", ELSE},
    {"do", DO_WHILE_LOOP},
    {"while", WHILE_LOOP},
    {"for", FOR_LOOP},
    {"return", RETURN_STATEMENT},
    {"struct", STRUCT}
};

std::unique_ptr<llvm::LLVMContext> Helper::TheContext = nullptr;
std::unique_ptr<llvm::Module> Helper::TheModule = nullptr;
std::unique_ptr<llvm::IRBuilder<>> Helper::Builder = nullptr;
std::map<std::string, llvm::AllocaInst*> Helper::SymbolTable = {}; // Symbol Table
std::map<std::string, llvm::StructType*> Helper::StructTable = {};
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
llvm::Function* Helper::MallocFunc = nullptr;
llvm::Function* Helper::ScanfFunc = nullptr;

void Helper::InitializeModuleAndManagers()
{
    // Open a new context and module.
    TheContext = std::make_unique<LLVMContext>();
    TheModule = std::make_unique<Module>("Yoel and neta JIT", *TheContext);
    //TheModule->setDataLayout(TheJIT->getDataLayout());

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

    defineMalloc();
    defineScanf();
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

bool Helper::isInteger(const std::string& num)
{
    return std::all_of(num.begin(), num.end(), ::isdigit);
}

bool Helper::isChar(const std::string& ch)
{
    return (ch.length() == 3 && ch.front() == '\'' && ch.back() == '\'');
}

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



llvm::Type* Helper::getLLVMType(std::string var_type, llvm::LLVMContext& Context)
{
    // Determine the type of the array elements from pTT
    llvm::Type* elementType;
    if (var_type == INT_TYPE_LIT ) {
        elementType = llvm::Type::getInt32Ty(Context);
    }
    else if (var_type == FLOAT_TYPE_LIT ) {
        elementType = llvm::Type::getFloatTy(Context);
    }
    else if (var_type == CHAR_TYPE_LIT) {
        elementType = llvm::Type::getInt8Ty(Context);
    }
    else {
        std::cerr << "Error: Unsupported array element type '" << var_type << "'.\n";
        return nullptr;
    }
    return elementType;
}

void Helper::defineMalloc() {
    // Check if malloc is already declared
    if (MallocFunc != nullptr) {
        return;
    }

    // Declare malloc in the module
    MallocFunc = llvm::cast<llvm::Function>(
        TheModule->getOrInsertFunction(
            "malloc",                                   // Function name
            PointerType::get(Type::getInt8Ty(getContext()), 0),                   // Return type: void*
            Builder->getInt32Ty()                      // Parameter type: int
        ).getCallee()
    );

    // Print confirmation
    llvm::outs() << "Declared malloc function.\n";
}

void Helper::defineScanf() {
    // Check if scanf is already declared
    if (ScanfFunc != nullptr) {
        return;
    }

    // Declare scanf in the module
    ScanfFunc = llvm::cast<llvm::Function>(
        TheModule->getOrInsertFunction(
            "scanf",                                           // Function name
            Builder->getInt32Ty(),                             // Return type: int
            llvm::PointerType::get(llvm::Type::getInt8Ty(getContext()), 0) // Parameter type: char* (format string)
        ).getCallee()
    );

    // Print confirmation
    llvm::outs() << "Declared scanf function.\n";
}

void Helper::builfObjectFile()
{
    // finding the machines attributes
    auto TargetTriple = sys::getDefaultTargetTriple();
    TheModule->setTargetTriple(TargetTriple);

    std::string Error;
    auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

    // Print an error and exit if we couldn't find the requested target.
    // This generally occurs if we've forgotten to initialise the
    // TargetRegistry or we have a bogus target triple.
    if (!Target) {
        errs() << Error;
        return;
    }

    auto CPU = "generic";
    auto Features = "";

    TargetOptions opt;
    auto TheTargetMachine = Target->createTargetMachine(
        TargetTriple, CPU, Features, opt, Reloc::PIC_);

    TheModule->setDataLayout(TheTargetMachine->createDataLayout());

    auto Filename = OBJECT_FILE_LOC;
    std::error_code EC;
    raw_fd_ostream dest(Filename, EC, sys::fs::OF_None);

    if (EC) {
        errs() << "Could not open file: " << EC.message();
        return;
    }

    legacy::PassManager pass;
    auto FileType = CodeGenFileType::ObjectFile;

    if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
        errs() << "TheTargetMachine can't emit a file of this type";
        return;
    }

    pass.run(*TheModule);
    dest.flush();

    outs() << "Wrote " << Filename << "\n";

}





    
llvm::AllocaInst* Helper::allocForNewSymbol(std::string var_name, std::string var_type, const int size, const std::string& pTT)
{
   
    llvm::IRBuilder<>& Builder = Helper::getBuilder();
    llvm::LLVMContext& Context = Helper::getContext();
    llvm::Type* llvmType = nullptr;
    if (var_type == "*") { // Pointer
        llvmType = llvm::PointerType::getUnqual(Helper::getLLVMType(Helper::removeSpecialCharacter(var_type.substr(0, var_type.size() - 1)), Context));
    }
    var_type = Helper::removeSpecialCharacter(var_type);
    // Map variable type to LLVM type
    if (var_type == INT_TYPE_LIT ) {
        llvmType = llvm::Type::getInt32Ty(Context);
    }
    else if (var_type == FLOAT_TYPE_LIT ) {
        llvmType = llvm::Type::getFloatTy(Context);
    }
    else if (var_type == CHAR_TYPE_LIT) {
        llvmType = llvm::Type::getInt8Ty(Context);
    }
    else if (var_type == "arr") {
        llvmType = llvm::ArrayType::get(Helper::getLLVMType(pTT, Context), size);
    }
    else {
        std::cerr << "Error: Unsupported variable type '" << var_type << "'.\n";
        return nullptr;
    }

    // Ensure the function context
    llvm::Function* currentFunction = Builder.GetInsertBlock()->getParent();
    if (!currentFunction) {
        std::cerr << "Error: Attempting to allocate variable outside of a function.\n";
        return nullptr;
    }

    // Allocate memory in the entry block
    llvm::IRBuilder<> tempBuilder(&currentFunction->getEntryBlock(), currentFunction->getEntryBlock().begin());
    llvm::AllocaInst* allocaInst = tempBuilder.CreateAlloca(llvmType, nullptr, var_name);

    return allocaInst; // LLVM automatically tracks the name in its symbol table
}

llvm::Value* Helper::getSymbolValue(const std::string& var_name) 
{
    llvm::IRBuilder<>& Builder = Helper::getBuilder();
    llvm::Function* currentFunction = Builder.GetInsertBlock()->getParent();
    if (!currentFunction) {
        std::cerr << "Error: Attempting to access variable outside of a function.\n";
        return nullptr;
    }

    // Access the ValueSymbolTable of the function
    llvm::ValueSymbolTable* symbolTable = currentFunction->getValueSymbolTable();
    if (!symbolTable) {
        std::cerr << "Error: No symbol table found in the current function.\n";
        return nullptr;
    }

    llvm::Value* symbol = symbolTable->lookup(var_name);

    if (!symbol) {
        std::cerr << "Error: Variable '" << var_name << "' not found in symbol table.\n";
    }
    return symbol;
}


bool Helper::addSymbol(std::string var_name, std::string var_type,const std::string& pTT, const int size)
{
    if (SymbolTable.find(var_name) != SymbolTable.end()) { // NOTE: THIS STATEMENT DOESN'T CONSIDER SCOPES 
                                            // (YET TO BE IMPLEMENTED)
        std::cerr << "Error: Symbol '" << var_name << "' already exists.\n";
        return false; // Symbol already exists
    }
    llvm::AllocaInst* var_address = allocForNewSymbol(var_name, var_type, size, pTT);

    Helper::SymbolTable[var_name] = var_address; // Add to symbol table 
    printLLVMSymbolTable();
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

Type* Helper::getTypeFromString(const std::string type)
{
    if (type == INT_TYPE_LIT )
    {
        return Type::getInt32Ty(Helper::getContext());
    }
    else if (type == FLOAT_TYPE_LIT )
    {
        return Type::getDoubleTy(Helper::getContext());
    }
    else if (type == CHAR_TYPE_LIT)
    {
        return Type::getInt8Ty(Helper::getContext());
    }
    else if(type == VOID_TYPE_LIT)
    {
        return Type::getVoidTy(Helper::getContext());
    }
}


void Helper::printLLVMSymbolTable() {
    std::cout << "Symbol Table:\n";
    std::cout << "----------------------------------\n";
    std::cout << "| Variable Name | Type    | Address |\n";
    std::cout << "----------------------------------\n";

    for (const auto& entry : Helper::SymbolTable) {
        const std::string& varName = entry.first;
        llvm::AllocaInst* allocaInst = entry.second;

        // Get the type of the variable
        llvm::Type* type = allocaInst->getAllocatedType();

        // Convert type to a string
        std::string typeStr;
        llvm::raw_string_ostream rso(typeStr);
        type->print(rso);

        // Print the variable name, type, and memory address
        std::cout << "| " << varName << " | " << typeStr << " | "
            << allocaInst << " |\n";
    }

    std::cout << "----------------------------------\n";
}
