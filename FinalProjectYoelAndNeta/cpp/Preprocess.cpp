#include "../Header/Preprocess.h"
#include <cctype>
#include <algorithm>
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
            while (pos < _fileRawContent.size() && _fileRawContent[pos] != NEW_LINE_CHAR) pos++;
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
            while (pos < _fileRawContent.size() && (ch = _fileRawContent[pos++]) != ' ' && ch != NEW_LINE_CHAR) {
                macroName += ch;
            }

            if (macroName == "define") {
                std::string macroKey, macroValue;
                bool multiLine = false, gotEnd = false;
                macroTableEntry entry;
                // Skip spaces before macro key
                while (pos < _fileRawContent.size() && (ch = _fileRawContent[pos]) == ' ') pos++;

                // Get macro key
                while (pos < _fileRawContent.size() && (ch = _fileRawContent[pos]) != ' ' && ch != NEW_LINE_CHAR && ch != '(') {
                    macroKey += ch;
                    pos++;
                }
                
                // if its a function like macro
                if (ch == '(')
                {
                    pos++;
                    std::string currentArg;
                    std::vector<std::string> Args;
                    std::string functionBody;
                    // getting args:
                    while (ch != ')'  && pos < _fileRawContent.size())
                    {
                        ch = _fileRawContent[pos++];
                        if (ch == COMMA_LITERAL || ch == ')')
                        {
                            if (!currentArg.empty())
                            {
                                if (!Helper::checkIdentifier(currentArg)) throw SyntaxError("Argument undefined in functioon like macro", pos);
                                Args.push_back(currentArg);
                                currentArg.clear();
                            }
                        }
                        else if (ch != ' ')
                        {
                            currentArg += ch;
                        }
                    }
                    if (ch != ')') throw SyntaxError("Excepted a )", pos);

                    // reding the function until end line
                    while (ch != NEW_LINE_CHAR && pos < _fileRawContent.size())
                    {
                        ch = _fileRawContent[pos++];
                        functionBody += ch;
                    }

                    entry = { macroKey, functionBody, true, Args };
                }

                else
                {
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

                        if (ch == NEW_LINE_CHAR) {
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

                    entry = { macroKey, macroValue, false };
                }
                
                _macroTable.push_back(entry);
            }
            else if (macroName == "include")
            {
                std::string file_name, file_content;
                // Skip spaces before file name
                while (pos < _fileRawContent.size() && (ch = _fileRawContent[pos]) == ' ') pos++;

                // Get file name
                while (pos < _fileRawContent.size() && (ch = _fileRawContent[pos++]) != ' ' && ch != NEW_LINE_CHAR) {
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
    for (_pos = 0; _pos < _fileRawContent.size(); _pos++) {
        char ch = _fileRawContent[_pos];
        bool isAdded = false;

        (ch == '"' && !singleQuote) ? isThereQuotes = !isThereQuotes : true;
        (ch == '\'' && !isThereQuotes) ? singleQuote = !singleQuote : true;

        if (((ch == ' ' || ch == NEW_LINE_CHAR) && !isThereQuotes && !currentBlock.empty()) || Helper::literalToType.find(std::string(ch, 1)) != Helper::literalToType.end()) {
            
            codeStream += ch;
            macroTableEntry entry;
            // searching macro
            for (auto it : _macroTable)
            {
                if (it.key == currentBlock)
                {
                    if (it.isFunction)
                    {
                        codeStream += handleFunctionReplacement(it);
                    }
                    else
                    {
                        codeStream += getFinalValue(it.value);
                    }
                    isAdded = true;
                }
            }

            if (!isAdded) codeStream += currentBlock;
            
            currentBlock.clear();
        }
        else if (_pos == _fileRawContent.size() - 1)
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

std::string Preprocess::handleFunctionReplacement(macroTableEntry entry)
{
    std::vector<std::string> Args;
    std::string currentArg = EMPTY_STR;
    char ch = _fileRawContent[_pos];
    int amountOfArgs = 0;
    std::string codeStream = EMPTY_STR;

    if (_fileRawContent[_pos++] != '(') throw SyntaxError("Excepted a ( in function like macro");

    // reading the argument list 
    while (ch != ')' && _pos < _fileRawContent.size())
    {
        ch = _fileRawContent[_pos++];
        if (ch == COMMA_LITERAL || ch == ')')
        {
            if (!currentArg.empty())
            {
                Args.push_back(currentArg);
                amountOfArgs++;
                currentArg.clear();
            }
        }
        else if (ch != ' ')
        {
            currentArg += ch;
        }
    }
    if (amountOfArgs != entry.args.size()) throw SyntaxError("Unmatched amount of arguments to function like macro");

    currentArg = EMPTY_STR;
    for (auto ch : entry.value)
    {
        if (ch == ' '|| ch == '(' || ch == ')')
        {
            if (!currentArg.empty())
            {
                auto location = std::find(entry.args.begin(), entry.args.end(), currentArg);
                if (location != entry.args.end())
                {
                    codeStream += Args[location - entry.args.begin()];
                }
                else
                {
                    codeStream += currentArg;
                }
                currentArg.clear();
            }
            if (ch == '(' || ch == ')') codeStream += ch;
        }
        else
        {
            currentArg += ch;
        }
    }

    return codeStream;
}

bool Preprocess::checkMacroValueValidity(std::string &macroValue)
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
    if (getFromVector(macroValue) != nullptr) return true;
    return false;
}

bool Preprocess::isNumber(const std::string &number)
{
    return !number.empty() && std::all_of(number.begin(), number.end(), ::isdigit);
}

std::string Preprocess::getFinalValue(std::string key)
{
    if (getFromVector(getFromVector(key)->value) == nullptr)
    {
        return getFromVector(key)->value;
    }
    else
    {
        return getFinalValue(getFromVector(key)->value);
    }
}

macroTableEntry* Preprocess::getFromVector(std::string key)
{
    for (auto it : _macroTable)
    {
        if (it.key == key)
        {
            return &it;
        }
    }
    return nullptr;
}
