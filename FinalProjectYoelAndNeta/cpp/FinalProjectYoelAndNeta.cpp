#include <iostream>
#include <fstream>
#include "../Header/sourceFileHandler.h"
#include "../Header/Preprocess.h"
#include "../Header/SyntexError.h"
#include "../Header/Tokenizer.h"
#include "../Header/SyntexAnalysis.h"	
#include "../Header/Parser.h"

int main(int argc, char* argv[]) {

	try
	{
		std::cout << "----      Compiler for C :)       ----" << std::endl;

		SourceFileHandler sourceFile(argv, argc);

		// ----     Preprocessor                   ----
		Preprocess preprocessFile = Preprocess(sourceFile.getSrcFileContent());

		std::cout << "After preprocesser: " << preprocessFile.getFinalStream() << std::endl;

		// ----     Lexical Analyzer               ----
		Tokeniser tokeniser = Tokeniser(preprocessFile.getFinalStream());
		std::cout << "After tokeniser " << std::endl;

		// ----     Syntax Analyzer                ----
		SyntexAnalysis syntaxAnalysis = SyntexAnalysis(tokeniser.getTokens());
		std::cout << "Syntax analysed" << std::endl;

		// ----     Parser                         ----
		Parser parser = Parser(tokeniser.getTokens());
		auto ast = parser.parse();

		std::cout << "Ast created" << std::endl;
		// ----     To LLVM IR                     ----
		// ----     Rest of compilation process    ----
	}
	catch (const SyntaxError& err)
	{
		std::cout << err.what();
	}
	return 0;
}