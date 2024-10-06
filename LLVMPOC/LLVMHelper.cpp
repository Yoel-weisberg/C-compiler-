#include "LLVMHelper.h"
#include <iostream>


llvm::Function* LLVMHelper::createFunction()
{
    // Create a function type: double main()
    llvm::FunctionType* funcType = llvm::FunctionType::get(llvm::Type::getDoubleTy(*INode::TheContext), false);

    // Create a function in the module
    llvm::Function* function = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "main", INode::TheModule.get());

    // Create a new basic block to start insertion into
    llvm::BasicBlock* BB = llvm::BasicBlock::Create(*INode::TheContext, "entry", function);
    INode::Builder->SetInsertPoint(BB);

    // Add return statement to the function (for example, return 0.0)
    INode::Builder->CreateRet(llvm::ConstantFP::get(llvm::Type::getDoubleTy(*INode::TheContext), 0.0));

    return function;
}

void LLVMHelper::generateIR(INode* astRoot)
{
    // Assuming TheModule is already initialized and populated
    llvm::Module* TheModule = INode::TheModule.get();
    llvm::LLVMContext& context = *INode::TheContext;

    // Create the main function and set it as the entry point
    createFunction();

    // Generate code for the AST node
    astRoot->codegen();
    
    TheModule->print(errs(), nullptr);

    // Verify the module
    llvm::verifyModule(*TheModule, &llvm::errs());

    // Save the module to a file
    std::error_code EC;
    llvm::raw_fd_ostream dest("output.ll", EC);
    if (EC) {
        std::cerr << "Error opening output file: " << EC.message() << std::endl;
        return;
    }
    TheModule->print(dest, nullptr);
}
