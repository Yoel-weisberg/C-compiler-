#include "SymbalTable.h"

// Implementation of Symbol class
Symbol::Symbol(const std::string& name, const std::string& type, llvm::Value* llvmValue)
    : _name(name), _type(type), _llvmValue(llvmValue) {}

const std::string& Symbol::getName() const {
    return _name;
}

const std::string& Symbol::getType() const {
    return _type;
}

llvm::Value* Symbol::getLLVMValue() const {
    return _llvmValue;
}

void Symbol::setLLVMValue(llvm::Value* value) {
    _llvmValue = value;
}

// Implementation of SymbolTable class
SymbolTable::SymbolTable() = default;

bool SymbolTable::addSymbol(const std::string& name, const std::string& type, llvm::Value* llvmValue) {
    if (findSymbol(name)) {
        std::cerr << "Error: Symbol '" << name << "' already exists.\n";
        return false; // Symbol already exists
    }
    _table.emplace_back(name, type, llvmValue);
    return true;
}

std::optional<std::reference_wrapper<Symbol>> SymbolTable::findSymbol(const std::string& name) {
    auto it = std::find_if(_table.begin(), _table.end(), [&name](const Symbol& symbol) {
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
    std::cout << "Symbol Table:\n";
    for (const auto& symbol : _table) {
        std::cout << "Name: " << symbol.getName()
            << ", Type: " << symbol.getType()
            << ", LLVM Value: " << symbol.getLLVMValue() << "\n";
    }
}
