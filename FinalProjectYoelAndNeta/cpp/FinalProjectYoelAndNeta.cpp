#include <iostream>
#include <fstream>
#include "../Header/sourceFileHandler.h"
#include "../Header/Preprocess.h"
#include "../Header/SyntaxError.h"
#include "../Header/Tokenizer.h"
#include "../Header/SyntaxAnalysis.h"	
#include "../Header/Parser.h"
#include "Helper.h"

int main(int argc, char* argv[]) {

	try
	{
		Helper::initializeModule();

		std::cout << "----      Compiler for C :)       ----" << std::endl;

		SourceFileHandler sourceFile(argv, argc);

		// ----     Preprocessor                   ----
		Preprocess preprocessFile = Preprocess(sourceFile.getSrcFileContent());

		std::cout << "After preprocesser: " << preprocessFile.getFinalStream() << std::endl;

		// ----     Lexical Analyzer               ----
		Tokeniser tokeniser = Tokeniser(preprocessFile.getFinalStream());
		std::cout << tokeniser << std::endl;
		std::cout << "After tokeniser " << std::endl;

		// ----     Syntax Analyzer                ----
		SyntaxAnalysis syntaxAnalysis = SyntaxAnalysis(tokeniser.getTokens());
		std::cout << "Syntax analysed" << std::endl;

		// ----     Parser                         ----
		Parser parser = Parser(tokeniser.getTokens());
		auto head = parser.getAst();

		std::cout << "Ast created" << std::endl;

		llvm::Value* generatedValue = head->codegen();
		generatedValue->print(llvm::errs());  // Prints the LLVM IR for the value


		// ----     To LLVM IR                     ----
		// ----     Rest of compilation process    ----
	}
	catch (const SyntaxError& err)
	{
		std::cout << err.what();
	}
	return 0;
}