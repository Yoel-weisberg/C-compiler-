#pragma once
#include <vector>
#include <string>
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
#include <map>

#define LEFT_CHILD 0
#define RIGHT_CHILD 1

using namespace llvm;

class INode {
public:
    static std::unique_ptr<LLVMContext> TheContext;
    static std::unique_ptr<IRBuilder<>> Builder;
    static std::unique_ptr<Module> TheModule;
    static std::map<std::string, Value*> NamedValues;

    static void initializeLLVM() {
        if (!TheContext) {
            TheContext = std::make_unique<LLVMContext>();
            Builder = std::make_unique<IRBuilder<>>(*TheContext);
            TheModule = std::make_unique<Module>("my cool jit", *TheContext);
        }
    }

    Value* LogErrorV(const char* Str) {
        LogError(Str);
        return nullptr;
    }

    void LogError(const char* Str) {
        fprintf(stderr, "Error: %s\n", Str);
    }

    virtual ~INode() = default;

    virtual llvm::Value* codegen() = 0;
};
