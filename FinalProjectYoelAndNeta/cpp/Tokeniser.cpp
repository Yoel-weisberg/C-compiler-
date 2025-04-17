#include "../Header/Tokenizer.h"
#include <algorithm>

Tokeniser::Tokeniser(const std::string& raw_code_str)
{
    std::string currentLiteral;

    for (int i = 0; i < raw_code_str.size(); i++)
    {
        // handaling equel cmp
        if (raw_code_str[i] == '=' && raw_code_str[i + 1] == '=')
        {
            this->_tokens.push_back({ "==", EQUELS_CMP});
            currentLiteral = EMPTY_STR;
            i += 2;
        }
        // Handle string literals
        if (raw_code_str[i] == '"')
        {
            // If we have a non-empty current literal, push it as a token
            if (!currentLiteral.empty())
            {
                this->_tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
                currentLiteral = EMPTY_STR;
            }

            // Start building the string literal
            currentLiteral += raw_code_str[i]; // Add opening quote
            i++; // Move past the opening quote

            // Capture everything until the closing quote
            while (i < raw_code_str.size() && raw_code_str[i] != '"')
            {
                // Handle escape sequences if needed
                if (raw_code_str[i] == '\\' && i + 1 < raw_code_str.size())
                {
                    currentLiteral += raw_code_str[i]; // Add backslash
                    i++; // Move to the escaped character
                    currentLiteral += raw_code_str[i]; // Add escaped character
                    i++; // Move past the escaped character
                }
                else
                {
                    currentLiteral += raw_code_str[i]; // Add normal character
                    i++; // Move to next character
                }
            }

            // Add closing quote if found
            if (i < raw_code_str.size() && raw_code_str[i] == '"')
            {
                currentLiteral += '"';
                this->_tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
                currentLiteral = EMPTY_STR;
            }
            else
            {
                // Handle unclosed string error
                // For now, just add what we have
                this->_tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
                currentLiteral = EMPTY_STR;
            }
            continue; // Continue to next iteration
        }

        // Handle whitespace
        if (raw_code_str[i] == BLANK || raw_code_str[i] == NEW_LINE_CHAR)
        {
            if (!currentLiteral.empty())
            {
                this->_tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
                currentLiteral = EMPTY_STR;
            }
            continue; // Skip whitespace
        }

        // Handle operators and special characters
        if (Helper::literalToType.find(std::string(1, raw_code_str[i])) != Helper::literalToType.end())
        {
            // First, push any existing literal
            if (!currentLiteral.empty())
            {
                this->_tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
                currentLiteral = EMPTY_STR;
            }

            // Then push the operator/special character
            this->_tokens.push_back({ std::string(1, raw_code_str[i]),
                                    categoriseLiteral(std::string(1, raw_code_str[i])) });
            continue; // Continue to next iteration
        }

        // Handle character literals
        if (raw_code_str[i] == '\'')
        {
            // First, push any existing literal
            if (!currentLiteral.empty())
            {
                this->_tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
                currentLiteral = EMPTY_STR;
            }

            // Capture the character literal
            std::string charLiteral;
            charLiteral += raw_code_str[i]; // Add opening quote

            // Add the character (or escape sequence)
            if (i + 1 < raw_code_str.size())
            {
                i++;
                if (raw_code_str[i] == '\\' && i + 1 < raw_code_str.size())
                {
                    charLiteral += raw_code_str[i]; // Add backslash
                    i++;
                    charLiteral += raw_code_str[i]; // Add escaped character
                }
                else
                {
                    charLiteral += raw_code_str[i]; // Add character
                }
            }

            // Add closing quote if found
            if (i + 1 < raw_code_str.size() && raw_code_str[i + 1] == '\'')
            {
                i++;
                charLiteral += '\'';
                this->_tokens.push_back({ charLiteral, categoriseLiteral(charLiteral) });
            }
            else
            {
                // Handle unclosed character literal error
                this->_tokens.push_back({ charLiteral, categoriseLiteral(charLiteral) });
            }
            continue;
        }

        // Default case: add character to current literal
        currentLiteral += raw_code_str[i];

        // Handle end of input
        if (i == raw_code_str.size() - 1 && !currentLiteral.empty())
        {
            this->_tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
        }
    }
}

std::vector<Token> Tokeniser::getTokens() const
{
    return _tokens;
}

Tokens_type Tokeniser::categoriseLiteral(const std::string& literal)
{
    if (Helper::literalToType.find(literal) != Helper::literalToType.end())
    {
        return Helper::literalToType.find(literal)->second;
    }
    else if (Helper::Keywords.find(literal) != Helper::Keywords.end())
    {
        return Helper::Keywords.find(literal)->second;
    }
    else if (isNumber(literal))
    {
        return INT;
    }
    else if (isStringLiteral(literal))
    {
        return STRING_LITERAL;
    }
    else if (isFloat(literal))
    {
        return FLOAT;
    }
    else if (std::find(Helper::definedTypes.begin(), Helper::definedTypes.end(), literal) != Helper::definedTypes.end())
    {
        return TYPE_DECLERATION;
    }
    else if (!literal.empty())
    {
        return IDENTIFIER;
    }

    // Default fallback (should add error handling here)
    return IDENTIFIER;
}

bool Tokeniser::isNumber(const std::string& literal)
{
    if (literal.empty()) return false;
    for (char c : literal) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool Tokeniser::isFloat(const std::string& literal)
{
    std::regex floatRegex(R"(^[+-]?(\d+(\.\d*)?|\.\d+)([eE][+-]?\d+)?$)");
    return std::regex_match(literal, floatRegex);
}

bool Tokeniser::isStringLiteral(const std::string& literal)
{
    return literal.size() >= 2 &&
        literal[0] == '"' &&
        literal[literal.size() - 1] == '"';
}

std::ostream& operator<<(std::ostream& os, const Tokeniser& tokeniser)
{
    for (Token pr : tokeniser.getTokens())
    {
        os << "<" << pr.getType() << ", " << pr.getLiteral() << ">" << std::endl;
    }
    return os;
}