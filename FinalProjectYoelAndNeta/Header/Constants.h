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
	SEMICOLON,
	IDENTIFIER,
	IF_WORD,
	AND,
	OR,
	L_CURLY_BRACK,
	R_CURLY_BRACK,
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
	RETURN_STATEMENT,
	DO_WHILE_LOOP,
	WHILE_LOOP,
	FOR_LOOP,
	INCREMENT,
	DECREMENT,
	STRUCT,
	DOT,
	// YET TO BE SUPPORTED:
	NOT,
	UNARY_MINUS,
	SIZE_OF,
	STRING_LITERAL
};

// Other
#define CHAR_DEC_LEN 3
#define	HEX_VAL 55
#define HEX_BASE 16
#define MIN_ARR_ID_LEN 3

// Variable Sizes
#define INTEGER_AND_FLOAT_SIZE 32
#define CHAR_SIZE 8

#define COMMA_LITERAL ','
#define INT_TYPE_LIT "int"
#define CHAR_TYPE_LIT "char"
#define FLOAT_TYPE_LIT "float"
#define VOID_TYPE_LIT "void"
#define ADD_TMP "addtmp"
#define SUB_TMP "subtmp"
#define MUL_TMP "multmp"
#define CMP_TMP "cmptmp"
#define BOOL_TMP "booltmp"
#define CALL_TMP "calltmp"
#define PRECEDENCE_OR 2
#define PRECEDENCE_AND 3
#define PRECEDENCE_LOWER_THEN 4
#define PRECEDENCE_HIGHER_THEN 5
#define PRECEDENCE_EQUEL_CMP 6
#define PRECEDENCE_ADDITION 20
#define PRECEDENCE_SUBTRACTION 20
#define PRECEDENCE_MULTIPLICATION 30
#define PRECEDENCE_DIVISION 30
#define ANNONIMUS_FUNC_LIT "__anon_expr"
#define EMPTY_STR ""
#define NEW_LINE_CHAR '\n'
#define BLANK ' ' // Empty space
#define DIS_BETWEEN_SINGLE_QOUTES 2
#define NEW_LINE_STR "\n"

#define OBJECT_FILE_LOC "D:/Yoel-Stuff/Maghimim/Final-Project/FinalProjectYoelAndNeta/object.obj"
#define EXE_FILE_LOC "executable.exe"
#define LLD_FILE "ld.lld.exe"


// -- Scopes
#define GLOBAL_SCOPE "-GLOBAL-"
#define WHILE_SCOPE "-while-"
#define FOR_SCOPE "-for-"
#define IF_SCOPE "-if-"
#define ELSE_SCOPE "-else-"
#define ELSEIF_SCOPE "-elseif-" // NOT IMPLEMENTED
#define DOWHILE_SCOPE "-dowhile-"