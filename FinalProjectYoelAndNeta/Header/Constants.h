#pragma once
#include <vector>
#include <iostream>

enum Tokens_type {
	INT,
	FLOAT,
	LPAREN,
	RPAREN,
	ADDITION,
	MULTIPLICATION,
	DIVISION,
	SUBTRACTION,
	TYPE_DECLERATION,
	EQUAL_SIGN,
	SEMICOLUMN,
	IDENTIFIER,
	IF_WORD,
	AND,
	OR,
	L_CURLY_PRAN,
	R_CURLY_PRAN,
	ELSE, 
	LOWER_THEN,
	HIGHER_THEN,
	EQUELS_CMP,
	COMMA,
	PTR_TYPE_DECLERATION, 
	ADDR_REFERENCE,
	ARR_TYPE_DECLERATION,
	AMPERSAND,
	SQR_BR_L,
	SQR_BR_R,
	RETURN_STATEMENT
};

// Other
#define CHAR_DEC_LEN 3
#define	HEX_VAL 55
#define HEX_BASE 16

// Variable Sizes
#define INTEGER_AND_FLOAT_SIZE 32
#define CHAR_SIZE 8
