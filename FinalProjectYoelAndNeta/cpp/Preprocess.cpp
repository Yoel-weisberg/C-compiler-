#include "../Header/Preprocess.h"
#include <cctype>
#include <algorithm>
#include "../Header/SyntaxError.h"
#include <sstream>

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
    while (pos < _fileRawContent.size())
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
            updatedStream.push_back(_fileRawContent[pos++]);
        }
    }
    _fileRawContent = updatedStream;
}

std::string Preprocess::manageIncludes(std::string fileName) {
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        throw SyntaxError("File in include isnt good", 0);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string fileContents = buffer.str();
    file.close();
    return fileContents;
}

void Preprocess::handleMacroVariables() {
    size_t pos = 0;
    int currentLine = 0;
    std::string newStream;

    while (pos < _fileRawContent.size()) {
        char ch = _fileRawContent[pos++];
        if (ch == '#') {
            std::string macroName;

            // Extract the macro name
            while (pos < _fileRawContent.size() && (ch = _fileRawContent[pos++]) != ' ' && ch != '\n') {
                macroName += ch;
            }

            if (macroName == "define") {
                std::string macroKey, macroValue;
                bool multiLine = false, gotEnd = false;

                // Skip spaces before macro key
                while (pos < _fileRawContent.size() && (ch = _fileRawContent[pos]) == ' ') pos++;

                // Get macro key
                while (pos < _fileRawContent.size() && (ch = _fileRawContent[pos]) != ' ' && ch != '\n') {
                    macroKey += ch;
                    pos++;
                }

                // Skip spaces before macro value
                while (pos < _fileRawContent.size() && (ch = _fileRawContent[pos]) == ' ') pos++;

                // Get macro value
                while (pos < _fileRawContent.size() && !gotEnd) {
                    ch = _fileRawContent[pos++];

                    // Check for multi-line definition
                    if (ch == '\\') {
                        multiLine = true;
                        continue; // Skip the '\' character
                    }

                    if (ch == '\n') {
                        currentLine++;
                        if (!multiLine) gotEnd = true; // End if not a multi-line macro
                        multiLine = false; // Reset multi-line flag
                    }
                    else {
                        macroValue += ch;
                    }
                }

                // Check key and value validity
                if (!Helper::checkIdentifier(macroKey)) {
                    throw SyntaxError("Macro key not valid", currentLine);
                }
                if (!checkMacroValueValidity(macroValue)) {
                    throw SyntaxError("Macro value not valid", currentLine);
                }

                // Add to macro table
                _macroTable[macroKey] = macroValue;
            }
            else if (macroName == "include")
            {
                std::string file_name, file_content;
                // Skip spaces before file name
                while (pos < _fileRawContent.size() && (ch = _fileRawContent[pos]) == ' ') pos++;

                // Get file name
                while (pos < _fileRawContent.size() && (ch = _fileRawContent[pos++]) != ' ' && ch != '\n') {
                    file_name += ch;
                }
                file_content = manageIncludes(file_name);
                _fileRawContent.insert(pos, " " + file_content);
            }
        }
        else
        {
            newStream += ch;
        }
    }

    // Replace macros in the file content using the macro table
    _fileRawContent = newStream;
    _fileRawContent = replaceMacro();
}


std::string Preprocess::replaceMacro() {
    std::string codeStream;
    std::string currentBlock;
    bool isThereQuotes = false, singleQuote = false;

    for (size_t pos = 0; pos < _fileRawContent.size(); pos++) {
        char ch = _fileRawContent[pos];

        (ch == '"' && !singleQuote) ? isThereQuotes = !isThereQuotes : true;
        (ch == '\'' && !isThereQuotes) ? singleQuote = !singleQuote : true;

        if (((ch == ' ' || ch == '\n') && !isThereQuotes)) {
            if (currentBlock.empty()) continue;

            auto it = _macroTable.find(currentBlock);
            if (it != _macroTable.end()) {
                codeStream += getFinalValue(it->first) + ' ';
            } else {
                codeStream += currentBlock + ' ';
            }
            currentBlock.clear();
        }
        else if (pos == _fileRawContent.size() - 1)
        {
            currentBlock += ch;
            codeStream += currentBlock;
        }
        else {
            currentBlock += ch;
        }
    }
    return codeStream;
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
    if (_macroTable.find(macroValue) != _macroTable.end()) { return true; }
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