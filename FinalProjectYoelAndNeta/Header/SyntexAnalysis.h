#pragma once
#include "Constents.h"
#include <vector>
#include <iostream>
#include <string>
#include "SyntexError.h"
#include "Tokenizer.h"
#include "Helper.h"

class SyntexAnalysis
{
public:
	SyntexAnalysis(const std::vector<Token>& tokens);

private:
	void checkPernthesis();
	void noTwoOperationAfterEachOther();
	int variebleDefinitionStructure( int pos);
	bool doesVariebleFitType(const std::string& type, std::string value);
	void validSentences(); // a function to check that the code is made from a list of leagel sentences 
	std::vector<Token> _tokens;
};
