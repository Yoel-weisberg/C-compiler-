#include "../Header/Preprocess.h"
#include <cctype>
#include <algorithm>
#include "../Header/SyntexError.h"

Preprocess::Preprocess(const std::string &fileContent) : _fileRawContent(fileContent)
{
    // first of all remove all comments
    removeComments();

    // then manage macros
    handleMacroVariables();
}

void Preprocess::removeComments()
{
    size_t pos = 0, currentPlaceInUpdated = 0;
    std::string updatedStream;
    while (pos < _fileRawContent.size() - 1)
    {
        // single line comment
        if (_fileRawContent[pos] == '/' && _fileRawContent[pos + 1] == '/')
        {
            // Skip until end of line
            while (pos < _fileRawContent.size() && _fileRawContent[pos] != '\n') pos++;
        }
        // multi-line comment
        else if (_fileRawContent[pos] == '/' && _fileRawContent[pos + 1] == '*') {
            while (pos < _fileRawContent.size() - 1 &&
                   (_fileRawContent[pos]!= '*' || _fileRawContent[pos + 1]!= '/'))
            {
                pos++;
            }
            // moving to the end of the comment
            pos += 2;
        }
        else
        {
            updatedStream[currentPlaceInUpdated++] = _fileRawContent[pos];
        }
    }
}

void Preprocess::manageIncludes() {
    // Implement include management logic.
}

void Preprocess::handleMacroVariables() {
    size_t pos = 0;
    int currentLine = 0;
    while (pos < _fileRawContent.size()) {
        char ch = _fileRawContent[pos++];
        if (ch == '#') {
            std::string macroName;
            while (pos < _fileRawContent.size() && (ch = _fileRawContent[pos++]) != ' ') {
                macroName += ch;
            }
            if (macroName == "define") {
                std::string macroKey, macroValue;
                bool multyLine = false, gotEnd = false;

                // Skip spaces before macro key
                while (pos < _fileRawContent.size() && (ch = _fileRawContent[pos]) == ' ') pos++;

                // Get macro key
                while (pos < _fileRawContent.size() && (ch = _fileRawContent[pos]) != ' ') {
                    macroKey += ch;
                    pos++;
                }

                // Skip spaces before macro value
                while (pos < _fileRawContent.size() && (ch = _fileRawContent[pos]) == ' ') pos++;

                // Get macro value
                while (pos < _fileRawContent.size() && !gotEnd) {
                    ch = _fileRawContent[pos++];
                    macroValue += ch;
                    if (ch == '\\') {
                        multyLine = true;
                    }
                    if (ch == '\n') {
                        multyLine ? gotEnd = false : gotEnd = true;
                        multyLine = false;
                        currentLine++;
                    }
                }

                // Check key and value validity
                if (!checkMacroKeyValidity(macroKey)) throw SyntaxError("Macro key not valid", currentLine);
                if (!checkMacroValueValidity(macroValue)) throw SyntaxError("Macro value not valid", currentLine);

                _macroTable[macroKey] = macroValue;
        }
    }

    // Replace macros in the macro table
    _fileRawContent = replaceMacro();
}
}

std::string Preprocess::replaceMacro() {
    std::string codeStream;
    std::string currentBlock;
    bool isThereQuotes = false, singleQuote = false;

    for (size_t pos = 0; pos < _fileRawContent.size(); ++pos) {
        char ch = _fileRawContent[pos];

        (ch == '"' && !singleQuote) ? isThereQuotes = !isThereQuotes : true;
        (ch == '\'' && !isThereQuotes) ? singleQuote = !singleQuote : true;

        if ((ch == ' ' || ch == '\n') && !isThereQuotes) {
            if (currentBlock.empty()) continue;

            auto it = _macroTable.find(currentBlock);
            if (it != _macroTable.end()) {
                codeStream += getFinalValue(it->first);
            } else {
                codeStream += currentBlock;
            }
            currentBlock.clear();
        } else {
            currentBlock += ch;
        }
    }
    return codeStream;
}

// The rest of your functions remain the same
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