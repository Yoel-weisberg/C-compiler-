#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "Tokenizer.h"
#include "SyntexAnalysis.h"
#include "Optimiser.h"
#include "ParseToAst.h"
#include "Asm.h"
#include "INode.h"
#include "LLVMHelper.h"

int main(int argc, char* argv[])
{
    try
    {
        INode::initializeLLVM();

        if (argc != 2)  // argc should be 2, since argv[0] is the program name and argv[1] is the file path
        {
            throw std::invalid_argument("You need to specify the file you want to open");
        }

        std::ifstream file(argv[1]);  // Open the file
        if (!file.is_open())
        {
            throw std::runtime_error("Could not open the specified file");
        }

        std::stringstream buffer;
        buffer << file.rdbuf();  // Read file content into stringstream
        std::string file_content = buffer.str();  // Store content into a string

        Tokeniser* tokeniser = new Tokeniser(file_content);

        SyntexAnalysis* analysis = new SyntexAnalysis(tokeniser->getTokens());

        Optimiser* optimiser = new Optimiser(tokeniser->getTokens());

        Parser parser(optimiser->getOptimised());

        INode rootAST = parser.parseExpression();

        if (astRoot) {
            generateIR(astRoot);  // Generate LLVM IR from the AST
        }
        else {
            std::cerr << "Parsing failed." << std::endl;
        }

        file.close();  // Close the file
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}