#include "../Header/Tokenizer.h"
#include <algorithm>

Tokeniser::Tokeniser(const std::string& raw_code_str)
{
	std::string currentLiteral;

	for (int i = 0; i < raw_code_str.size(); i++)
	{
		if ((raw_code_str[i] == BLANK || raw_code_str[i] == NEW_LINE_CHAR) && !currentLiteral.empty()) // If it's a seperator 
		{
			this->_tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
			currentLiteral = EMPTY_STR;
		} // Any other valid char except for a digit
		else if (Helper::literalToType.find(std::string(1, raw_code_str[i])) != Helper::literalToType.end())
		{
			if (!currentLiteral.empty())  this->_tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
			this->_tokens.push_back({ std::string(1, raw_code_str[i]), categoriseLiteral(std::string(1, raw_code_str[i])) });
			currentLiteral = EMPTY_STR;
		}
		else if (Helper::literalToType.find(std::string(1, raw_code_str[i])) != Helper::literalToType.end())
		{
			currentLiteral = raw_code_str.substr(i, DIS_BETWEEN_SINGLE_QOUTES + 1);
			if (!currentLiteral.empty())  this->_tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
			i += DIS_BETWEEN_SINGLE_QOUTES; // Skip char initilization 
			currentLiteral = EMPTY_STR;
		}/*
		else if (raw_code_str[i] == CURL_BR_L_LIT)
		{

		}*/
		else if ((i == raw_code_str.size() - 1)) // Check if end is reached
		{
			currentLiteral += raw_code_str[i];
			if (!(currentLiteral.empty() || currentLiteral[0] == BLANK)) // If not empty
			{
				this->_tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
				currentLiteral = EMPTY_STR;
			}
		}
		else if (raw_code_str[i] != BLANK && raw_code_str[i] != NEW_LINE_CHAR)
		{
			currentLiteral += raw_code_str[i];
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
	else if (isNumber(literal))
	{
		// Assuming INT is the default case for literals that aren't operators or parentheses
		return INT;
	}
	else if (isFloat(literal))
	{
		return FLOAT;
	}
	else if (std::find(Helper::definedTypes.begin(), Helper::definedTypes.end(), literal) != Helper::definedTypes.end())
	{
		return TYPE_DECLERATION;
	}
	else if (literal == "if")
	{
		return IF_WORD;
	}
	else if (literal == "else")
	{
		return ELSE;
	}
	else if(literal == "return")
	{
		return RETURN_STATEMENT;
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

bool Tokeniser::isFloat(const std::string& literal)
{
	std::regex floatRegex(R"(^[+-]?(\d+(\.\d*)?|\.\d+)([eE][+-]?\d+)?$)");
    return std::regex_match(literal, floatRegex);
}

std::ostream& operator<<(std::ostream& os, const Tokeniser& tokeniser)
{
	for (Token pr : tokeniser.getTokens())
	{
		os << "<" << pr.getType() << ", " << pr.getLiteral() << ">" << std::endl;
	}
	return os;
}
