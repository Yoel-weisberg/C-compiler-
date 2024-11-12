#include "Preprocess.h"
#include <cctype>
#include <algorithm>
#include "SyntexError.h"

void Preprocess::removeComments() {
    // Implement comment removal logic using _fileRawContent.
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

            } else if (macroName == "undef") {
                // TODO: Remove define from macro table
            } else if (macroName == "include") {
                // TODO: Manage include
            }
        }
    }

    // Replace macros in the macro table
    replaceMacro();
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
