//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <stdexcept>
//#include "Tokenizer.h"
//#include "SyntexAnalysis.h"
//#include "Optimiser.h"
//#include "ParseToAst.h"
//#include "Asm.h"
//
//
//int main(int argc, char* argv[])
//{
//    try
//    {
//        if (argc != 2)  // argc should be 2, since argv[0] is the program name and argv[1] is the file path
//        {
//            throw std::invalid_argument("You need to specify the file you want to open");
//        }
//
//        std::ifstream file(argv[1]);  // Open the file
//        if (!file.is_open())
//        {
//            throw std::runtime_error("Could not open the specified file");
//        }
//
//        std::stringstream buffer;
//        buffer << file.rdbuf();  // Read file content into stringstream
//        std::string file_content = buffer.str();  // Store content into a string
//
//        Tokeniser* tokeniser = new Tokeniser(file_content);
//
//        SyntexAnalysis* analysis = new SyntexAnalysis(tokeniser->getTokens());
//
//        Optimiser* optimiser = new Optimiser(tokeniser->getTokens());
//
//        Parser parser(optimiser->getOptimised());
//
//        ConvortASTtoASM* converter = new ConvortASTtoASM(parser.parseExpression());
//
//        file.close();  // Close the file
//    }
//    catch (const std::exception& e)
//    {
//        std::cerr << "Error: " << e.what() << std::endl;
//        return 1;
//    }
//
//    return 0;
//}
//
//
//
//
//
//

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

int main() {
    // Create a new LLVM context and a module to hold the function
    llvm::LLVMContext context;
    llvm::Module module("my_module", context);

    // Create an IR builder to help construct the LLVM IR
    llvm::IRBuilder<> builder(context);

    // Define the function type (int32 (int32, int32)) - a function taking two int32 arguments and returning an int32
    llvm::FunctionType* funcType = llvm::FunctionType::get(builder.getInt32Ty(), { builder.getInt32Ty(), builder.getInt32Ty() }, false);

    // Create the function and insert it into the module
    llvm::Function* addFunction = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "add", &module);

    // Create a basic block and set the insertion point to this block
    llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(context, "entry", addFunction);
    builder.SetInsertPoint(entryBlock);

    // Get function arguments
    llvm::Function::arg_iterator args = addFunction->arg_begin();
    llvm::Value* x = args++;
    llvm::Value* y = args++;

    // Generate code for x + y
    llvm::Value* sum = builder.CreateAdd(x, y, "sum");

    // Return the result
    builder.CreateRet(sum);

    // Output the generated LLVM IR to the console
    module.print(llvm::outs(), nullptr);

    return 0;
}
