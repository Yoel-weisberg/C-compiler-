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
	: _tokens(tokens), _currentTokenIndex(0) 
{

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

bool Parser::isUnaryOp()
{
	if (currentToken().getType() == ADDITION) // ex: '++'
	{
		consume();
		return currentToken().getType() == ADDITION ? true : false;
	}
	else if (currentToken().getType() == SUBTRACTION) // ex: '--'
	{
		consume();
		return currentToken().getType() == SUBTRACTION ? true : false;
	}
	return false;
}

std::unique_ptr<ExprAST> Parser::parseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS)
{
	// If this is a binop, find its precedence.
	while (true) 
	{
		int TokPrec = getTokenPrecedence();

		// If this is a binop that binds at least as tightly as the current binop,
		// consume it, otherwise we are done.
		if (TokPrec < ExprPrec)
			return LHS;

		// Okay, we know this is a binop.
		Tokens_type BinOp = currentToken().getType();
		consume(); // eat binop

		// Parse the primary expression after the binary operator.
		auto RHS = parsePrimary();
		if (!RHS)
			return nullptr;

		// If BinOp binds less tightly with RHS than the operator after RHS, let
		// the pending operator take RHS as its LHS.
		int NextPrec = getTokenPrecedence();
		if (TokPrec < NextPrec) {
			RHS = parseBinOpRHS(TokPrec + 1, std::move(RHS));
			if (!RHS)
				return nullptr;
		}

		// Merge LHS/RHS.
		LHS = std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
	}
}

std::unique_ptr<ExprAST> Parser::parseUnaryOp(std::unique_ptr<ExprAST> LHS)
{
	Tokens_type op = currentToken().getType();
	return std::make_unique<UnaryOpExprAST>(op, std::move(LHS));
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
	return parsePrimary();
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
		auto PTVit = Helper::SymbolTable.find(pointedToVar);
		if (Helper::SymbolTable.end() == PTVit)
		{
			throw ParserError("can't point to non-existing variable --> " + pointedToVar);
		}
		llvm::AllocaInst* allocInst = Helper::SymbolTable[pointedToVar];

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
	if (currentToken().getLiteral().size() >= MIN_ARR_ID_LEN && currentToken().getLiteral().substr(currentToken().getLiteral().size() - 2, currentToken().getLiteral().size()) == "[]")
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
	auto Cond = parseExpression();
	if (!Cond)
	{
		return nullptr;
	}

	
	consume(); // eat the {

	// TODO: construct 'Then' with multiple expressions
	auto Then = parseExpression();
	if (!Then)
	{
		return nullptr;
	}
	
	consume(); 

	if (currentToken().getType() == ELSE)
	{
		Else = parseExpression();
	}

	consume(3); // Get to next expression

	return std::make_unique<IfExprAST>(std::move(Cond), std::move(Then),
		std::move(Else));

}

std::unique_ptr<ExprAST> Parser::parseFloatNumberExpr()
{
	auto Result = std::make_unique<FloatNumberExprAST>(std::stof(currentToken().getLiteral()));
	consume();
	if (currentToken().getType() == SEMICOLUMN)
	{
		consume();
	}
	return std::move(Result);
}

std::unique_ptr<ExprAST> Parser::parseIntagerNumberExpr()
{
	auto Result = std::make_unique<IntegerNumberExprAST>(std::stof(currentToken().getLiteral()));
	consume();
	return std::move(Result);
}

bool Parser::isStructVariable(const std::string& varName) {
	auto it = Helper::SymbolTable.find(varName);
	if (it == Helper::SymbolTable.end()) {
		return false;  // Variable not found
	}

	llvm::AllocaInst* alloca = it->second;
	llvm::Type* varType = alloca->getAllocatedType();

	return varType->isStructTy();  // Check if it's a struct type
}

