#pragma once

#include <iostream>
#include <fstream>
#include <map>
//#include "CompilationErrors.h"
#include "../Header/Helper.h"
#include "../Header/CompilationErrors.h"

#define INCLUDE_IDENTIFIER "#include"

class Preprocess{
public:
    Preprocess(const std::string& fileContent);
    std::string getFinalStream() const { return _fileRawContent; }
private:
    // Handle Comments
    void removeComments ();

    // Handle Includes
    std::string manageIncludes(std::string fileName);

    // Handle Macros
    void handleMacroVariables();
    std::string replaceMacro();
    // void handleFunctinMacro(); - sprint2
    bool checkMacroValueValidity (const std::string& macroValue);
    bool isNumber(const std::string& number);
    std::string getFinalValue(std::string key);

    std::string _fileRawContent;
    std::map <std::string, std::string> _macroTable;
};