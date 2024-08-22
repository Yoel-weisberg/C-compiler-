#pragma once
#include "Constents.h"
#include <vector>
#include <iostream>
#include <string>
#include "SyntaxError.h"

class SyntexAnalysis
{
public:
	SyntexAnalysis(const std::vector<std::pair<std::string, Tokens>>& tokens);
	
private:
	void checkPernthesis();
	void noTwoOperationAfterEachOther();

	std::vector<std::pair<std::string, Tokens>> tokens;
};
