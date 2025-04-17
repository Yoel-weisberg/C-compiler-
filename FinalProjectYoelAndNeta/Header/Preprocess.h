#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include "../Header/Helper.h"
#include "../Header/CompilationErrors.h"

#define INCLUDE_IDENTIFIER "#include"

struct macroTableEntry
{
    std::string key;
    std::string value;
    bool isFunction;
    std::vector<std::string> args;
};

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
    std::string handleFunctionReplacement(macroTableEntry entry);
    // void handleFunctinMacro(); - sprint2
    bool checkMacroValueValidity (std::string& macroValue);
    bool isNumber(const std::string& number);
    std::string getFinalValue(std::string key);

    std::string _fileRawContent;
    std::vector<macroTableEntry> _macroTable;

    macroTableEntry* getFromVector(std::string key);

    int _pos;
};