#include "../Header/Parser.h"


#include "ExprAST.h"  // Include the AST classes (or wherever they are defined)
#include <iostream>
#include <stdexcept>

int Parser::getTokenPrecedence()
{
	//if (!isascii(currentToken().getLiteral())
	//	return -1;

	//// Make sure it's a declared binop.
	//int TokPrec = _BinopPrecedence[CurTok];
	//if (TokPrec <= 0)
	//	return -1;
	//return TokPrec;
	if (_BinopPrecedence.find(currentToken().getType()) != _BinopPrecedence.end())
	{
		return _BinopPrecedence[currentToken().getType()];
	}
	else
	{
		return -1;
	}
}

// Constructor for Parser
Parser::Parser(const std::vector<Token>& tokens)
	: _tokens(tokens), _currentTokenIndex(0) {

	// comparing operaters
	_BinopPrecedence[OR] = PRECEDENCE_OR;
	_BinopPrecedence[AND] = PRECEDENCE_AND;
	_BinopPrecedence[LOWER_THEN] = PRECEDENCE_LOWER_THEN;
	_BinopPrecedence[HIGHER_THEN] = PRECEDENCE_HIGHER_THEN;
	_BinopPrecedence[ADDITION] = PRECEDENCE_ADDITION;
	_BinopPrecedence[SUBTRACTION] = PRECEDENCE_SUBTRACTION;
	_BinopPrecedence[MULTIPLICATION] = PRECEDENCE_MULTIPLICATION;
	_BinopPrecedence[DIVISION] = PRECEDENCE_DIVISION;
}

ExprAST* Parser::getAst()
{
	return _head.get();
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
	return _tokens[_currentTokenIndex];
}

// Move to the next token
void Parser::consume(int times) {
	_currentTokenIndex += times;
}

// Check if we've reached the end of the token vector
bool Parser::isAtEnd() {
	return _currentTokenIndex >= _tokens.size();
}

// Main entry point to parse tokens and build an AST
std::unique_ptr<ExprAST> Parser::parse() {
	return ParsePrimary();
}


std::unique_ptr<ExprAST> Parser::parseAssignment() {
	if (currentToken().getType() == TYPE_DECLERATION && currentToken().getType() == MULTIPLICATION)
	{
		return ptrAssignmentParsing();
	}
	if (currentToken().getType() == TYPE_DECLERATION) // Indicates both regular and array type assignment
	{
		return regularAssignmentParsing();
	}
	return nullptr;
}

// Parse pointer assignment statement (e.g "int* ptr = &point_to;")
std::unique_ptr<ExprAST> Parser::ptrAssignmentParsing()
{
	try
	{
		// Regular parsing
		std::string type = Helper::removeSpecialCharacter(currentToken().getLiteral());
		consume();

		std::string var_name = currentToken().getLiteral();
		consume(); // Skip equal sign
		consume(); 

		std::string pointedToVar = Helper::removeSpecialCharacter(currentToken().getLiteral());
		auto PTVit = Helper::NamedValues.find(pointedToVar);
		if (Helper::NamedValues.end() == PTVit)
		{
			throw ParserError("can't point to non-existing variable --> " + pointedToVar);
		}
		llvm::AllocaInst* allocInst = Helper::NamedValues[pointedToVar];

		Helper::addSymbol(var_name, type);

		// Return
		auto value_literal = std::make_unique<ptrExprAST>(pointedToVar);

		consume();

		return std::make_unique<AssignExprAST>(var_name, std::move(value_literal), type);

	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << std::endl;
	}
}

// Parse Basic assignment statements (e.g., "int a = 5;")
std::unique_ptr<ExprAST> Parser::regularAssignmentParsing()
{

	std::string type = currentToken().getLiteral();
	consume();

	// Check for arrays 
	if (currentToken().getLiteral().substr(currentToken().getLiteral().size() - 2, currentToken().getLiteral().size()) == "[]")
	{
		return arrAssignmentParsing(Helper::removeSpecialCharacter(type));
	}

	std::string varName = currentToken().getLiteral();
	consume(); // Move past IDENTIFIER

	if (currentToken().getType() == EQUAL_SIGN) {
		consume(); // Move past '='
		//Helper::addSymbol(varName, type, currentToken().getLiteral());
		if (type == FLOAT_TYPE_LIT )
		{
			auto value_literal = std::make_unique<FloatNumberExprAST>(std::stod(currentToken().getLiteral()));
			consume(); // Move past value
			consume();
			return std::make_unique<AssignExprAST>(varName, std::move(value_literal), type);
		}
		else if (type == INT_TYPE_LIT )
		{
			auto value_literal = std::make_unique<IntegerNumberExprAST>(std::stod(currentToken().getLiteral()));
			consume(); // Move past value
			consume();
			return std::make_unique<AssignExprAST>(varName, std::move(value_literal), type);
		}
		else if (type == CHAR_TYPE_LIT)
		{
			auto value_literal = std::make_unique<CharExprAST>(std::stod(currentToken().getLiteral()));
			consume(); // Move past value
			consume();
			return std::make_unique<AssignExprAST>(varName, std::move(value_literal), type);
		}
	}
	return nullptr;
}

