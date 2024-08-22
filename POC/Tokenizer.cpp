#include "Tokenizer.h"

Tokeniser::Tokeniser(const std::string& string)
{
	std::string currentLiteral;

	for (int i = 0; i < string.size(); i++)
	{
		if (string[i] == ' ')
		{
            if (currentLiteral.size() != 0)
            {
                this->tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
                currentLiteral = "";
            }
		}
        else if (string[i] == LPREN_LITERAL || string[i] == RPREN_LITERAL ||
            string[i] == ADDITION_LITERAL || string[i] == MULTIPLICATION_LITERAL ||
            string[i] == DIVISION_LITERAL || string[i] == SUBSTRICTION_LITERAL)
        {
            if (currentLiteral.size() != 0)
            {
                this->tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
            }
         
            this->tokens.push_back({ std::string(1, string[i]) , categoriseLiteral(std::string(1, string[i]))});
            currentLiteral = "";
        }
        
        else if (std::isdigit(string[i]))
        {
            currentLiteral += string[i];
        }
	}
}

std::vector<std::pair<std::string, Tokens>> Tokeniser::getTokens() const
{
    return this->tokens;
}


Tokens Tokeniser::categoriseLiteral(const std::string& literal)
{
    if (literal == std::string(1, LPREN_LITERAL))
    {
        return LPREN;
    }
    else if (literal == std::string(1, RPREN_LITERAL))
    {
        return RPREN;
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
        return SUBSTRACTION;
    }
    else if (isNumber(literal))
    {
        // Assuming INT is the default case for literals that aren't operators or parentheses
        return INT;
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
    for (auto pr : tokeniser.getTokens())
    {
        os << "<" << pr.first << ", " << pr.second << ">" << std::endl;
    }
    return os;
}
