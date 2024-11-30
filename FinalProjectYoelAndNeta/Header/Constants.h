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
	IDENTIFIER
};

#define RPAREN_LITERAL ')'
#define LPAREN_LITERAL '('
#define ADDITION_LITERAL '+'
#define MULTIPLICATION_LITERAL '*'
#define DIVISION_LITERAL '/'
#define SUBSTRICTION_LITERAL '-'
#define EQUEL_SIGN_LITERAL '='
#define SEMICOLON_LITERAL ';'

// ------ Keywords ------
// -- variable types --
#define	FLOAT "float"
#define INTEGER "int"