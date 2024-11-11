#include <iostream>
#include <fstream>
#include <map>
#include "SyntexError.h"

class Preprocess{
public:
    Preprocess(const std::string& fileName) : _fileName(fileName) {std::cout << "Created a 'Preprocess' instance\n" << std::endl;} // print only for testing

private:
    std::string _fileName;
    void removeComments ();
    void manageIncludes();
    void handleMacroVariebles();
    void replaceMacro();
    // void handleFunctinMacro(); - sprint2
    bool checkMacroKeyValidity (const std::string& macroKey);
    bool checkMacroValueValidity (const std::string& macroValue);
    bool isNumber(const std::string& number);
    std::map <std::string, std::string> _macroTable;
};