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
	void checkAlgebricStructure();
	bool doesVariebleFitType(const std::string& type, std::string value);

	// structure checking function
	int validSentences(int pos); // a function to check that the code is made from a list of leagel sentences 
	int checkIfStructure(int& pos);
	int variebleDefinitionStructure(int pos);
	int checkConditionStructure(int& pos);

	std::vector<Token> _tokens;
};
