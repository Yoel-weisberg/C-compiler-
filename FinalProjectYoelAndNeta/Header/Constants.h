#pragma once
#include <vector>
#include <iostream>

enum Tokens_type {
	INT,
	LPAREN,
	RPAREN,
	ADDITION,
	MULTIPLICATION,
	DIVISION,
	SUBTRACTION,
	TYPE_DECLERATION,
	EQUEL_SIGN,
	SEMICOLON,
	IDENTIFIER,
	CHAR_LITERAL
};

#define RPAREN_LITERAL ')'
#define LPAREN_LITERAL '('
#define ADDITION_LITERAL '+'
#define MULTIPLICATION_LITERAL '*'
#define DIVISION_LITERAL '/'
#define SUBSTRICTION_LITERAL '-'
#define EQUEL_SIGN_LITERAL '='
#define SEMICOLON_LITERAL ';'

#define SINGLE_QUOTE_LITERAL '\''
#define DOUBLE_QUOTE_LITERAL '"'

// ------ Keywords ------
// -- variable types --
#define	FLOAT "float"
#define INTEGER "int"
#define CHAR "char"


// Other
#define CHAR_DEC_LEN 3