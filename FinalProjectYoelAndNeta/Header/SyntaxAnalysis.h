#pragma once
#include "Constants.h"
#include <vector>
#include <iostream>
#include <string>
#include "SyntaxError.h"
#include "Tokenizer.h"
#include "Helper.h"

class SyntaxAnalysis
{
public:
	SyntaxAnalysis(const std::vector<Token>& tokens);

private:
	void checkParentheses();
	void checkAlgebricStructure();
	int variebleDefinitionStructure( int pos);
	bool doesVariableFitType(const std::string& type, std::string value);
	void validSentences(); // a function to check that the code is made from a list of leagel sentences 
	std::vector<Token> _tokens;
};
