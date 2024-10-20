#include "sourceFileHandler.h"

void SourceFileHandler::handleFile()
{
    // Check if file opens
    if(!_sourceFile.is_open())
    {
        std::cerr << "[PREPROCESSOR ERROR]  Unable to open source file" << std::endl;
    }
    // Check for correct file type 
    
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

