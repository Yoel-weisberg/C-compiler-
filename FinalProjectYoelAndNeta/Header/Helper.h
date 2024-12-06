#pragma once

#include <iostream>
#include <vector>
#include "Constents.h"
#include <regex>
#include "SymbalTable.h"
#include <map>
#include "Constents.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <memory>

class Helper
{
public:
    // llvm related
    static std::unique_ptr<llvm::LLVMContext> TheContext;
    static std::unique_ptr<llvm::IRBuilder<>> Builder;
    static std::unique_ptr<llvm::Module> TheModule;

    // Initialization of the static members
    static void initializeModule();

    // Accessor methods to get the objects
    static llvm::LLVMContext& getContext() { return *TheContext; }
    static llvm::IRBuilder<>& getBuilder() { return *Builder; }
    static llvm::Module& getModule() { return *TheModule; }
    static void HandleTopLevelExpression();
    static void createAnonymousFunction();

    // Utility methods
    static bool checkIdentifier(const std::string& id);
    static bool isFloat(const std::string& num);

    // Data members
    static std::vector<std::string> definedTypes;
    static std::map<std::string, Tokens_type> literalToType;
    static SymbolTable symbolTable;
};
