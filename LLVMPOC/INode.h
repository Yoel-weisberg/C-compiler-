#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>
#include "Constents.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#define LEFT_CHILD 0
#define RIGHT_CHILD 1

using namespace llvm;

class INode {
public:
    static std::unique_ptr<LLVMContext> TheContext;
    static std::unique_ptr<IRBuilder<>> Builder;
    static std::unique_ptr<Module> TheModule;
    static std::map<std::string, Value*> NamedValues;

    static void initializeLLVM(); // Declaration only
    static Value* LogErrorV(const char* Str); // Declaration only
    static void LogError(const char* Str); // Declaration only

    virtual ~INode() = default; // Virtual destructor
    virtual llvm::Value* codegen() = 0; // Pure virtual function
};
