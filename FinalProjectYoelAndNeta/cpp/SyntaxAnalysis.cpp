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
		//if (!_tokens[pos + MAX_NUM_OF_OPERATIONS_FOR_DECLERATION].getType() == SEMICOLUMN)
		//{
		//	throw SyntaxError("excepted a semicolon");
		//}
		// advancing pos by 4 to get to the next action
		return pos + MAX_NUM_OF_OPERATIONS_FOR_DECLERATION;
	}
	else if (_tokens[pos + MIN_NUM_OF_OPERATIONS].getType() == SEMICOLUMN)
	{
		return pos + MIN_NUM_OF_OPERATIONS;
	}
	else if (_tokens[pos + MIN_NUM_OF_OPERATIONS].getType() == LPAREN)
	{
		int currentIndex = pos + MIN_NUM_OF_OPERATIONS;
		return checkFunctionDecleration(currentIndex);
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
	if (type == FLOAT_TYPE_LIT )
	{
		return Helper::isFloat(value);
	}
	else if (type == INT_TYPE_LIT )
	{
		return Helper::isInteger(value);
	}
	else if (type == CHAR_TYPE_LIT)
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
			if ((pos + 2 < _tokens.size()) && ( _tokens[pos + 1].getLiteral() == "[" && _tokens[pos + 2].getLiteral() == "]"))
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
				checkFlowControlStructure(pos);
			}
			else if (_tokens[pos].getType() == R_CURLY_BRACK)
			{
				pos++;
			}
			else if(_tokens[pos].getType() == IDENTIFIER)
			{
				pos = checkIdentifier(pos);
			}
			else if (_tokens[pos].getType() == RETURN_STATEMENT)
			{
				pos++;
			}
			else if (_tokens[pos].getType() >= DO_WHILE_LOOP && _tokens[pos].getType() <= FOR_LOOP)
			{
				pos = checkLoopStructure(pos);
			}
			else if (_tokens[pos].getType() == STRUCT)
			{
				pos = checkStructStructure(pos);
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


int SyntaxAnalysis::checkFunctionDecleration(int& pos)
{
	// move forowerd the (
	pos++;
	while (_tokens[pos].getType() == TYPE_DECLERATION)
	{
		pos++;
		if (_tokens[pos].getType() != IDENTIFIER)
		{
			throw SyntaxError("Excepted an identifier after type decleration in function decleration");
		}
		pos++;
		if (_tokens[pos].getType() != COMMA || _tokens[pos].getType() != RPAREN)
		{
			throw SyntaxError("Excepted a comma or a pran after paramter decleration");
		}
		pos++;
	}
	if (_tokens[pos].getType() == RPAREN)
	{
		pos++;
	}
	return pos;
}

int SyntaxAnalysis::checkIdentifier(int& pos)
{
	// Check for struct member assignment
	if (_tokens[pos].getLiteral().find(".") != std::string::npos)
	{
		return 0;
	}

	// move past the identifier itself
	pos++;
	if (_tokens[pos].getType() == LPAREN)
	{
		// move past LPRAN
		pos++;
		while (_tokens[pos].getType() == INT || _tokens[pos].getType() == FLOAT)
		{
			pos++;
			if (_tokens[pos].getType() != COMMA && _tokens[pos].getType() != RPAREN)
			{
				throw SyntaxError("Excepted a comma or a ) after argument in function call");
			}
		}
		if (_tokens[pos].getType() == RPAREN) pos++;
	}
	if (_tokens[pos].getType() != SEMICOLUMN && pos < _tokens.size()) throw SyntaxError("Excepted a semicolumn");
	return ++pos;
}


int SyntaxAnalysis::checkFlowControlStructure(int& pos)
{
	int chrIndex = 0;
	bool isThereIf = false;

	// Check if the current token is "if"
	if (_tokens[pos].getType() == IF_WORD)
	{
		isThereIf = true;
		checkIfStructure(pos, chrIndex);
	}

	// Check for "else" without a preceding "if"
	if (_tokens[pos].getType() == ELSE)
	{
		if (!isThereIf)
		{
			throw SyntaxError("'else' without a previous 'if'", chrIndex);
		}
		checkElseStructure(pos, chrIndex);
	}

	return pos; // Return the updated position
}

int SyntaxAnalysis::checkLoopStructure(int& pos)
{
	Tokens_type loopType = _tokens[pos].getType();
	pos++;

	if (loopType != DO_WHILE_LOOP && _tokens[pos].getType() != LPAREN)
	{
		throw SyntaxError("Missing '(' ", pos);
	}
	else if (loopType == DO_WHILE_LOOP && _tokens[pos].getType() != L_CURLY_BRACK)
	{
		throw SyntaxError("Missing '{' ", pos);
	}
	pos++;

	// while -- just check condition 
	if (loopType == WHILE_LOOP)
	{
		return checkWhileLoopStructure(pos);
	}
	// for -- check identifier --> check condition --> check rotation 
	else if (loopType == FOR_LOOP)
	{
		return checkForLoopStructure(pos);
	}
	// do -- check brackets --> check condition
	else if (loopType == DO_WHILE_LOOP)
	{
		return checkDoWhileStructure(pos) + 1;
	}
}

int SyntaxAnalysis::checkForLoopStructure(int& pos)
{
	checkForLoopInitialization(pos);
	pos++;
	// Check condition
	checkConditionStructure(pos, true);
	pos++;

	// Check increment - NOTE! it can be blank, but must include a semicolon 
	do {
		pos++;
		if (_tokens[pos].getType() == L_CURLY_BRACK)
		{
			throw SyntaxError("Missing ')' for increment ", pos);
		}
	} while (_tokens[pos].getType() != RPAREN);
	pos++;
	pos++;
	// Check brackets
	std::pair<int, int> bCheck = findMatchingCurlB(pos);
	pos = bCheck.first;
	if (bCheck.second > 0)
	{
		throw SyntaxError("Unmatched '{' in 'for' block", pos);
	}
	return pos;
}

int SyntaxAnalysis::checkWhileLoopStructure(int& pos)
{
	checkConditionStructure(pos);
	pos++;
	std::pair<int, int> bCheck = findMatchingCurlB(pos);
	pos = bCheck.first;
	if (bCheck.second > 0)
	{
		throw SyntaxError("Unmatched '{' in 'while' block", pos);
	}
	return pos;
}

std::pair<int, int> SyntaxAnalysis::findMatchingCurlB(int& pos)
{
	// Find the matching right curly brace "}"
	int braceCount = 1;
	while (pos < _tokens.size() && braceCount > 0)
	{
		if (_tokens[pos].getType() == L_CURLY_BRACK)
			braceCount++;
		else if (_tokens[pos].getType() == R_CURLY_BRACK)
			braceCount--;

		pos++;
	}
	return { pos, braceCount };
}


int SyntaxAnalysis::checkConditionStructure(int& pos, bool isForLoop)
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
		if (tokT == SEMICOLUMN && isForLoop) // If it's a statement in a for loop
		{
			break;
		}
		// if its a value to campare to and not a opreand
		if (tokT == INT || tokT == IDENTIFIER || tokT == FLOAT)
		{
			if (currentType != OPERAND)
			{
				throw SyntaxError("Can't give a number after a number in a condition", pos);
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
				throw SyntaxError("Can't give an operand after an operand in a condition", pos);
			}
			currentType = OPERAND;
		}
		pos++;
	}
}


