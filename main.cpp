 #include <iostream>
 #include <fstream>
 #include "sourceFileHandler.h"

 int main(int argc, char *argv[]){
    std::cout << "----      Compiler for C :)       ----" << std::endl;
    SourceFileHandler sourceFile = SourceFileHandler(argv);
    sourceFile.handleFile();
    // ----     Preprocessor                   ----
    // ----     Lexical Analyzer               ----
    // ----     Syntax Analyzer                ----
    // ----     Semantic Analyzer              ----
    // ----     To LLVM IR                     ----
    // ----     Rest of compilation process    ----
    return 0;
 }