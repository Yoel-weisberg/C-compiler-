#include <iostream>
#include <fstream>
#include "../Header/sourceFileHandler.h"
#include "../Header/Preprocess.h"
#include "../Header/CompilationErrors.h"
#include "../Header/Tokenizer.h"
#include "../Header/SyntaxAnalysis.h"	
#include "../Header/Parser.h"
#include "TopLevelParser.h"
#include "Helper.h"
#include "llvm/Support/Error.h"           // For llvm::Error and llvm::Expected
#include "llvm/Support/ErrorHandling.h"   // For llvm::ExitOnError

int main(int argc, char* argv[]) {

	try
	{
		InitializeNativeTarget();
		InitializeNativeTargetAsmPrinter();
		InitializeNativeTargetAsmParser();

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
		TopLevelParser parser = TopLevelParser(tokeniser.getTokens());
		
		Helper::TheJIT = Helper::ExitOnErr(llvm::orc::KaleidoscopeJIT::Create());

		Helper::InitializeModuleAndManagers();
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