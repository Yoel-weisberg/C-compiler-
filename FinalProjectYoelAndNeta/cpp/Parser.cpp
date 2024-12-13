#include "../Header/Parser.h"


#include "ExprAST.h"  // Include the AST classes (or wherever they are defined)
#include <iostream>
#include <stdexcept>

// Constructor for Parser
Parser::Parser(const std::vector<Token>& tokens)
	: _tokens(tokens), _currentTokenIndex(0) {
	// Create Head node (more details in )
	while (find(_tokens.begin(), _tokens.end(), currentToken()) != _tokens.end())
	{
		_head.push_back(parse());
		consume(); // Go to the next line 
	}
}

//ExprAST* Parser::getAst()
//{
//	return _head.get();
//}

std::vector<std::unique_ptr<ExprAST>>& Parser::getMainHead()
{
	return _head;
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


std::unique_ptr<ExprAST> Parser::parseAssignment() {
	if (currentToken().getType() == TYPE_DECLERATION)
	{
		return regularAssignmentParsing();
	}
	if (currentToken().getType() == PTR_TYPE_DECLERATION)
	{
		return ptrAssignmentParsing();
	}
	return nullptr;
}

// Parse pointer assignment statement (e.g "int* ptr = &point_to;")
std::unique_ptr<ExprAST> Parser::ptrAssignmentParsing()
{
	try
	{
		// Regular parsing
		std::string type = currentToken().getLiteral().substr(1, currentToken().getLiteral().size() - 1);
		consume();

		std::string var_name = currentToken().getLiteral();
		consume();
		consume(); // Skip equal sign

		// Check if "pointed to" variable is valid
		// Does it exists in the symbol table
		auto symbolWrapped = Helper::symbolTable.findSymbol(currentToken().getLiteral().substr(1, currentToken().getLiteral().size() - 1));

		if (!symbolWrapped)
		{
			throw ParserError("can't point to non-existing variable");
		}
		// Add pointer to symbol table
		Symbol& pointed_to_var = symbolWrapped->get();
		std::stringstream str_stream;
		str_stream << pointed_to_var.getLLVMValue();

		Helper::addSymbol(var_name, type, str_stream.str());

		consume();
		// Return
		auto value_literal = std::make_unique<ptrExprAST>(str_stream.str());
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
	if (currentToken().getLiteral().substr(currentToken().getLiteral().size() - 2, currentToken().getLiteral().size()) == ARR_INIT_LIT)
	{
		return arrAssignmentParsing(Helper::removeSpecialCharacter(type));
	}

	std::string varName = currentToken().getLiteral();
	consume(); // Move past IDENTIFIER

	if (currentToken().getType() == EQUAL_SIGN) {
		consume(); // Move past '='
		Helper::addSymbol(varName, type, currentToken().getLiteral());
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

std::unique_ptr<ExprAST> Parser::arrAssignmentParsing(const std::string& type)
{
	std::string currVal = "";
	int len = 0; // Length of the created array
	std::string varName = currentToken().getLiteral().substr(0, currentToken().getLiteral().size() - 2); // Cut out '[]'
	consume();
	consume(); // Move past '='
	consume(); // Move part '{'
	std::string init = currentToken().getLiteral();
	try
	{
		// Guide to the solution --> https://stackoverflow.com/questions/7844049/how-are-c-arrays-represented-in-memory	
		if (init[0] == COMMA_LIT)
		{
			throw ParserError("Value missing");
		}

		for (int i = 0; i < init.size(); i++)
		{
			if (init[i] == COMMA_LIT)
			{
				throw ParserError("Empty initilization");
			}
			while (init[i] != COMMA_LIT && (i != init.size()))
			{
				currVal += init[i];
				i++;
			}
			if (type == INTEGER)
			{
				if (!Helper::isInteger(currVal))
				{
					throw ParserError("Invalid type, supposd to be " + type);
				}
			}
			else if (type == FLOAT)
			{
				if (!Helper::isFloat(currVal))
				{
					throw ParserError("Invalid type, supposd to be " + type);
				}
			}
			else if (type == CHAR)
			{
				if (!Helper::isChar(currVal))
				{
					throw ParserError("Invalid type, supposd to be " + type);
				}
			}
			currVal = "";
			len++;
		}
		Helper::addSymbol(varName, ARRAY, init, type, len);
		std::string convertalbleLen = std::to_string(len);
		auto value_literal = std::make_unique<arrExprAST>(type, convertalbleLen, init);
		return std::make_unique<AssignExprAST>(varName, std::move(value_literal), type);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}


