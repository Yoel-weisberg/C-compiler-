#include "Preprocess.h"
#include <cctype>

void Preprocess::removeComments()
{
    
}

void Preprocess::handleMacroVariebles()
{
    std::fstream file (_fileName, std::ios::in | std::ios::out);
    if (!file.is_open()){
        std::cerr << "[PREPROCESSOR ERROR]  Unable to open file" << std::endl;
        return;
    }
    char ch;
    while (file.get(ch)) {
        int currentLine = 0;
        int currentChar = 0;
        if (ch == '#') {
            std::string macroName;
            while (file.get(ch) && ch!= ' ') {
                macroName += ch;
            }
            if (macroName == "define") {
                std::string macroValue;
                std::string macroKey;
                bool multyLine = false;
                bool gotEnd = false;
                // moving pointer to start of macro key 
                while (file.get(ch) && ch == ' ') {}
                
                // getting macro key
                while (file.get(ch) && ch!= ' ') {
                    macroKey += ch; 
                }

                // moving pointer to start of macro value
                while (file.get(ch) && ch==' '){}
                
                // getting macro value
                while (file.get(ch) && !gotEnd) {
                    macroValue += ch; 
                    // checking if file needs to continue
                    if (ch == '\\')
                    {
                        multyLine = true; 
                    }
                    if (ch == '\n')
                    {
                        multyLine ? gotEnd = false : gotEnd = true;
                        multyLine = false;
                        currentLine++;
                    }
                }

                // checking that the key is valid
                checkMacroKeyValidity(macroKey) ? throw SyntaxError("Macro key not valid", currentLine) : true;

                // checking that the value is valid
                checkMacroValueValidity(macroValue)? throw SyntaxError("Macro value not valid", currentLine) : true;

                // storing macro in the table
                _macroTable[macroKey] = macroValue;

            } else if (macroName == "undef") {
                // TODO  - remove define from macro table 
            } else if (macroName == "include") {
                // TODO - manage include 
            }
        } else {
            file.putback(ch);
        }
    }
    // replace the macros in the macro table 
    replaceMacro();
}

bool Preprocess::checkMacroKeyValidity(const std::string &macroKey)
{
    if (macroKey.empty()) {
        return false; // Empty strings are not valid identifiers
    }

    // Check the first character: it must be a letter or underscore
    if (!(std::isalpha(macroKey[0]) || macroKey[0] == '_')) {
        return false;
    }

    // Check the rest of the characters: they must be letters, digits, or underscores
    for (size_t i = 1; i < macroKey.length(); ++i) {
        if (!(std::isalnum(macroKey[i]) || macroKey[i] == '_')) {
            return false;
        }
    }

    return true;  
}

bool Preprocess::checkMacroValueValidity(const std::string &macroValue)
{
    // TODO: check if macro value is valid
}
