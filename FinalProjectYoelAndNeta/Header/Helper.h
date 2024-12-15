#pragma once
#define _OPEN_SYS_ITOA_EXT

#include <iostream>
#include <vector>
#include <memory>
#include <regex>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <map>      


#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include "llvm/IR/ValueSymbolTable.h"


#include "Constants.h"
#include "SymbolTable.h"



class Helper
{
public:
    // llvm related
    static std::unique_ptr<llvm::LLVMContext> TheContext;
    static std::unique_ptr<llvm::IRBuilder<>> Builder;
    static std::unique_ptr<llvm::Module> TheModule;
    static std::map<std::string, llvm::AllocaInst*> namedValues; // New symbol table

    // Initialization of the static members
    static void initializeModule();

    // Accessor methods to get the objects
    static llvm::LLVMContext& getContext() { return *TheContext; }
    static llvm::IRBuilder<>& getBuilder() { return *Builder; }
    static llvm::Module& getModule() { return *TheModule; }
    static void HandleTopLevelExpression();
    static void createAnonymousFunction();
    static llvm::AllocaInst* allocForNewSymbol(std::string var_name, std::string var_type, const int size, const std::string& pTT);
    static bool addSymbol(std::string var_name, std::string var_type, const std::string& pTT = "", const int size = 1);
    

    // Utility methods
    static bool checkIdentifier(const std::string& id);
    static bool isFloat(const std::string& num);
    static bool isInteger(const std::string& num);
    static bool isChar(const std::string& ch);
    static std::string removeSpecialCharacter(std::string s);

    // New Symbol Table Methods
    static llvm::Value* getSymbolValue(const std::string& var_name);
    static void printLLVMSymbolTable();

    static uint64_t hexToDec(std::string& str);
    //static llvm::Type* getLLVMptrType(std::string var_type, llvm::LLVMContext& Context, std::string var_name);
    static llvm::Type * getLLVMType(std::string var_type, llvm::LLVMContext& context);

    // Data members
    static std::vector<std::string> definedTypes;
    static std::vector<char> separetors;
    static std::vector<char> quotes;
    static SymbolTable symbolTable;
};