std::unique_ptr<ExprAST> Parser::parseIdentifierExpr()
{
	std::string IdName = currentToken().getLiteral();
	
	// Check for struct
	if (isStructVariable(IdName))
	{
		// Handle struct
		return parseStructAssignment();
	}

	consume(); // eat identifier.

	if (currentToken().getType() != LPAREN) // Simple variable ref.
	{
		if (currentToken().getType() == SEMICOLUMN) 
		{
			consume();
		}
		return std::make_unique<VariableExprAST>(IdName);
	}

	// Call.
	consume(); // eat (
	std::vector<std::unique_ptr<ExprAST>> Args;
	if (currentToken().getType() != RPAREN)
	{
		while (true)
		{
			if (auto Arg = parseExpression())
			{
				Args.push_back(std::move(Arg));
			}
			else
			{
				return nullptr;
			}

			if (currentToken().getType() == RPAREN)
			{
				break;
			}

			consume();
		}
	}

	// Eat the ')'.
	consume();
	if (currentToken().getType() == SEMICOLUMN)
	{
		consume();
	}

	return std::make_unique<CallExprAST>(IdName, std::move(Args));
}

std::unique_ptr<ExprAST> Parser::parseParenExpr()
{
	consume(); // eat (.
	auto V = parseExpression();
	if (!V)
	{
		return nullptr;
	}

	consume(); // eat ).
	return V;
}

//Parse general expressions (addition, subtraction, etc.)
std::unique_ptr<ExprAST> Parser::parseExpression() {
	auto LHS = parsePrimary();
	if (!LHS)
	{
		return nullptr;
	}
	if (isUnaryOp()) // Currently handling only Postfix Ops. Such as "x++" and not "++x"
	{
		return parseUnaryOp(std::move(LHS));
	}
	return parseBinOpRHS(0, std::move(LHS));
}


/*
* Main function for parsing each sentence (expression)
*/
std::unique_ptr<ExprAST> Parser::parsePrimary()
{
	switch (currentToken().getType())
	{
	case TYPE_DECLERATION:
		return this->parseAssignment();
	case LPAREN:
		return parseParenExpr();
	case IF_WORD:
		return parseIfStatement();
	case FLOAT:
		return parseFloatNumberExpr();
	case IDENTIFIER:
		return parseIdentifierExpr();
	// TODO: parse return statement  (return VariableExprAST)
	case RETURN_STATEMENT:
		return nullptr;
	case INT:
		return parseIntagerNumberExpr();
	case WHILE_LOOP:
		return parseWhileLoop();
	case DO_WHILE_LOOP:
		return parseDoWhileLoop();
	case FOR_LOOP:
		return parseForLoop();
	case STRUCT:
		return parseStruct();
	default:
		throw SyntaxError("Undefined Sentence begining");
	}
}


std::unique_ptr<ExprAST> Parser::parseWhileLoop()
{
	consume(); // Move past 'while'

	// Parse condition
	auto condition = parseExpression();
	if (!condition)
	{
		return nullptr;
	}

	consume(); // Move past '{'

	// Parse body
	std::vector<std::unique_ptr<ExprAST>> body;
	while (currentToken().getType() != R_CURLY_BRACK)
	{
		auto expr = parseExpression();
		if (!expr)
		{
			return nullptr;
		}
		body.emplace_back(std::move(expr));
	}
	consume(); // Move past '}'

	return std::make_unique<WhileLoopAST>(std::move(condition), std::move(body));
}

std::unique_ptr<ExprAST> Parser::parseDoWhileLoop()
{
	consume(2); // Move past "do" and "{"

	// Parse body
	std::vector<std::unique_ptr<ExprAST>> body;
	while (currentToken().getType() != R_CURLY_BRACK)
	{
		auto expr = parseExpression();
		if (!expr)
		{
			return nullptr;
		}
		body.emplace_back(std::move(expr));
	}

	consume(2);
	// Parse condition
	auto condition = parseExpression();
	if (!condition)
	{
		return nullptr;
	}

	consume(); // Move past ';'
	return std::make_unique<DoWhileLoopAST>(std::move(condition), std::move(body));
}

std::unique_ptr<ExprAST> Parser::parseForLoop()
{
	consume(2); // Move past 'for' and '('
	// Parse condition
	auto condInit = parseExpression();
	if (!condInit)
	{
		return nullptr;
	}
	
	auto condC = parseExpression();
	if (!condC)
	{
		return nullptr;
	}

	consume(); // Move past ';'
	auto condInc = parseExpression();
	if (!condInc)
	{
		return nullptr;
	}
	consume(3);
	// Parse body
	std::vector<std::unique_ptr<ExprAST>> body;
	while (currentToken().getType() != R_CURLY_BRACK)
	{
		auto expr = parseExpression();
		if (!expr)
		{
			return nullptr;
		}
		body.emplace_back(std::move(expr));
	}
	consume(); // Move past '}' 
	return std::make_unique<ForLoopAST>(std::move(body), std::move(condInit), std::move(condC), std::move(condInc));
}

