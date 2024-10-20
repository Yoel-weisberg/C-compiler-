#include <iostream>
#include <fstream>
#include <filesystem>

#define TARGET_FILE_TYPE ".c"

class SourceFileHandler
{
public:
    SourceFileHandler(char *argv[]): _sourceFile(argv[0]), _fileName(argv[0]){};
    void handleFile();

private:
    std::ifstream _sourceFile;
    std::string _fileName;
    bool isFileTypeCorrect();
};
