#include "../Header/Parser.h"


#include "ExprAST.h"  // Include the AST classes (or wherever they are defined)
#include <iostream>
#include <stdexcept>

int Parser::getTokenPrecedence()
{
	//if (!isascii(currentToken().getLiteral())
	//	return -1;

	//// Make sure it's a declared binop.
	//int TokPrec = BinopPrecedence[CurTok];
	//if (TokPrec <= 0)
	//	return -1;
	//return TokPrec;
	if (BinopPrecedence.find(currentToken().getType()) != BinopPrecedence.end())
	{
		return BinopPrecedence[currentToken().getType()];
	}
	else
	{
		return -1;
	}
}

// Constructor for Parser
Parser::Parser(const std::vector<Token>& tokens)
	: tokens(tokens), currentTokenIndex(0) {

	// comparing operaters
	BinopPrecedence[OR] = 2;
	BinopPrecedence[AND] = 3;
	BinopPrecedence[LOWER_THEN] = 4;
	BinopPrecedence[HIGHER_THEN] = 5;
	BinopPrecedence[ADDITION] = 20;
	BinopPrecedence[SUBTRACTION] = 20;
	BinopPrecedence[MULTIPLICATION] = 30;
	BinopPrecedence[DIVISION] = 30;
}

ExprAST* Parser::getAst()
{
	return head.get();
}

std::unique_ptr<ExprAST> Parser::ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS)
{
	// If this is a binop, find its precedence.
	while (true) {
		int TokPrec = getTokenPrecedence();

		// If this is a binop that binds at least as tightly as the current binop,
		// consume it, otherwise we are done.
		if (TokPrec < ExprPrec)
			return LHS;

		// Okay, we know this is a binop.
		Tokens_type BinOp = currentToken().getType();
		consume(); // eat binop

		// Parse the primary expression after the binary operator.
		auto RHS = ParsePrimary();
		if (!RHS)
			return nullptr;

		// If BinOp binds less tightly with RHS than the operator after RHS, let
		// the pending operator take RHS as its LHS.
		int NextPrec = getTokenPrecedence();
		if (TokPrec < NextPrec) {
			RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));
			if (!RHS)
				return nullptr;
		}

		// Merge LHS/RHS.
		LHS = std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
	}
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
			consume(); // move past the semicolomn
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

std::unique_ptr<ExprAST> Parser::ParseIdentifierExpr()
{
	std::string IdName = IdentifierStr;

	consume(); // eat identifier.

	if (currentToken().getType() != LPAREN) // Simple variable ref.
		return std::make_unique<VariableExprAST>(IdName);

	// Call.
	consume(); // eat (
	std::vector<std::unique_ptr<ExprAST>> Args;
	if (currentToken().getType() != RPAREN) {
		while (true) {
			if (auto Arg = ParseExpression())
				Args.push_back(std::move(Arg));
			else
				return nullptr;

			if (currentToken().getType() == RPAREN)
				break;

			consume();
		}
	}

	// Eat the ')'.
	consume();

	return std::make_unique<CallExprAST>(IdName, std::move(Args));
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
		return this->parseAssignment();
	case LPAREN:
		return ParseParenExpr();
	case IF_WORD:
		return parseIfStatement();
	case FLOAT:
		return ParseFloatNumberExpr();
	case IDENTIFIER:
		return ParseIdentifierExpr();
	default:
		throw SyntaxError("Undefined Sentence begining");
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