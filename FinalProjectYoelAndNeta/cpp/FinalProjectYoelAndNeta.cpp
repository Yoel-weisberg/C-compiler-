#include <iostream>
#include <fstream>
#include "../Header/sourceFileHandler.h"
#include "../Header/Preprocess.h"
#include "../Header/SyntexError.h"
#include "../Header/Tokenizer.h"
#include "../Header/SyntexAnalysis.h"	
#include "../Header/Parser.h"
#include "TopLevelParser.h"
#include "Helper.h"

int main(int argc, char* argv[]) {

	try
	{
		InitializeNativeTarget();
		InitializeNativeTargetAsmPrinter();
		InitializeNativeTargetAsmParser();
		Helper::InitializeModuleAndManagers();

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
		TopLevelParser parser = TopLevelParser(tokeniser.getTokens());
		parser.mainLoop();

	
		// ---- Emit LLVM IR ----
		std::error_code EC;
		llvm::raw_fd_ostream dest("output.ll", EC, llvm::sys::fs::OF_None);
		if (EC) {
			std::cerr << "Could not open file: " << EC.message() << std::endl;
			return 1;
		}
		Helper::TheModule->print(dest, nullptr);
		dest.flush();
		std::cout << "LLVM IR emitted to output.ll" << std::endl;

	}
	catch (const SyntaxError& err)
	{
		std::cout << err.what();
	}
	return 0;
}