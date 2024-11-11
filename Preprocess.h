#include <iostream>
#include <fstream>
#include <map>
#include "SyntexError.h"

#define INCLUDE_IDENTIFIER "#include"

class Preprocess{
public:
    Preprocess(const std::string& fileContent) : _fileRawContent(fileContent) {std::cout << "Created a 'Preprocess' instance\n" << std::endl;} // Print only for testing

private:
    // Handle Comments
    void removeComments ();

    // Handle Includes
    void manageIncludes();

    // Handle Macros
    void handleMacroVariables();
    std::string replaceMacro();
    // void handleFunctinMacro(); - sprint2
    bool checkMacroKeyValidity (const std::string& macroKey);
    bool checkMacroValueValidity (const std::string& macroValue);
    bool isNumber(const std::string& number);
    std::string getFinalValue(std::string key);

    std::string _fileRawContent;
    std::map <std::string, std::string> _macroTable;
};