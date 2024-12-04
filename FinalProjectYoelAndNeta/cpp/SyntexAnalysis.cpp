#include "SyntexAnalysis.h"
#include <regex>

SyntexAnalysis::SyntexAnalysis(const std::vector<Token>& _tokens) :
	_tokens(_tokens)
{
	checkPernthesis();
	validSentences();
}

void SyntexAnalysis::checkPernthesis()
{
	int LPARENCount = 0;
	int curlyLPRENCount = 0;
	int chrIndex = 0;
	for (auto pr : _tokens)
	{
		chrIndex += pr.getLiteral().size();
		// Check for left parenthesis and increment the counter
		if (pr.getType() == LPAREN)
		{
			LPARENCount++;
		}
		else if(pr.getType() == L_CURLY_PRAN)
		{
			curlyLPRENCount++;
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
		else if (pr.getType() == R_CURLY_PRAN)
		{
			curlyLPRENCount--;

			// If count goes negative, there's an unmatched right parenthesis
			if (curlyLPRENCount < 0)
			{
				throw SyntaxError("Unmatched right curly parenthesis", chrIndex);
			}
		}
	}

	// After loop, if count is positive, there are unmatched left parentheses
	if (LPARENCount > 0)
	{
		throw SyntaxError("Unmatched left parenthesis", chrIndex);
	}
	if (curlyLPRENCount > 0)
	{
		throw SyntaxError("Unmatched left curly parenthesis", chrIndex);
	}
}

void SyntexAnalysis::checkAlgebricStructure()
{
	bool isThereSign = true; // indicates if there is * or / signs in the current session
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

int SyntexAnalysis::variebleDefinitionStructure(int pos)
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
		if (!doesVariebleFitType(_tokens[pos].getLiteral(), _tokens[pos + 3].getLiteral()))
		{
			throw SyntaxError("Varieble type dosent fit decleration");
		}
		pos + 4 >= _tokens.size() ? throw SyntaxError("excepted a semicolumn") : true;
		if (!_tokens[pos + 4].getType() == SEMICOLUMN)
		{
			throw SyntaxError("excepted a semicolumn");
		}
		// advancing pos by 4 to get to the next action
		return pos + 4;
	}
	else if (_tokens[pos + 2].getType() == SEMICOLUMN)
	{
		return pos + 2;
	}
	else
	{
		throw SyntaxError("excepted a semicolumn");
	}
}


bool SyntexAnalysis::doesVariebleFitType(const std::string& type, std::string value)
{
	if (type == "float")
	{
		return Helper::isFloat(value);
	}
}

void SyntexAnalysis::validSentences()
{
	int pos = 0;
	try
	{
		while (pos < _tokens.size())
		{
			if (_tokens[pos].getType() == TYPE_DECLERATION)
			{
				// adding 1 because funtion is returning the last location the sentence ends in
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
