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
	EQUAL_SIGN,
	SEMICOLON,
	IDENTIFIER,
	CHAR_LITERAL,
	AMPERSEND,
	PTR_TYPE_DECLERATION, 
	ADDR_REFERENCE,
	ARR_TYPE_DECLERATION
};

#define RPAREN_LITERAL ')'
#define LPAREN_LITERAL '('
#define ADDITION_LITERAL '+'
#define MULTIPLICATION_LITERAL '*'
#define DIVISION_LITERAL '/'
#define SUBSTRICTION_LITERAL '-'
#define EQUAL_SIGN_LITERAL '='
#define SEMICOLON_LITERAL ';'
#define AMPERSEND_LITERAL '&'
#define SQUARE_BRACK_L '['
#define SQUARE_BRACK_R ']'

#define SINGLE_QUOTE_LITERAL '\''
#define DOUBLE_QUOTE_LITERAL '"'

// ------ Keywords ------
// -- variable types --
#define FLOAT "float"
#define INTEGER "int"
#define CHAR "char"
#define POINTER "ptr"


// Other
#define CHAR_DEC_LEN 3
#define	HEX_VAL 55
#define HEX_BASE 16

// Variable Sizes
#define INTEGER_AND_FLOAT_SIZE 32
#define CHAR_SIZE 8
