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
	//void checkAlgebricStructure(); // No use!
	bool doesVariableFitType(const std::string& type, std::string value);
	void validSentences(); // a function to check that the code is made from a list of legal sentences
	// Definitoin checkeing
	int ptrVariableDefenitionStructure(int pos);
	int variableDefinitionStructure(int pos);
	int arrTypeVariableDefinitionStructure(int pos);

	int checkFlowControlStructure(int& pos);
	int checkLoopStructure(int& pos);

	// Control Flow checking (if, else...)
	int checkIfStructure(int& pos, int chrIndex);
	int checkElseStructure(int& pos, int chrIndex);

	// Loop checking
	int checkForLoopStructure(int& pos);
	int checkWhileLoopStructure(int& pos);
	int checkDoWhileStructure(int& pos);
	int checkForLoopInitialization(int& pos);


	// Helper methods
	std::pair<int, int> findMatchingCurlB(int& pos);
	int checkConditionStructure(int& pos, bool ifForLoop = false);
};
