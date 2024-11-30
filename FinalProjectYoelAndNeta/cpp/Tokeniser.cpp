#include "../Header/Tokenizer.h"
#include <algorithm>

Tokeniser::Tokeniser(const std::string& raw_code_str)
{
	std::string currentLiteral;

	for (int i = 0; i < raw_code_str.size(); i++)
	{
		std::cout << i << "\tCurrent Literal --> \t" << currentLiteral << std::endl;
		if (raw_code_str[i] == BLANK && !currentLiteral.empty()) // If it's a seperator 
		{
			std::cout << raw_code_str[i] << "\tIs a seperator" << std::endl;
			this->_tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
			currentLiteral = "";
		} // Any other valid char except for a digit
		else if ((std::find(Helper::separetors.begin(), Helper::separetors.end(), raw_code_str[i]) != Helper::separetors.end()) && !currentLiteral.empty())
		{
			std::cout << raw_code_str[i] << "\tIs not a digit" << std::endl;
			this->_tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
			this->_tokens.push_back({ std::string(1, raw_code_str[i]), categoriseLiteral(std::string(1, raw_code_str[i])) });
			currentLiteral = "";
		}
		else if ((i == raw_code_str.size() - 1)) // Check if end is reached
		{
			std::cout << raw_code_str[i] << "\tIs the end" << std::endl;
			currentLiteral += raw_code_str[i];
			if (!(currentLiteral.empty() || currentLiteral[0] == BLANK)) // If not empty
			{
				this->_tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
				currentLiteral = "";
			}
		}
		else
		{
			std::cout << raw_code_str[i] << "\tIs something else" << std::endl;
			currentLiteral += raw_code_str[i];
		}
	}
}

std::vector<Token> Tokeniser::getTokens() const
{
	return this->_tokens;
}


Tokens_type Tokeniser::categoriseLiteral(const std::string& literal)
{
	std::cout << "\t\tCtegorise: " << literal << std::endl;
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
	else if (literal[0] == SEMICOLUMN_LITERAL)
	{
		std::cout << "Semicolon!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
		return SEMICOLUMN;
	}
	else if (std::find(Helper::definedTypes.begin(), Helper::definedTypes.end(), literal) != Helper::definedTypes.end())
	{
		return TYPE_DECLERATION;
	}
	else if (!literal.empty())
	{
		return IDENTIFIER; // Returning identifier without error handeling which would happen in the syntax analysis phase
	}

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

std::ostream& operator<<(std::ostream& os, const Tokeniser& tokeniser)
{
	for (Token pr : tokeniser.getTokens())
	{
		os << "<" << pr.getType() << ", " << pr.getLiteral() << ">" << std::endl;
	}
	return os;
}