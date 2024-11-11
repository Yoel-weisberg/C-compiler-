#pragma once
#include "Constents.h"
#include <vector>
#include <iostream>
#include <string>
#include "SyntaxError.h"
#include "Tokenizer.h"

class SyntexAnalysis
{
public:
	SyntexAnalysis(const std::vector<Token>& tokens);
	
private:
	void checkPernthesis();
	void noTwoOperationAfterEachOther();

	std::vector<Token> tokens;
};
