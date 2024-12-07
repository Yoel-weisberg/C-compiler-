#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include "Constents.h"
#include "Token.h"
#include "SyntexError.h"
#include "Helper.h"

class Tokeniser {
public:
	Tokeniser(const std::string& string);
	std::vector<Token> getTokens() const;
	friend std::ostream& operator<<(std::ostream& os, const Tokeniser& tokeniser);

private:
	std::vector<Token> tokens;
	Tokens_type categoriseLiteral(const std::string& literal);
	bool isNumber(const std::string& literal);
	bool isFloat(const std::string& literal);
};