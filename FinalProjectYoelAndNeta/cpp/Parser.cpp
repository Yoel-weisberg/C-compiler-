#include "../Header/Parser.h"


#include "ExprAST.h"  // Include the AST classes (or wherever they are defined)
#include <iostream>
#include <stdexcept>

// Constructor for Parser
Parser::Parser(const std::vector<Token>& tokens)
	: _tokens(tokens), _currentTokenIndex(0) {
	_head =  parse();
}

ExprAST* Parser::getAst()
{
	return _head.get();
}

// Current token getter
Token& Parser::currentToken() {
	return _tokens[_currentTokenIndex];
}

// Move to the next token
void Parser::consume() {
	_currentTokenIndex++;
}

// Check if we've reached the end of the token vector
bool Parser::isAtEnd() {
	return _currentTokenIndex >= _tokens.size();
}

// Main entry point to parse tokens and build an AST
std::unique_ptr<ExprAST> Parser::parse() {
	return parseAssignment(); // This is the only possibility for now
}

// Parse assignment statements (e.g., "int a = 5;")
std::unique_ptr<ExprAST> Parser::parseAssignment() {
	if (currentToken().getType() == Tokens_type::TYPE_DECLERATION) {
		std::string type = currentToken().getLiteral();
		consume();

		std::string varName = currentToken().getLiteral();
		consume(); // Move past IDENTIFIER

		if (currentToken().getType() == Tokens_type::EQUAL_SIGN) {
			consume(); // Move past '='
			if (type == FLOAT)
			{
				auto value_literal = std::make_unique<FloatNumberExprAST>(std::stod(currentToken().getLiteral()));
				consume(); // Move past value
				return std::make_unique<AssignExprAST>(varName, std::move(value_literal), type);
			}
			if (type == INTEGER)
			{
				auto value_literal = std::make_unique<IntegerNumberExprAST>(std::stod(currentToken().getLiteral()));
				consume(); // Move past value
				return std::make_unique<AssignExprAST>(varName, std::move(value_literal), type);
			}
			if (type == CHAR)
			{
				auto value_literal = std::make_unique<CharExprAST>(currentToken().getLiteral()[1]);
				consume(); // Move past value
				return std::make_unique<AssignExprAST>(varName, std::move(value_literal), type);
			}

		}
	}
	if (currentToken().getType() == PTR_TYPE_DELERATION)
	{
		return ptrParseAssignment();
	}
	return nullptr;
}
std::unique_ptr<ExprAST> Parser::ptrParseAssignment()
{
	return std::unique_ptr<ExprAST>();
}
