#include "STable.h"

STable::STable()
{
	_varTable = {};
	_structTable = {};
	_funcTable = {};
    _scopes = {};

    // set default scope (global)
    _scopes.push_back(GLOBAL_SCOPE);
}

void STable::printAllTables()
{
	std::cout << "--------------------------------------------------------------------------------" << std::endl;
	std::cout << "|\t Name \t|\t Scope \t\|\t Type \t\|\t Address \t|" << std::endl;
	std::cout << "---------------------------------- Functions -----------------------------------" << std::endl;
	for (auto& entry : _funcTable)
	{
		std::cout << "|\t" << std::get<0>(entry) << "\t|\t" << std::get<1>(entry) << "\t|\t" << std::get<2>(entry) << "\t|\t" << std::get<3>(entry) << std::endl;
	}

	std::cout << "---------------------------------- Variables -----------------------------------" << std::endl;
	for (auto& entry : _varTable)
	{
        std::cout << "|\t" << std::get<0>(entry) << "\t|\t" << std::get<1>(entry) << "\t|\t" << std::get<2>(entry) << "\t|\t" << std::get<3>(entry) << std::endl;
    }

	std::cout << "---------------------------------- Structs -----------------------------------" << std::endl;
	for (auto& entry : _structTable)
	{
		std::cout << "|\t" << std::get<0>(entry) << "\t|\t" << std::get<1>(entry) << "\t|\t" << std::get<2>(entry) << "\t|" << std::endl;
	}
}


void STable::addVarEntry(std::string var_name, std::string var_type, const std::string& pTT, const int size)
{
    // Find Scope
    std::string scope = getScope();

    // Check if var already exists in the same scope
    auto it = std::find_if(_varTable.begin(), _varTable.end(), [&](const std::tuple<std::string, std::string, std::string, llvm::AllocaInst*>& entry) {return (std::get<0>(entry) == var_name && std::get<1>(entry) == scope); });
    if (it != _varTable.end())
    {
        std::runtime_error("Can't re-define variable");
    }
    
    // Get address
    llvm::AllocaInst* address = getVarAddress(var_name, var_type, size, pTT);

    // Make entry tuple
    std::tuple<std::string, std::string, std::string, llvm::AllocaInst*> new_entry = std::make_tuple(var_name, scope, var_type, address);

    // Add to table 
    _varTable.push_back(new_entry);
    printAllTables();

}

void STable::addStructDefinitionEntry(std::string struct_name, llvm::StructType* type)
{
    // Get scope
    std::string scope = getScope();
    
    std::tuple<std::string, std::string, llvm::StructType*> new_entry = std::make_tuple(struct_name, scope, type);

    _structTable.push_back(new_entry);
    printAllTables();
}

void STable::addStructVarEntry(std::string var_name, std::string type, llvm::AllocaInst* address)
{
    // Get scope for current var
    std::string scope = getScope();

    // Create new entry
    std::tuple<std::string, std::string, std::string, llvm::AllocaInst*> new_entry = std::make_tuple(var_name, scope, type, address);

    // Add new entry
    _varTable.push_back(new_entry);
    printAllTables();
}

void STable::addFuncEntry(std::string func_name, std::string ret_type, llvm::Function* address)
{
    // All functions have a global scope
    std::string scope = GLOBAL_SCOPE;

    // Create new entry
    std::tuple<std::string, std::string, std::string, llvm::Function*> new_entry = std::make_tuple(func_name, scope, ret_type, address);
    _funcTable.push_back(new_entry);
    printAllTables();
}

void STable::addScope(const std::string& new_scope)
{   
    _scopes.push_back(new_scope);
}

llvm::StructType* STable::getStructType(std::string type)
{
    auto structT = std::find_if(_structTable.begin(), _structTable.end(), [&](std::tuple<std::string, std::string, llvm::StructType*>& entry) {return std::get<0>(entry) == type;});
    if (structT == _structTable.end())
    {
        std::runtime_error("Invalid struct name");
        return nullptr;
    }

    return std::get<2>(*structT); // Return StructType
}

llvm::AllocaInst* STable::getVarAddress(std::string name)
{
    auto it = std::find_if(_varTable.begin(), _varTable.end(), [&](std::tuple<std::string, std::string, std::string, llvm::AllocaInst*>& entry) {return std::get<0>(entry) == name; });
    if (it == _varTable.end())
    {
        std::runtime_error("Varibale doesn't exist");
    }
    return std::get<3>(*it);
}

// Returns function return type, using the scope
std::string STable::getRetType()
{
    std::string ret_type = "";
    std::reverse(_funcTable.begin(), _funcTable.end());

    for (auto& it : _funcTable)
    {
        std::string curr_scope = Helper::removeSpecialCharacter(std::get<0>(it));
        if (std::find(Helper::scopes.begin(), Helper::scopes.end(), "-" + curr_scope + "-") != Helper::scopes.end())
        {
            // find the return type for the current function
            ret_type = std::get<2>(it);
        }
    }

    std::reverse(_funcTable.begin(), _funcTable.end());
    return ret_type;
}


std::string STable::getScope()
{
    std::string curr_scope = "";

    // Reverse scopes vector
    std::reverse(_scopes.begin(), _scopes.end());
    for (auto& scope : _scopes)
    {
        curr_scope += scope;
        // Find the head of the scope (function name)
        if (std::find(Helper::scopes.begin(), Helper::scopes.end(), scope) == Helper::scopes.end())
        {
            break;
        }
    }

    // Reverse back to original
    std::reverse(_scopes.begin(), _scopes.end());
    return curr_scope;
}

llvm::AllocaInst* STable::getVarAddress(std::string var_name, std::string var_type, const int size, const std::string& pTT)
{

    llvm::IRBuilder<>& Builder = Helper::getBuilder();
    llvm::LLVMContext& Context = Helper::getContext();
    llvm::Type* llvmType = nullptr;
    if (var_type == "*") { // Pointer
        llvmType = llvm::PointerType::getUnqual(Helper::getLLVMType(Helper::removeSpecialCharacter(var_type.substr(0, var_type.size() - 1)), Context));
    }
    var_type = Helper::removeSpecialCharacter(var_type);
    // Map variable type to LLVM type
    if (var_type == INT_TYPE_LIT) {
        llvmType = llvm::Type::getInt32Ty(Context);
    }
    else if (var_type == FLOAT_TYPE_LIT) {
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
