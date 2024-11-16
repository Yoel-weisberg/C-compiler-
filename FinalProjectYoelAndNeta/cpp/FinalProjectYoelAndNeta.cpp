 #include <iostream>
 #include <fstream>
 #include "../Header/sourceFileHandler.h"
 #include "../Header/Preprocess.h"
#include "../Header/SyntexError.h"

 int main(int argc, char *argv[]){
     try
     {
         std::cout << "----      Compiler for C :)       ----" << std::endl;

         SourceFileHandler sourceFile(argv, argc);

         // ----     Preprocessor                   ----
         Preprocess preprocessFile = Preprocess(sourceFile.getSrcFileContent());

         std::cout << "After preprocesser: " << preprocessFile.getFinalStream() << std::endl;

         // ----     Lexical Analyzer               ----
         // ----     Syntax Analyzer                ----
         // ----     Semantic Analyzer              ----
         // ----     To LLVM IR                     ----
         // ----     Rest of compilation process    ----
     }
     catch (const SyntaxError& err)
     {
         std::cout << err.what();
     }
    return 0;
 }