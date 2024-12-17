#include "SymbolTable.h"

// Implementation of Symbol class
Symbol::Symbol(const std::string& name, const std::string& type, llvm::Value* llvmValue, std::string& val, const std::string& pTT)
    : _name(name), _type(type), _llvmValue(llvmValue) , _val(val), _pointToType(pTT){}

const std::string& Symbol::getName() const {
    return _name;
}

const std::string& Symbol::getType() const {
    return _type;
}

llvm::Value* Symbol::getLLVMValue() const {
    return _llvmValue;
}

const std::string& Symbol::getValue() const
{
    return _val;
}

const std::string& Symbol::getPointToType() const
{
    return _pointToType;
}

void Symbol::setLLVMValue(llvm::Value* value) {
    _llvmValue = value;
}

// Implementation of SymbolTable class
SymbolTable::SymbolTable() = default;

void SymbolTable::add(const std::string& name, const std::string& type, llvm::Value* llvmValue, std::string& val, const std::string& pTT)
{
    _table.emplace_back(name, type, llvmValue, val, pTT);
}

std::optional<std::reference_wrapper<Symbol>> SymbolTable::findSymbol(const std::string& name) {
    auto it = std::find_if(_table.begin(), _table.end(), [&name](const Symbol& symbol)
        {
        return symbol.getName() == name;
        });
    if (it != _table.end()) {
        return *it;
    }
    return std::nullopt; // Symbol not found
}

bool SymbolTable::updateSymbol(const std::string& name, llvm::Value* newLLVMValue) {
    auto symbolOpt = findSymbol(name);
    if (symbolOpt) {
        symbolOpt->get().setLLVMValue(newLLVMValue);
        return true;
    }
    std::cerr << "Error: Symbol '" << name << "' not found.\n";
    return false; // Symbol not found
}

void SymbolTable::printSymbols() const {
    std::cout << "     ######### ----------- Symbol Table ----------- #########\n";
    std::cout << "Name  \ttype  \taddress  \tvalue  \tpointToType (optional)" << std::endl;
    std::cout << " -------------------------------------------------------------------" << std::endl;
    for (const auto& symbol : _table) {
        std::cout << symbol.getName() << "\t" << symbol.getType() << "\t" << symbol.getLLVMValue() << "\t" << symbol.getValue() << "\t" << symbol.getPointToType() << std::endl;
        std::cout << " -------------------------------------------------------------------" << std::endl;
    }
    std::cout << "###################################################################" << std::endl;
}


