#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"

int main() {
    llvm::LLVMContext Context;
    llvm::Module* Module = new llvm::Module("MyModule", Context);
    llvm::IRBuilder<> Builder(Context);

    // Create a function signature: int add(int a, int b)
    llvm::FunctionType* funcType = llvm::FunctionType::get(Builder.getInt32Ty(),
        { Builder.getInt32Ty(), Builder.getInt32Ty() }, false);
    llvm::Function* addFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage,
        "add", Module);

    // Create a basic block and set the insertion point
    llvm::BasicBlock* BB = llvm::BasicBlock::Create(Context, "entry", addFunc);
    Builder.SetInsertPoint(BB);

    // Get function arguments
    llvm::Function::arg_iterator args = addFunc->arg_begin();
    llvm::Value* a = args++;
    llvm::Value* b = args++;

    // Add the two arguments
    llvm::Value* sum = Builder.CreateAdd(a, b, "sum");

    // Return the result
    Builder.CreateRet(sum);

    // Verify the function
    llvm::verifyFunction(*addFunc);

    // Output the LLVM IR to stdout
    Module->print(llvm::outs(), nullptr);

    delete Module;
    return 0;
}
