#include "SyntaxAnalysis.h"
#include <regex>

SyntaxAnalysis::SyntaxAnalysis(const std::vector<Token> &_tokens) : _tokens(_tokens)
{
	checkParentheses();
	validSentences();
}

void SyntaxAnalysis::checkParentheses() // !For now! check only regular parenthesis "(" ")"
{
	int LPARENCount = 0;
	int chrIndex = 0;
	for (auto pr : _tokens)
	{
		chrIndex += pr.getLiteral().size();
		// Check for left parenthesis and increment the counter
		if (pr.getType() == LPAREN)
		{
			LPARENCount++;
		}
		// Check for right parenthesis and decrement the counter

		else if (pr.getType() == RPAREN)
		{
			LPARENCount--;

			// If count goes negative, there's an unmatched right parenthesis
			if (LPARENCount < 0)
			{
				throw SyntaxError("Unmatched right parenthesis", chrIndex);
			}
		}
	}

	// After loop, if count is positive, there are unmatched left parentheses
	if (LPARENCount > 0)
	{
		throw SyntaxError("Unmatched left parenthesis", chrIndex);
	}
}

int SyntaxAnalysis::variableDefinitionStructure(int pos)
{
	if (pos + 1 >= _tokens.size())
	{
		throw SyntaxError("missing Identifier");
	}
	if (_tokens[pos + 1].getType() != IDENTIFIER)
	{
		throw SyntaxError("missing Identifier");
	}
	if (!Helper::checkIdentifier(_tokens[pos + 1].getLiteral()))
	{
		throw SyntaxError("Identifier not valid");
	}

	// Check if there are enough tokens for the next set of operations
	if (pos + MIN_NUM_OF_OPERATIONS >= _tokens.size())
	{
		throw SyntaxError("unexpected end of input");
	}
	if (_tokens[pos + MIN_NUM_OF_OPERATIONS].getType() == EQUAL_SIGN)
	{
		// Check for additional tokens needed for this branch
		if (pos + MAX_NUM_OF_OPERATIONS_FOR_DECLERATION - 1 >= _tokens.size())
		{
			throw SyntaxError("missing value after equal sign");
		}
		if (!doesVariableFitType(_tokens[pos].getLiteral(), _tokens[pos + MAX_NUM_OF_OPERATIONS_FOR_DECLERATION - 1].getLiteral()))
		{
			throw SyntaxError("Variable type dosent fit decleration");
		}
		pos + MAX_NUM_OF_OPERATIONS_FOR_DECLERATION >= _tokens.size() ? throw SyntaxError("excepted a semicolon") : true;
		if (!_tokens[pos + MAX_NUM_OF_OPERATIONS_FOR_DECLERATION].getType() == SEMICOLUMN)
		{
			throw SyntaxError("excepted a semicolon");
		}
		// advancing pos by 4 to get to the next action
		return pos + MAX_NUM_OF_OPERATIONS_FOR_DECLERATION;
	}
	else if (_tokens[pos + MIN_NUM_OF_OPERATIONS].getType() == SEMICOLUMN)
	{
		return pos + MIN_NUM_OF_OPERATIONS;
	}
	else
	{
		throw SyntaxError("excepted a semicolon");
	}
}

int SyntaxAnalysis::arrTypeVariableDefinitionStructure(int pos)
{
	int type_pos = pos; // Position of the array type
	pos++;

	if ((_tokens[pos].getType() != IDENTIFIER) && !(Helper::checkIdentifier(_tokens[pos].getLiteral().substr(0, _tokens[pos + 1].getLiteral().size() - 2))))
	{
		throw SyntaxError("Missing Identifier");
	}
	std::cout << "pos -- > " << _tokens[pos].getLiteral() << std::endl;
	pos++;
	if (_tokens[pos].getType() != EQUAL_SIGN)
	{
		throw SyntaxError("missing '='");
	}
	pos++;
	// check array initilization, bracket placement
	std::cout << "pos -- > " << _tokens[pos].getLiteral() << std::endl;
	if (_tokens[pos].getType() == SQR_BR_L || _tokens[pos + 2].getType() == SQR_BR_R)
	{
		throw SyntaxError("Missing bracket");
	}
	// check inside of initilization in parser
	pos++;
	pos++;
	pos++;
	return pos;
}

bool SyntaxAnalysis::doesVariableFitType(const std::string &type, std::string value)
{
	if (type == "float")
	{
		return Helper::isFloat(value);
	}
	else if (type == "int")
	{
		return Helper::isInteger(value);
	}
	else if (type == "char")
	{
		return Helper::isChar(value);
	}
}

void SyntaxAnalysis::validSentences()
{
	int pos = 0;
	try
	{
		while (pos < _tokens.size())
		{
			if (_tokens[pos + 1].getLiteral().substr(_tokens[pos + 1].getLiteral().size() - 1, _tokens[pos + 1].getLiteral().size()) == "[]")
			{
				std::cout << "Arr to syn --> " << _tokens[pos].getLiteral() << std::endl;
				pos = arrTypeVariableDefinitionStructure(pos) + 1; // send identifier
			}
			else if (_tokens[pos].getType() == TYPE_DECLERATION)
			{
				// adding 1 because function returns the last token of the sentence
				pos = variableDefinitionStructure(pos) + 1;
			}
			else if (_tokens[pos].getType() == PTR_TYPE_DECLERATION)
			{
				pos = ptrVariableDefenitionStructure(pos + 1) + 1;
			}
			else if (_tokens[pos].getType() == IF_WORD)
			{
				checkIfStructure(pos);
			}
			// TODO - checking if the sentence just a defined identifier (like just 3; or somthing like that)
			// TODO - need to check if its a redefinition of t a symbol
			// TODO - need to check if the sentnce is an algebric sentnece
			// there is no suppert for other sentence structure
			else
			{
				std::cout << " Noooooooo ->> " << _tokens[pos].getLiteral() << std::endl;
				std::cout << " Noooooooo ->> " << _tokens[pos].getType() << std::endl;
				throw SyntaxError("Not defined sentence", pos);
			}
		}
	}
	catch (const std::out_of_range &e)
	{
		throw SyntaxError("Got to end of token stream without a semicolumn");
	}
}

