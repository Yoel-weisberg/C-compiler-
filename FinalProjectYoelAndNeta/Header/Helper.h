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

#include <map>
#include "Constants.h"
#include "ExprAST.h"

#include "KaleidoscopeJIT.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/IR/LegacyPassManager.h"
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/Program.h>
#include <llvm/Support/WithColor.h>


#include "Constants.h"
#include "SymbolTable.h"

class PrototypeAST;


using namespace llvm;

class Helper
{
public:
    // llvm related
    static void InitializeModuleAndManagers();
    static std::unique_ptr<LLVMContext> TheContext;
    static std::unique_ptr<Module> TheModule;
    static std::unique_ptr<IRBuilder<>> Builder;
    static std::map<std::string, AllocaInst*> NamedValues;
    static std::unique_ptr<llvm::orc::KaleidoscopeJIT> TheJIT;
    static std::unique_ptr<FunctionPassManager> TheFPM;
    static std::unique_ptr<LoopAnalysisManager> TheLAM;
    static std::unique_ptr<FunctionAnalysisManager> TheFAM;
    static std::unique_ptr<CGSCCAnalysisManager> TheCGAM;
    static std::unique_ptr<ModuleAnalysisManager> TheMAM;
    static std::unique_ptr<PassInstrumentationCallbacks> ThePIC;
    static std::unique_ptr<StandardInstrumentations> TheSI;
    static std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;
    static ExitOnError ExitOnErr;

    // Accessor methods to get the objects
    static llvm::LLVMContext& getContext() { return *TheContext; }
    static llvm::IRBuilder<>& getBuilder() { return *Builder; }
    static llvm::Module& getModule() { return *TheModule; }
    static void createAnonymousFunction();
    static Function* getFunction(std::string Name);
    static llvm::AllocaInst* allocForNewSymbol(std::string var_name, std::string var_type, const int size, const std::string& pTT);
    static bool addSymbol(std::string var_name, std::string var_type, const std::string& pTT = EMPTY_STR, const int size = 1);
    

    // Utility methods
    static bool checkIdentifier(const std::string& id);
    static bool isFloat(const std::string& num);
    static bool isInteger(const std::string& num);
    static bool isChar(const std::string& ch);
    static std::string removeSpecialCharacter(std::string s);
    static Type* getTypeFromString(const std::string type);

    // New Symbol Table Methods
    static llvm::Value* getSymbolValue(const std::string& var_name);
    static void printLLVMSymbolTable();

    static uint64_t hexToDec(std::string& str);
    //static llvm::Type* getLLVMptrType(std::string var_type, llvm::LLVMContext& Context, std::string var_name);
    static llvm::Type * getLLVMType(std::string var_type, llvm::LLVMContext& context);

    // Data members
    static std::vector<std::string> definedTypes;
    static std::map<std::string, Tokens_type> literalToType;
    static SymbolTable symbolTable;

    // memory related 
    static llvm::Function* MallocFunc;
    static void defineMalloc();

    // user input
    static llvm::Function* ScanfFunc;
    static void defineScanf();

    // compiling files
    static void builfObjectFile();
};
