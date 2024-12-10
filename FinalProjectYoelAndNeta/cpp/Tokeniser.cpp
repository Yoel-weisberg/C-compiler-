#include "../Header/Tokenizer.h"
#include <algorithm>

Tokeniser::Tokeniser(const std::string& raw_code_str)
{
	std::string currentLiteral;

	for (int i = 0; i < raw_code_str.size(); i++)
	{
		if (raw_code_str[i] == BLANK && !currentLiteral.empty()) // If it's a seperator 
		{
			this->_tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
			currentLiteral = "";
		} // Any other valid char except for a digit
		else if ((std::find(Helper::separetors.begin(), Helper::separetors.end(), raw_code_str[i]) != Helper::separetors.end()) && !currentLiteral.empty())
		{
			this->_tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
			this->_tokens.push_back({ std::string(1, raw_code_str[i]), categoriseLiteral(std::string(1, raw_code_str[i])) });
			currentLiteral = "";
		} // Check for char initilization
		else if (raw_code_str[i] == SINGLE_QUOTE_LITERAL && raw_code_str[i + DIS_BETWEEN_SINGLE_QOUTES] == SINGLE_QUOTE_LITERAL)
		{
			currentLiteral = raw_code_str.substr(i, DIS_BETWEEN_SINGLE_QOUTES + 1);
			this->_tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
			i += DIS_BETWEEN_SINGLE_QOUTES; // Skip char initilization 
			currentLiteral = "";
		}
		else if ((i == raw_code_str.size() - 1)) // Check if end is reached
		{
			currentLiteral += raw_code_str[i];
			if (!(currentLiteral.empty() || currentLiteral[0] == BLANK)) // If not empty
			{
				this->_tokens.push_back({ currentLiteral, categoriseLiteral(currentLiteral) });
				currentLiteral = "";
			}
		}
		else
		{
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
	if (literal == std::string(1, LPAREN_LIT))
	{
		return LPAREN;
	}
	else if (literal == std::string(1, RPAREN_LIT))
	{
		return RPAREN;
	}
	else if (literal == std::string(1, ADDITION_LIT))
	{
		return ADDITION;
	}
	else if (literal == std::string(1, MULTIPLICATION_LIT))
	{
		return MULTIPLICATION;
	}
	else if (literal == std::string(1, DIVISION_LIT))
	{
		return DIVISION;
	}
	else if (literal == std::string(1, SUBSTRICTION_LIT))
	{
		return SUBTRACTION;
	}
	else if (literal == std::string(1, CURL_BR_L_LIT) || literal == std::string(1, CURL_BR_R_LIT))
	{
		return CURL_BR;
	}
	else if (isNumber(literal))
	{
		// Assuming INT is the default case for literals that aren't operators or parentheses
		return INT;
	}
	else if (literal[0] == EQUAL_SIGN_LIT)
	{
		return EQUAL_SIGN;
	}
	else if (literal[0] == SEMICOLON_LIT)
	{
		return SEMICOLON;
	}
	else if (Helper::isChar(literal))
	{
		return CHAR_LITERAL;
	}
	else if (std::find(Helper::definedTypes.begin(), Helper::definedTypes.end(), literal) != Helper::definedTypes.end())
	{
		return TYPE_DECLERATION;
	}
	//	Check for pointer declerations
	if (literal[literal.size() - 1] == MULTIPLICATION_LIT && (std::find(Helper::definedTypes.begin(), Helper::definedTypes.end(), removeSpecialCharacter(literal)) != Helper::definedTypes.end()))
	{
		return PTR_TYPE_DECLERATION;
	}
	// Check for address referance
	else if (literal[0] == AMPERSEND_LIT && literal.size() > 1)
	{
		return ADDR_REFERENCE;
	}
	//else if ((literal[literal.size() - 2] == SQUARE_BR_L_LIT) 
	//	&& (literal[literal.size() - 1] == SQUARE_BR_R_LIT)
	//	&& (std::find(Helper::definedTypes.begin(), Helper::definedTypes.end(), removeSpecialCharacter(literal)) != Helper::definedTypes.end()))
	//{
	//	std::cout << "Found Array!!! Hurray!!!" << std::endl;
	//	return ARR_TYPE_DECLERATION;
	//}
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

std::string Tokeniser::removeSpecialCharacter(std::string str)
{
	for (int i = 0; i < str.size(); i++) {

		// Finding the character whose
		// ASCII value fall under this
		// range
		if (str[i] < 'A' || str[i] > 'Z' && str[i] < 'a'
			|| str[i] > 'z') {
			// erase function to erase
			// the character
			str.erase(i, 1);
			i--;
		}
	}
	return str;
}

std::ostream& operator<<(std::ostream& os, const Tokeniser& tokeniser)
{
	for (Token pr : tokeniser.getTokens())
	{
		os << "<" << pr.getType() << ", " << pr.getLiteral() << ">" << std::endl;
	}
	return os;
}
