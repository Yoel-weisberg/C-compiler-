#include "SyntexAnalysis.h"
#include <regex>

SyntexAnalysis::SyntexAnalysis(const std::vector<Token>& _tokens) :
	_tokens(_tokens)
{
	checkPernthesis();
	noTwoOperationAfterEachOther();
}

void SyntexAnalysis::checkPernthesis()
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

void SyntexAnalysis::noTwoOperationAfterEachOther()
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
	if (_tokens[pos + 1].getType() != IDENTIFIER)
	{
		throw SyntaxError("missing Identifier");
	}
	if (!Helper::checkIdentifier(_tokens[pos + 1].getLiteral()))
	{
		throw SyntaxError("Identifier not valid");
	}
	if (_tokens[pos + 2].getType() == EQUEL_SIGN)
	{
		if (!doesVariebleFitType(_tokens[pos].getLiteral(), _tokens[pos + 3].getLiteral()))
		{
			throw SyntaxError("Varieble type dosent fit decleration");
		}
		else if (!_tokens[pos + 4].getType() == SEMICOLUMN)
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
		std::regex decimalPattern(R"(^[-+]?[0-9]*\.[0-9]+$)");
		return std::regex_match(value, decimalPattern);

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
				pos = variebleDefinitionStructure(pos);
			}
		}
	}
	catch (const std::out_of_range& e)
	{
		throw SyntaxError("Got to end of token stream without a semicolumn");
	}
}
