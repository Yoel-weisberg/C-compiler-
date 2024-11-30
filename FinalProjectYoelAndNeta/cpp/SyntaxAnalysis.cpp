#include "SyntaxAnalysis.h"
#include <regex>

SyntaxAnalysis::SyntaxAnalysis(const std::vector<Token>& _tokens) :
	_tokens(_tokens)
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

void SyntaxAnalysis::checkAlgebricStructure()
{
	bool isThereSign = true; // indicates if there is "*" or "/" signs in the current session
	int chrIndex = 0;
	bool isTherelPren = false;
	bool isThereANumber = false;
	for (auto pr : _tokens)
	{
		chrIndex += pr.getLiteral().size();
		if (pr.getType() == MULTIPLICATION || pr.getType() == DIVISION)
		{
			if (isThereSign || isTherelPren)
			{
				throw SyntaxError("There are  too many operaters ", chrIndex);
			}

			isThereSign = true;
		}
		else if (pr.getType() == LPAREN)
		{
			isTherelPren = true;
		}
		else if (pr.getType() == RPAREN)
		{
			if (!isThereANumber)  throw SyntaxError("There is nothing in the pernthesis", chrIndex);
			isTherelPren = false;
		}
		else if (pr.getType() == INT)
		{
			isThereSign = false;
			isThereANumber = true;
			isTherelPren = false;
		}
		else if (pr.getType() == ADDITION || pr.getType() == SUBTRACTION)
		{
			if (isTherelPren)
			{
				throw SyntaxError("There are  too many operaters ", chrIndex);
			}
			isThereSign = true;
		}
	}
	if (!isThereANumber)
	{
		throw SyntaxError("There is no integer in the expression", chrIndex);
	}
}

int SyntaxAnalysis::variebleDefinitionStructure(int pos)
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
	if (pos + 2 >= _tokens.size())
	{
		throw SyntaxError("unexpected end of input");
	}
	if (_tokens[pos + 2].getType() == EQUEL_SIGN)
	{
		// Check for additional tokens needed for this branch
		if (pos + 3 >= _tokens.size())
		{
			throw SyntaxError("missing value after equal sign");
		}
		if (!doesVariableFitType(_tokens[pos].getLiteral(), _tokens[pos + 3].getLiteral()))
		{
			throw SyntaxError("Variable type dosent fit decleration");
		}
		pos + 4 >= _tokens.size() ? throw SyntaxError("excepted a semicolon") : true;
		if (!_tokens[pos + 4].getType() == SEMICOLON)
		{
			throw SyntaxError("excepted a semicolon");
		}
		// advancing pos by 4 to get to the next action
		return pos + 4;
	}
	else if (_tokens[pos + 2].getType() == SEMICOLON)
	{
		return pos + 2;
	}
	else
	{
		throw SyntaxError("excepted a semicolon");
	}
}


bool SyntaxAnalysis::doesVariableFitType(const std::string& type, std::string value)
{
	if (type == FLOAT)
	{
		return Helper::isFloat(value);
	}
	else if (type == INTEGER)
	{
		return Helper::isInteger(value);
	}
}

void SyntaxAnalysis::validSentences()
{
	int pos = 0;
	try
	{
		while (pos < _tokens.size())
		{
			if (_tokens[pos].getType() == TYPE_DECLERATION)
			{
				// adding 1 because function is returning the last location the sentence ends in
				pos = variebleDefinitionStructure(pos) + 1;
			}
			// T0DO - checking if the sentence just a defined identifier (like just 3; or somthing like that)
			// TODO - need to check if its a redefinition of t a symbol 
			// TODO - need to check if the sentnce is an algebric sentnece 
			// there is no suppert for other sentence structure
			else
			{
				throw SyntaxError("Not defined sentence", pos);
			}
		}
	}
	catch (const std::out_of_range& e)
	{
		throw SyntaxError("Got to end of token stream without a semicolumn");
	}
}
