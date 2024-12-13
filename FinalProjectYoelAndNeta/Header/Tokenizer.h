#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include "Constants.h"
#include "Token.h"
#include "CompilationErrors.h"
#include "Helper.h"

#define BLANK ' ' // Empty space
#define DIS_BETWEEN_SINGLE_QOUTES 2

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