int SyntaxAnalysis::ptrVariableDefenitionStructure(int pos)
{
	try
	{
		if (pos + 1 >= _tokens.size())
		{
			throw SyntaxError("missing Identifier");
		}
		if (_tokens[pos].getType() != IDENTIFIER)
		{
			throw SyntaxError("missing Identifier");
		}
		if (!Helper::checkIdentifier(_tokens[pos].getLiteral()))
		{
			throw SyntaxError("Incorrect identifier");
		}
		pos++;
		if (_tokens[pos].getType() != EQUAL_SIGN)
		{
			throw SyntaxError("missing '='");
		}
		pos++;
		if (_tokens[pos].getType() != ADDR_REFERENCE)
		{
			throw SyntaxError("Missing Address Refernce");
		}
		if (!Helper::checkIdentifier(_tokens[pos].getLiteral().substr(1, _tokens[pos].getLiteral().size() - 1)))
		{
			throw SyntaxError("Incorrect identifier");
		}
		pos++;
		if (_tokens[pos].getType() != SEMICOLUMN)
		{
			throw SyntaxError("Missing Line Ending");
		}
		return pos;
	}
	catch (const std::exception &)
	{
		throw SyntaxError("Incorrect pointer decleration");
	}
}

int SyntaxAnalysis::checkIfStructure(int &pos)
{
	int chrIndex = 0;
	bool isThereIf = false;

	// Check if the current token is "if"
	if (_tokens[pos].getType() == IF_WORD)
	{
		isThereIf = true;
		pos++; // Move to the next token

		// Ensure the next token is a left parenthesis "("
		if (_tokens[pos].getType() != LPAREN)
		{
			throw SyntaxError("Expected '(' after 'if'", chrIndex);
		}
		pos++;

		// Check the actual condition
		checkConditionStructure(pos);

		// Ensure the next token is a left curly brace "{"
		if (_tokens[pos].getType() != L_CURLY_PRAN)
		{
			throw SyntaxError("Expected '{' after 'if' condition", chrIndex);
		}
		pos++;

		// Find the matching right curly brace "}"
		int braceCount = 1;
		while (pos < _tokens.size() && braceCount > 0)
		{
			if (_tokens[pos].getType() == L_CURLY_PRAN)
				braceCount++;
			else if (_tokens[pos].getType() == R_CURLY_PRAN)
				braceCount--;

			pos++;
		}

		if (braceCount > 0)
		{
			throw SyntaxError("Unmatched '{' in 'if' block", chrIndex);
		}
	}

	// Check for "else" without a preceding "if"
	if (_tokens[pos].getType() == ELSE)
	{
		if (!isThereIf)
		{
			throw SyntaxError("'else' without a previous 'if'", chrIndex);
		}

		pos++; // Move to the next token

		// Ensure the next token is a left curly brace "{"
		if (_tokens[pos].getType() != L_CURLY_PRAN)
		{
			throw SyntaxError("Expected '{' after 'else'", chrIndex);
		}
		pos++;

		// Find the matching right curly brace "}"
		int braceCount = 1;
		while (pos < _tokens.size() && braceCount > 0)
		{
			if (_tokens[pos].getType() == L_CURLY_PRAN)
				braceCount++;
			else if (_tokens[pos].getType() == R_CURLY_PRAN)
				braceCount--;

			pos++;
		}

		if (braceCount > 0)
		{
			throw SyntaxError("Unmatched '{' in 'else' block", chrIndex);
		}
	}

	return pos; // Return the updated position
}

int SyntaxAnalysis::checkConditionStructure(int &pos)
{
	int numPran = 1;
	enum currentState
	{
		OPERAND,
		VALUE
	};
	currentState currentType = OPERAND;
	while (numPran != 0)
	{
		Tokens_type tokT = _tokens[pos].getType();
		// if its a value to campare to and not a opreand
		if (tokT == INT || tokT == IDENTIFIER || tokT == FLOAT)
		{
			if (currentType != OPERAND)
			{
				throw SyntaxError("cant give a number after a number in an if statement", pos);
			}
			currentType = VALUE;
		}
		else if (tokT == AND || tokT == OR || tokT == EQUELS_CMP || tokT == LPAREN || tokT == RPAREN || tokT == LOWER_THEN || tokT == HIGHER_THEN)
		{
			if (tokT == LPAREN)
			{
				numPran++;
			}
			else if (tokT == RPAREN)
			{
				numPran--;
			}
			if (currentType != VALUE)
			{
				throw SyntaxError("cant give an operand after an operand in an if statement", pos);
			}
			currentType = OPERAND;
		}
		pos++;
	}
}