std::unique_ptr<ExprAST> Parser::arrAssignmentParsing(const std::string& type)
{
	std::string currVal = EMPTY_STR;
	int len = 0; // Length of the created array
	std::string varName = currentToken().getLiteral().substr(0, currentToken().getLiteral().size() - 2); // Cut out '[]'
	consume();
	consume(); // Move past '='
	consume(); // Move part '{'
	std::string init = currentToken().getLiteral();
	try
	{
		// Guide to the solution --> https://stackoverflow.com/questions/7844049/how-are-c-arrays-represented-in-memory	
		if (init[0] == COMMA_LITERAL)
		{
			throw ParserError("Value missing");
		}

		for (int i = 0; i < init.size(); i++)
		{
			if (init[i] == COMMA_LITERAL)
			{
				throw ParserError("Empty initilization");
			}
			while (init[i] != COMMA_LITERAL && (i != init.size()))
			{
				currVal += init[i];
				i++;
			}
			if (type == INT_TYPE_LIT )
			{
				if (!Helper::isInteger(currVal))
				{
					throw ParserError("Invalid type, supposd to be " + type);
				}
			}
			else if (type == FLOAT_TYPE_LIT )
			{
				if (!Helper::isFloat(currVal))
				{
					throw ParserError("Invalid type, supposd to be " + type);
				}
			}
			else if (type == CHAR_TYPE_LIT)
			{
				if (!Helper::isChar(currVal))
				{
					throw ParserError("Invalid type, supposd to be " + type);
				}
			}
			currVal = EMPTY_STR;
			len++;
		}
		consume();
		consume();
		Helper::addSymbol(varName, "arr", type, len);
		std::string convertalbleLen = std::to_string(len);
		auto value_literal = std::make_unique<arrExprAST>(type, convertalbleLen, init, varName);
		return std::make_unique<AssignExprAST>(varName, std::move(value_literal), type);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

bool Parser::isFinished() const
{
	return _currentTokenIndex == this->_tokens.size();
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
	
	consume(); // eat the }

	if (currentToken().getType() == ELSE)
		Else = ParseExpression();


	return std::make_unique<IfExprAST>(std::move(Cond), std::move(Then),
		std::move(Else));

}

std::unique_ptr<ExprAST> Parser::ParseFloatNumberExpr()
{
	auto Result = std::make_unique<FloatNumberExprAST>(std::stof(currentToken().getLiteral()));
	consume();
	if (currentToken().getType() == SEMICOLUMN) consume();
	return std::move(Result);
}

std::unique_ptr<ExprAST> Parser::ParseIntagerNumberExpr()
{
	auto Result = std::make_unique<IntegerNumberExprAST>(std::stof(currentToken().getLiteral()));
	consume();
	return std::move(Result);
}

std::unique_ptr<ExprAST> Parser::ParseIdentifierExpr()
{
	std::string IdName = currentToken().getLiteral();

	consume(); // eat identifier.

	if (currentToken().getType() != LPAREN) // Simple variable ref.
	{
		if (currentToken().getType() == SEMICOLUMN) consume();
		return std::make_unique<VariableExprAST>(IdName);
	}

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
	if (currentToken().getType() == SEMICOLUMN)
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
	case RETURN_STATEMENT:
		return nullptr;
	case INT:
		return ParseIntagerNumberExpr();
	default:
		throw SyntaxError("Undefined Sentence begining");
	}
}


std::unique_ptr<PrototypeAST> Parser::ParsePrototype()
{
	// were starting on the type
	std::string returnType = currentToken().getLiteral();
	consume();
	std::string FnName = currentToken().getLiteral();
	consume(2);
	// consume the (
	std::vector<FuncArg> argsTypesAndNames;
	while (currentToken().getType() == TYPE_DECLERATION)
	{
		std::string type = currentToken().getLiteral();
		consume();
		std::string name = currentToken().getLiteral();
		consume();
		if (currentToken().getType() == COMMA)
			consume();
		
		argsTypesAndNames.push_back({ type, name });
	}
		
	// success.
	consume(2); // eat ') and { or ;
	return std::make_unique<PrototypeAST>(FnName, std::move(argsTypesAndNames), returnType);
}

std::unique_ptr<FunctionAST> Parser::ParseDefinition()
{
	auto Proto = ParsePrototype();
	if (!Proto)
		return nullptr;
	std::string type = Proto.get()->getReturnType();

	if (auto E = ParseExpression())
	{
		if (currentToken().getType() == RETURN_STATEMENT)
		{
			consume();
			auto returnStatement = ParseExpression();
			
			return std::make_unique<FunctionAST>(std::move(Proto), std::move(E), std::move(returnStatement), type);
		}
		else if (Proto.get()->getReturnType() == VOID_TYPE_LIT)
		{
			// it dosent realy matter 
			auto voidRet = ParseVoid();
			return std::make_unique<FunctionAST>(std::move(Proto), std::move(E), std::move(voidRet), type);
		}
		else
		{
			throw SyntaxError("Excepted a return statement");
		}

	}
	return nullptr;
}

std::unique_ptr<ExprAST> Parser::ParseVoid()
{
	return std::make_unique<VoidAst>();
}

std::unique_ptr<FunctionAST> Parser::ParseTopLevelExpr()
{
	if (auto E = ParseExpression()) {
		// Make an anonymous proto.
		auto Proto = std::make_unique<PrototypeAST>(ANNONIMUS_FUNC_LIT,
			std::vector<FuncArg>(),
			VOID_TYPE_LIT);
		return std::make_unique<FunctionAST>(std::move(Proto), std::move(E), nullptr, VOID_TYPE_LIT);
	}
	return nullptr;
}