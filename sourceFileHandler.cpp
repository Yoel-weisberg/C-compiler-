#include "sourceFileHandler.h"

void SourceFileHandler::handleFile()
{
    // Check if file opens
    if(!_sourceFile.is_open())
    {
        std::cerr << "[PREPROCESSOR ERROR]  Unable to open source file" << std::endl;
        return;
    }
    // Check for correct file type 
    if(isFileTypeCorrect())
    {   
        // Read file content and append it into '_fileContent'
        while (getline(_sourceFile, _srcFileContent))
        {
            std::cout << _srcFileContent << std::endl;
        }
    }
}

std::string SourceFileHandler::getSrcFileContent()
{
    return _srcFileContent;
}

bool SourceFileHandler::isFileTypeCorrect()
{
    std::filesystem::path path(_fileName);
    if(path.extension() != TARGET_FILE_TYPE)
    {
        std::cerr << "[PREPROCESSOR ERROR]  File type: '" << path.extension() << "' incorect!, only '" << TARGET_FILE_TYPE << "'!" << std::endl;
        return true;
    }
    return false;
}

