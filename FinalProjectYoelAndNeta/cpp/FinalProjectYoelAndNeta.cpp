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
    try {
        // Initialize LLVM
        InitializeAllTargetInfos();
        InitializeAllTargets();
        InitializeAllTargetMCs();
        InitializeAllAsmParsers();
        InitializeAllAsmPrinters();

        Helper::InitializeModuleAndManagers();

        SourceFileHandler sourceFile(argv, argc);
        Preprocess preprocessFile = Preprocess(sourceFile.getSrcFileContent());
        Tokeniser tokeniser = Tokeniser(preprocessFile.getFinalStream());
        SyntaxAnalysis syntaxAnalysis = SyntaxAnalysis(tokeniser.getTokens());

        // Create parser and JIT
        TopLevelParser parser = TopLevelParser(tokeniser.getTokens());
        Helper::TheJIT = Helper::ExitOnErr(llvm::orc::KaleidoscopeJIT::Create());

        parser.mainLoop();

        Helper::builfObjectFile();
    }
    catch (const SyntaxError& err) {
        std::cout << err.what();
    }
    return 0;
}