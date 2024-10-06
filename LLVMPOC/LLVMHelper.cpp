#include "LLVMHelper.h"

llvm::Function* LLVMHelper::createFunction()
{
    // Create a function type: double foo()
    llvm::FunctionType* funcType = llvm::FunctionType::get(llvm::Type::getDoubleTy(*INode::TheContext), false);

    // Create a function in the module
    llvm::Function* function = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "foo", INode::TheModule.get());

    // Create a new basic block to start insertion into
    llvm::BasicBlock* BB = llvm::BasicBlock::Create(*INode::TheContext, "entry", function);
    INode::Builder->SetInsertPoint(BB);

    return function;
}

void LLVMHelper::generateIR(INode* astRoot)
{
    // Create a function
    llvm::Function* function = createFunction();

    // Generate code for the AST and set the result as the return value
    llvm::Value* result = astRoot->codegen();
    if (result) {
        INode::Builder->CreateRet(result);
    }

    // Validate the generated code, checking for consistency
    llvm::verifyFunction(*function);

    // Optionally, you can print the generated code
    INode::TheModule->print(llvm::outs(), nullptr);
}
