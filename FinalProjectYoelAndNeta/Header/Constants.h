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
	CHAR_LITERAL,
	AMPERSEND,
	PTR_TYPE_DELERATION, 
	ADDR_REFERENCE
};

#define RPAREN_LITERAL ')'
#define LPAREN_LITERAL '('
#define ADDITION_LITERAL '+'
#define MULTIPLICATION_LITERAL '*'
#define DIVISION_LITERAL '/'
#define SUBSTRICTION_LITERAL '-'
#define EQUEL_SIGN_LITERAL '='
#define SEMICOLON_LITERAL ';'
#define AMPERSEND_LITERAL '&'

#define SINGLE_QUOTE_LITERAL '\''
#define DOUBLE_QUOTE_LITERAL '"'

// ------ Keywords ------
// -- variable types --
#define	FLOAT "float"
#define INTEGER "int"
#define CHAR "char"


// Other
#define CHAR_DEC_LEN 3