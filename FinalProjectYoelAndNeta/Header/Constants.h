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
	ARR_TYPE_DECLERATION,
	CURL_BR,

};

// LIT is short for "LITERAL"
#define RPAREN_LIT ')'
#define LPAREN_LIT '('
#define ADDITION_LIT '+'
#define MULTIPLICATION_LIT '*'
#define DIVISION_LIT '/'
#define SUBSTRICTION_LIT '-'
#define EQUAL_SIGN_LIT '='
#define SEMICOLON_LIT ';'
#define AMPERSEND_LIT '&'
#define SQUARE_BR_L_LIT '['
#define SQUARE_BR_R_LIT ']'
#define CURL_BR_L_LIT '{'
#define CURL_BR_R_LIT '}'

#define SINGLE_QUOTE_LITERAL '\''
#define DOUBLE_QUOTE_LITERAL '"'
#define	COMMA_LIT ','
#define ARR_INIT_LIT "[]"

// ------ Keywords ------
// -- variable types --
#define FLOAT "float"
#define INTEGER "int"
#define CHAR "char"
#define POINTER "ptr"
#define ARRAY "arr"


// Other
#define CHAR_DEC_LEN 3
#define	HEX_VAL 55
#define HEX_BASE 16

// Variable Sizes
#define INTEGER_AND_FLOAT_SIZE 32
#define CHAR_SIZE 8
