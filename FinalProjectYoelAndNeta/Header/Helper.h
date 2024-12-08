#pragma once

#include <iostream>
#include <vector>
#include "Constents.h"
#include <regex>
#include "SymbalTable.h"
#include <map>
#include "Constents.h"
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

#include <memory>

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
    static std::map<std::string, Value*> NamedValues;
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

    // Utility methods
    static bool checkIdentifier(const std::string& id);
    static bool isFloat(const std::string& num);

    // Data members
    static std::vector<std::string> definedTypes;
    static std::map<std::string, Tokens_type> literalToType;
    static SymbolTable symbolTable;
};
