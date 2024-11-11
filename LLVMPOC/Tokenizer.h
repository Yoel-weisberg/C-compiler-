#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include "Constents.h"


class Token {
private:
	std::string _literal;
	Tokens_type _type;
public:

	Token(const std::string& _literal, const Tokens_type& _type)
		: _literal(_literal), _type(_type)
	{
	}
	std::string getLiteral() { return _literal; }
	Tokens_type getType() { return _type;  }
};

class Tokeniser {
public:
	Tokeniser(const std::string& string);
	std::vector<Token> getTokens() const;
	friend std::ostream& operator<<(std::ostream& os, const Tokeniser& tokeniser);

private:
	std::vector<Token> tokens;
	Token makeToken(const std::string& literal);
	bool isNumber(const std::string& literal);
};