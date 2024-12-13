#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#pragma once

#include <llvm/IR/Value.h>
#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include <algorithm>

class Symbol {
public:
    Symbol(const std::string& name, const std::string& type, llvm::Value* llvmValue, std::string& val, const std::string& pTT);

    // Accessor methods
    const std::string& getName() const;
    const std::string& getType() const;
    llvm::Value* getLLVMValue() const;
    const std::string& getValue() const;
    const std::string& getPointToType() const;

    // Modifier methods
    void setLLVMValue(llvm::Value* value);

private:
    std::string _name;      // Name of the symbol
    std::string _type;      // Type of the symbol
    std::string _val;       // Value (content) of the symbol as string
    llvm::Value* _llvmValue; // LLVM representation of the symbol (address?)

    // Optional
    std::string _pointToType;
};

class SymbolTable {
public:
    SymbolTable();

    // Add a new symbol to the table
    void add(const std::string& name, const std::string& type, llvm::Value* llvmValue, std::string& val, const std::string& pTT = "");

    // Find a symbol by name
    std::optional<std::reference_wrapper<Symbol>> findSymbol(const std::string& name);

    // Update the LLVM value of an existing symbol
    bool updateSymbol(const std::string& name, llvm::Value* newLLVMValue);

    // Print all symbols (for debugging)
    void printSymbols() const;


private:
    std::vector<Symbol> _table; // Vector of Symbol objects
};

#endif // SYMBOL_TABLE_H
