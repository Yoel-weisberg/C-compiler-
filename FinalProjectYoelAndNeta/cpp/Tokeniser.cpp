#include "../Header/Tokenizer.h"
#include <algorithm>

Tokeniser::Tokeniser(const std::string& string)
{
    std::string currentLiteral;

    for (int i = 0; i < string.size(); i++)
    {
        if (string[i] == ' ') // If it's a seperater 
        {
            if (currentLiteral.size() != 0)
            {
                this->tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
                currentLiteral = "";
            }
        } // Any other valid char exept for a digit
        else if (std::find(Helper::separeters.begin(), Helper::separeters.end(), string[i]) != Helper::separeters.end())
        {
            this->tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
            this->tokens.push_back({ std::string(1, string[i]), categoriseLiteral(std::string(1, string[i]))});
        }
    }

    this->tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
}

std::vector<Token> Tokeniser::getTokens() const
{
    return this->tokens;
}


Tokens_type Tokeniser::categoriseLiteral(const std::string& literal)
{
    if (literal == std::string(1, LPAREN_LITERAL))
    {
        return LPAREN;
    }
    else if (literal == std::string(1, RPAREN_LITERAL))
    {
        return RPAREN;
    }
    else if (literal == std::string(1, ADDITION_LITERAL))
    {
        return ADDITION;
    }
    else if (literal == std::string(1, MULTIPLICATION_LITERAL))
    {
        return MULTIPLICATION;
    }
    else if (literal == std::string(1, DIVISION_LITERAL))
    {
        return DIVISION;
    }
    else if (literal == std::string(1, SUBSTRICTION_LITERAL))
    {
        return SUBTRACTION;
    }
    else if (isNumber(literal))
    {
        // Assuming INT is the default case for literals that aren't operators or parentheses
        return INT;
    }
    else if (literal[0] == EQUEL_SIGN_LITERAL)
    {
        return EQUEL_SIGN;
    }
    else if (literal[0] == SEMICOLUMN)
    {
        return SEMICOLUMN;
    }
    else if (std::find(Helper::definedTypes.begin(), Helper::definedTypes.end(), literal) != Helper::definedTypes.end())
    {
        return TYPE_DECLERATION;
    }
    else
    {
        return IDENTIFIER; // returning identifier without error handeling which would happen in the syntex analysis phrase
    }
  
}



bool Tokeniser::isNumber(const std::string& literal)
{
    for (char c : literal) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

std::ostream& operator<<(std::ostream& os, const Tokeniser& tokeniser)
{
    for (Token pr : tokeniser.getTokens())
    {
        os << "<" << pr.getType()  << ", " << pr.getLiteral() << ">" << std::endl;
    }
    return os;
}