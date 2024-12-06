#pragma once
#include "Constants.h"

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
	Tokens_type getType() { return _type; }

	bool operator==(Token const& tkn) // Used in find command at parser C'tor
	{
		return _literal == tkn._literal && _type == tkn._type;
	}
};