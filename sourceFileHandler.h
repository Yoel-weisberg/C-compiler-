#include <iostream>
#include <fstream>
#include <filesystem>

#define TARGET_FILE_TYPE ".c"

class SourceFileHandler
{
public:

    SourceFileHandler(char *argv[]): _sourceFile(argv[1]), _fileName(argv[2]){};
    void handleFile();
    std::string getSrcFileContent();

private:

    std::ifstream _sourceFile;
    std::string _fileName;
    std::string _srcFileContent;
    bool isFileTypeCorrect();
    
};
