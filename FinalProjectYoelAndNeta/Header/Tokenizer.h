#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include "Constants.h"
#include "Token.h"
#include "SyntexError.h"
#include "Helper.h"

#define BLANK ' ' // Empty space

class Tokeniser {
public:
	Tokeniser(const std::string& raw_code_str);
	std::vector<Token> getTokens() const;
	friend std::ostream& operator<<(std::ostream& os, const Tokeniser& tokeniser);

private:
	std::vector<Token> _tokens;
	Tokens_type categoriseLiteral(const std::string& literal);
	bool isNumber(const std::string& literal);
};