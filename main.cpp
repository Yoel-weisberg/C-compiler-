 #include <iostream>
 #include <fstream>
 #include "sourceFileHandler.h"
 #include "Preprocess.h"

 int main(int argc, char *argv[]){
   std::cout << "----      Compiler for C :)       ----" << std::endl;
   
   SourceFileHandler sourceFile(argv); 
   sourceFile.handleFile();
   
    // ----     Preprocessor                   ----
   Preprocess preprocessFile = Preprocess(sourceFile.getSrcFileContent());

    // ----     Lexical Analyzer               ----
    // ----     Syntax Analyzer                ----
    // ----     Semantic Analyzer              ----
    // ----     To LLVM IR                     ----
    // ----     Rest of compilation process    ----
    return 0;
 }