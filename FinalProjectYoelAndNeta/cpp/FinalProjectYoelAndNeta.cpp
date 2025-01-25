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
#include <llvm/Support/TargetSelect.h>


int main(int argc, char* argv[]) {

	try
	{

		std::cout << "----      Compiler for C :)       ----" << std::endl;


		InitializeAllTargetInfos();
		InitializeAllTargets();
		InitializeAllTargetMCs();
		InitializeAllAsmParsers();
		InitializeAllAsmPrinters();
		Helper::InitializeModuleAndManagers();

		SourceFileHandler sourceFile(argv, argc);

		// ----     Preprocessor                   ----
		Preprocess preprocessFile = Preprocess(sourceFile.getSrcFileContent());

		//std::cout << "After preprocesser: " << preprocessFile.getFinalStream() << std::endl;

		// ----     Lexical Analyzer               ----
		Tokeniser tokeniser = Tokeniser(preprocessFile.getFinalStream());
		//std::cout << tokeniser << std::endl;
		//std::cout << "After tokeniser " << std::endl;

		// ----     Syntax Analyzer                ----
		SyntaxAnalysis syntaxAnalysis = SyntaxAnalysis(tokeniser.getTokens());
		//std::cout << "Syntax analysed" << std::endl;

		// ----     Parser                         ----
		TopLevelParser parser = TopLevelParser(tokeniser.getTokens());
		
		Helper::TheJIT = Helper::ExitOnErr(llvm::orc::KaleidoscopeJIT::Create());

		parser.mainLoop();

		Helper::builfObjectFile();

	}
	catch (const SyntaxError& err)
	{
		std::cout << err.what();
	}
	return 0;
}