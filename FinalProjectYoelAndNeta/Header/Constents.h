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
	SEMICOLUMN,
	IDENTIFIER,
	IF_WORD,
	AND,
	OR,
	L_CURLY_PRAN,
	R_CURLY_PRAN,
	ELSE
};

#define RPAREN_LITERAL ')'
#define LPAREN_LITERAL '('
#define ADDITION_LITERAL '+'
#define MULTIPLICATION_LITERAL '*'
#define DIVISION_LITERAL '/'
#define SUBSTRICTION_LITERAL '-'
#define EQUEL_SIGN_LITERAL '='
#define SEMICOLUMN_LITERSL ';'
#define AND_LITERAL "&&"
#define OR_ITERAL "||"
#define L_CURLY_PRAN_LITERAL "{"
#define R_CURLY_PRAN_LITERAL "}"