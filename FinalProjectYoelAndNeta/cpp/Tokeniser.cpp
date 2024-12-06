#include "../Header/Tokenizer.h"
#include <algorithm>

Tokeniser::Tokeniser(const std::string& string)
{
	std::string currentLiteral;

	for (int i = 0; i < string.size(); i++)
	{
		if (string[i] == ' ' && !currentLiteral.empty()) // If it's a seperater 
		{
			this->tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
			currentLiteral = "";
		} // Any other valid char exept for a digit
		else if (Helper::literalToType.find(currentLiteral) != Helper::literalToType.end())
		{
			this->tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
			this->tokens.push_back({ std::string(1, string[i]), categoriseLiteral(std::string(1, string[i])) });
			currentLiteral = "";
		}
		else if ((i == string.size() - 1))
		{
			currentLiteral += string[i];
			if (!(currentLiteral.empty() || currentLiteral[0] == ' '))
			{
				this->tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
				currentLiteral = "";
			}
		}
		else
		{
			currentLiteral += string[i];
		}
	}
}

std::vector<Token> Tokeniser::getTokens() const
{
	return this->tokens;
}


Tokens_type Tokeniser::categoriseLiteral(const std::string& literal)
{
	if (Helper::literalToType.find(literal) != Helper::literalToType.end())
	{
		return Helper::literalToType.find(literal)->second;
	}
	else if (isNumber(literal))
	{
		// Assuming INT is the default case for literals that aren't operators or parentheses
		return INT;
	}
	else if (std::find(Helper::definedTypes.begin(), Helper::definedTypes.end(), literal) != Helper::definedTypes.end())
	{
		return TYPE_DECLERATION;
	}
	else if (literal == "IF")
	{
		return IF_WORD;
	}
	else if (literal == "else")
	{
		return ELSE;
	}
	else if (!literal.empty())
	{
		return IDENTIFIER; // returning identifier without error handeling which would happen in the syntex analysis phrase
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