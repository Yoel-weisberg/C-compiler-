#include "../Header/Parser.h"


#include "ExprAST.h"  // Include the AST classes (or wherever they are defined)
#include <iostream>
#include <stdexcept>

// Constructor for Parser
Parser::Parser(const std::vector<Token>& tokens)
	: tokens(tokens), currentTokenIndex(0) {
	head =  parse();
}

ExprAST* Parser::getAst()
{
	return head.get();
}

// Current token getter
Token& Parser::currentToken() {
	return tokens[currentTokenIndex];
}

// Move to the next token
void Parser::consume() {
	currentTokenIndex++;
}

// Check if we've reached the end of the token vector
bool Parser::isAtEnd() {
	return currentTokenIndex >= tokens.size();
}

// Main entry point to parse tokens and build an AST
std::unique_ptr<ExprAST> Parser::parse() {
	return ParsePrimary();
}

// Parse assignment statements (e.g., "int a = 5;")
std::unique_ptr<ExprAST> Parser::parseAssignment() {
	if (currentToken().getType() == Tokens_type::TYPE_DECLERATION) {
		std::string type = currentToken().getLiteral();
		consume();

		std::string varName = currentToken().getLiteral();
		consume(); // Move past IDENTIFIER

		if (currentToken().getType() == Tokens_type::EQUEL_SIGN) {
			consume(); // Move past '='
			auto rhs = std::make_unique<FloatNumberExprAST>(std::stod(currentToken().getLiteral()));
			consume(); // Move past the number
			return std::make_unique<AssignExprAST>(varName, std::move(rhs), type);
		}
	}
	return nullptr;
}

std::unique_ptr<ExprAST> Parser::parseIfStatement() {
	consume(); // eat the if.
	std::unique_ptr<ExprAST> Else = nullptr;
	// condition.
	auto Cond = ParseExpression();
	if (!Cond)
		return nullptr;

	
	consume(); // eat the {

	auto Then = ParseExpression();
	if (!Then)
		return nullptr;

	if (currentToken().getType() == ELSE)
		Else = ParseExpression();

	consume();

	return std::make_unique<IfExprAST>(std::move(Cond), std::move(Then),
		std::move(Else));

}

std::unique_ptr<ExprAST> Parser::ParseFloatNumberExpr()
{
	auto Result = std::make_unique<FloatNumberExprAST>(std::stoi(currentToken().getLiteral()));
	consume();
	return std::move(Result);
}

std::unique_ptr<ExprAST> Parser::ParseParenExpr()
{
	consume(); // eat (.
	auto V = ParseExpression();
	if (!V)
		return nullptr;

	consume(); // eat ).
	return V;
}

//Parse general expressions (addition, subtraction, etc.)
std::unique_ptr<ExprAST> Parser::ParseExpression() {
	auto LHS = ParsePrimary();
	if (!LHS)
		return nullptr;

	return ParseBinOpRHS(0, std::move(LHS));
}


// this is like the sentence switch but for the parser
std::unique_ptr<ExprAST> Parser::ParsePrimary()
{
	switch (currentToken().getType())
	{
	case TYPE_DECLERATION:
		return parseAssignment();
	case LPAREN:
		return ParseParenExpr();
	case IF_WORD:
		return parseIfStatement();
	case FLOAT:
		return ParseFloatNumberExpr();
	default:
		break;
	}
}


std::unique_ptr<PrototypeAST> Parser::ParsePrototype()
{
	std::string FnName = IdentifierStr;
	consume();

	std::vector<std::string> ArgNames;
	consume();
	while (currentToken().getType() == IDENTIFIER)
	{
		ArgNames.push_back(IdentifierStr);
		consume();
	}
		
	// success.
	consume(); // eat ')'.

	return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames));
}

std::unique_ptr<FunctionAST> Parser::ParseTopLevelExpr()
{
	if (auto E = ParseExpression()) {
		// Make an anonymous proto.
		auto Proto = std::make_unique<PrototypeAST>("__anon_expr",
			std::vector<std::string>());
		return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
	}
	return nullptr;
}