int SyntaxAnalysis::checkDoWhileStructure(int& pos)
{
	std::pair<int, int> bCheck = findMatchingCurlB(pos);
	pos = bCheck.first;
	if (bCheck.second > 0)
	{
		throw SyntaxError("Unmatched '{' in 'do-while' block", pos);
	}
	if (_tokens[pos].getType() != WHILE_LOOP)
	{
		throw SyntaxError("Expected 'while' in 'do-while' block", pos);
	}
	pos++;
	pos++;
	checkConditionStructure(pos);
	if (_tokens[pos].getType() != SEMICOLUMN)
	{
		throw SyntaxError("Expected ';' ", pos);
	}
	return pos;
}


int SyntaxAnalysis::checkIfStructure(int& pos, int chrIndex)
{
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
	if (_tokens[pos].getType() != L_CURLY_BRACK)
	{
		throw SyntaxError("Expected '{' after 'if' condition", chrIndex);
	}
	pos++;
	std::pair<int, int> bCheck = findMatchingCurlB(pos);
	pos = bCheck.first;
	if (bCheck.second > 0)
	{
		throw SyntaxError("Unmatched '{' in 'if' block", chrIndex);
	}
}

int SyntaxAnalysis::checkElseStructure(int& pos, int chrIndex)
{
	pos++; // Move to the next token

	// Ensure the next token is a left curly brace "{"
	if (_tokens[pos].getType() != L_CURLY_BRACK)
	{
		throw SyntaxError("Expected '{' after 'else'", chrIndex);
	}
	pos++;
	std::pair<int, int> bCheck = findMatchingCurlB(pos);
	pos = bCheck.first;
	if (bCheck.second > 0)
	{
		throw SyntaxError("Unmatched '{' in 'else' block", chrIndex);
	}
}


int SyntaxAnalysis::checkForLoopInitialization(int& pos)
{
	// Check initilization
	if (_tokens[pos].getType() == TYPE_DECLERATION)
	{
		pos++;
		if (_tokens[pos].getType() != IDENTIFIER)
		{
			throw SyntaxError("Missing Identifier ", pos);
		}
	}
	else if (_tokens[pos].getType() != IDENTIFIER)
	{
		throw SyntaxError("missing identifier", pos);
	}
	else
	{
		throw SyntaxError("missing identifier", pos);
	}
	pos++;
	if (_tokens[pos].getType() != EQUAL_SIGN)
	{
		throw SyntaxError("missing '='", pos);
	}
	pos++;
	if (_tokens[pos].getType() != INT)
	{
		throw SyntaxError("Expected value for init ", pos);
	}
	pos++;
	if (_tokens[pos].getType() != SEMICOLUMN)
	{
		throw SyntaxError("Expected ';' ", pos);
	}
	return pos;
}

int SyntaxAnalysis::checkStructStructure(int& pos)
{
	std::cout << "Syntax for struct!!" << std::endl;

	pos++;

	if (_tokens[pos].getType() != IDENTIFIER)
	{
		throw SyntaxError("Expected identifier ", pos);
	}

	pos++;
	// Check for struct Allocation
	if (_tokens[pos].getType() == IDENTIFIER && _tokens[pos + 1].getType() == SEMICOLUMN)
	{
		return pos + 2; 
	}
	// Ensure the next token is a left curly brace "{"
	else if (_tokens[pos].getType() != L_CURLY_BRACK)
	{
		throw SyntaxError("Expected '{' after 'struct'", pos);
	}

	pos++;
	std::pair<int, int> bCheck = findMatchingCurlB(pos);
	pos = bCheck.first;
	if (bCheck.second > 0)
	{
		throw SyntaxError("Unmatched '{' in 'structure' block", pos);
	}
	return pos + 1;
}
