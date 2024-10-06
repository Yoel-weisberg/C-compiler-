#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "Tokenizer.h"
#include "SyntexAnalysis.h"
#include "Optimiser.h"
#include "ParseToAst.h"
#include "INode.h"
#include "LLVMHelper.h"


int main(int argc, char* argv[])
{
    try
    {
        INode::initializeLLVM();

        if (argc != 2)
        {
            throw std::invalid_argument("You need to specify the file you want to open");
        }

        std::ifstream file(argv[1]);
        if (!file.is_open())
        {
            throw std::runtime_error("Could not open the specified file");
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string file_content = buffer.str();

        auto tokeniser = std::make_unique<Tokeniser>(file_content);
        auto analysis = std::make_unique<SyntexAnalysis>(tokeniser->getTokens());
        auto optimiser = std::make_unique<Optimiser>(tokeniser->getTokens());

        Parser parser(optimiser->getOptimised());
        INode* rootAST = parser.parseExpression();

        if (rootAST)
        {
            LLVMHelper::generateIR(rootAST);
        }
        else
        {
            std::cerr << "Parsing failed." << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
