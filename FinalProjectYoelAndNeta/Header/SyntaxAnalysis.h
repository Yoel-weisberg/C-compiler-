#pragma once
#include "Constants.h"
#include <vector>
#include <iostream>
#include <string>
#include "CompilationErrors.h"
#include "Tokenizer.h"
#include "Helper.h"

#define MIN_NUM_OF_OPERATIONS 2
#define MAX_NUM_OF_OPERATIONS_FOR_DECLERATION 4

class SyntaxAnalysis
{
public:
	SyntaxAnalysis(const std::vector<Token>& tokens);

private:

	std::vector<Token> _tokens;

	void checkParentheses();
	void checkAlgebricStructure(); // No use!
	int variableDefinitionStructure( int pos);
	bool doesVariableFitType(const std::string& type, std::string value);
	void validSentences(); // a function to check that the code is made from a list of leagel sentences 
	int ptrVariableDefenitionStructure(int pos);
};
