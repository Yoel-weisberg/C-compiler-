#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cctype>

enum Tokens {
	INT, 
	LPREN,
	RPREN,
	ADDITION,
	MULTIPLICATION,
	DIVISION,
	SUBSTRACTION
};

#define RPREN_LITERAL ')'
#define LPREN_LITERAL '('
#define ADDITION_LITERAL '+'
#define MULTIPLICATION_LITERAL '*'
#define DIVISION_LITERAL '/'
#define SUBSTRICTION_LITERAL '-'

class Tokeniser {
public:
	Tokeniser(const std::string& string);
	std::vector<std::pair<std::string, Tokens>> getTokens() const;
	friend std::ostream& operator<<(std::ostream& os, const Tokeniser& tokeniser);

private:
	std::vector<std::pair<std::string, Tokens>> tokens;
	Tokens categoriseLiteral(const std::string& literal);
	bool isNumber(const std::string& literal);
};