std::unique_ptr<ExprAST> Parser::parseStructDefinition()
{
	consume();
	std::string name = currentToken().getLiteral(); // Name of struct
	std::string type;
	std::string nameM; // New struct member name

	consume(2); // Move past name and '{'

	// Get struct members
	std::vector<Field> members;

	while (currentToken().getType() != R_CURLY_BRACK)
	{
		if (std::find(Helper::definedTypes.begin(), Helper::definedTypes.end(), currentToken().getLiteral()) != Helper::definedTypes.end())
		{
			std::string type = currentToken().getLiteral();
			consume();
			std::string name = currentToken().getLiteral();
			consume();
			if (currentToken().getType() == SEMICOLUMN)
				consume();

			members.push_back({ type, name });
		}
		//consume();
	}
	consume(2);
	return std::make_unique<StructDefinitionExprAST>(name, members);
}

std::unique_ptr<ExprAST> Parser::parseStruct()
{
	consume(); // Move past 'struct'

	std::string structT = currentToken().getLiteral();
	// Check that struct exists
	if (Helper::StructTable.find(structT) == Helper::StructTable.end())
	{
		SyntaxError("Non-existing struct");
	}
	consume(); // Move past struct type

	std::string name = currentToken().getLiteral();
	consume(2); // Move past name and ';'

	return std::make_unique<StructDeclerationExprAST>(structT, name); // LATER
}

std::unique_ptr<ExprAST> Parser::parseStructAssignment()
{
	
	return std::unique_ptr<ExprAST>();
}



std::unique_ptr<PrototypeAST> Parser::parsePrototype()
{
	// were starting on the type
	std::string returnType = currentToken().getLiteral();
	consume();
	std::string FnName = currentToken().getLiteral();
	std::cout << "NEW FUNC --> " << FnName << " <--" << std::endl;
	consume(2);
	// consume the (
	std::vector<Field> argsTypesAndNames;
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

std::unique_ptr<FunctionAST> Parser::parseFunctionDefinition()
{
	auto Proto = parsePrototype();
	if (!Proto)
	{
		return nullptr;
	}
	std::string funcType = Proto.get()->getReturnType();

	std::vector<std::unique_ptr<ExprAST>> funcBody;

	// Iterate through function and parse it
	while (currentToken().getType() != RETURN_STATEMENT && currentToken().getType() != R_CURLY_BRACK)
	{
		std::unique_ptr<ExprAST> expr = parseExpression();
		if (!expr)
		{
			return nullptr;
		}
		funcBody.emplace_back(std::move(expr));
	}

	std::unique_ptr<ExprAST> returnStatement = nullptr;
	if (currentToken().getType() == RETURN_STATEMENT)
	{
		//consume();
		returnStatement = parseExpression();
		return std::make_unique<FunctionAST>(std::move(Proto), std::move(funcBody), std::move(returnStatement), funcType);
	}
	else if (Proto.get()->getReturnType() == "void")
	{
		auto voidRet = parseVoid();
		return std::make_unique<FunctionAST>(std::move(Proto), std::move(funcBody), std::move(returnStatement), funcType);
	}
	else
	{
		throw SyntaxError("Excepted a return statement");
	}

	return nullptr;
}

std::unique_ptr<ExprAST> Parser::parseVoid()
{
	return std::make_unique<VoidAst>();
}

std::unique_ptr<FunctionAST> Parser::parseTopLevelExpr()
{
	std::vector<std::unique_ptr<ExprAST>> expr;
	expr.emplace_back(parseExpression());
	if (!expr.empty()) {
		// Make an anonymous proto.
		auto Proto = std::make_unique<PrototypeAST>(ANNONIMUS_FUNC_LIT,
			std::vector<Field>(),
			VOID_TYPE_LIT);
		return std::make_unique<FunctionAST>(std::move(Proto), std::move(expr), nullptr, VOID_TYPE_LIT);
	}
	return nullptr;
}