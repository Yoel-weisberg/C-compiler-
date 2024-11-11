#include "Preprocess.h"
#include <cctype>
#include <algorithm>

void Preprocess::removeComments()
{
    
}

void Preprocess::handleMacroVariables()
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

std::string Preprocess::replaceMacro()
{
    std::fstream file (_fileName, std::ios::in | std::ios::out);
    if (!file.is_open()){
        throw SyntaxError("Invalid file name", 0);
    }
    char ch;
    std::string codeStream, currentBlock;
    bool isThereQuetes = false, singleQuete = false;
    while (file.get(ch))
    {
        // were not appling macros inside strings 
        (ch == '"' && !singleQuete)  ? isThereQuetes = !isThereQuetes : true;
        (ch == '\'' && !isThereQuetes) ? singleQuete = !singleQuete : true;

        if ((ch == ' ' || ch == '\n') && !isThereQuetes)
        {
            if (currentBlock.empty()) {
                continue;
            }
            auto it = _macroTable.find(currentBlock);
            if (it!= _macroTable.end()) {
                codeStream += getFinalValue(it->first);
            } else {
                codeStream += currentBlock;
            }
            currentBlock.clear();
        }

    }
    
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
    if (macroValue.empty()) { return false; }

    // if the macro is a string
    if (macroValue[0] == '"')
    {
        bool escapeChar = false;
        // we only need to check that it ends with a quote
        if (macroValue[macroValue.length() - 1] != '"') {return false;}
        // checking that it is a vlid string - it dosent contain quotes
        for (int i = 1; i < macroValue.length() - 1; i++)
        {
            if (macroValue[i] == '\\')
            {
                escapeChar = true;
            }
            else if (macroValue[i] == '"')
            {
                if (!escapeChar)
                {
                    return false;
                }
                escapeChar = false;
            }
            else
            {
                escapeChar = false;
            }
        }
    }
    if (isNumber(macroValue))
    {
        return true;
    }
    if (macroValue[0] == '\'')
    {
        if (macroValue[macroValue.length() - 1] != '\'') {return false;}
        if (macroValue.length() == 1 || (macroValue[1] == '/' && macroValue.length() == 2))
        {
            return true;
        }
        return false;
    }
    // a macro can hold a different macro
    if (checkMacroKeyValidity(macroValue)) { return true;}
    return false;
}

bool Preprocess::isNumber(const std::string &number)
{
    return !number.empty() && std::all_of(number.begin(), number.end(), ::isdigit);
}

std::string Preprocess::getFinalValue(std::string key)
{
    if (_macroTable.find(_macroTable.find(key)->second) == _macroTable.end())
    {
        return _macroTable.find(key)->second;
    }
    else
    {
        return getFinalValue(_macroTable.find(key)->second);
    }
}
