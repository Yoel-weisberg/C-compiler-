#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <memory>
#include <vector>
#include "Token.h"  // Make sure Token.h is in the same directory
#include "ExprAST.h"
#include "Constents.h"

// Forward declarations of the AST classes
class ExprAST;
class AssignExprAST;
class BinaryExprAST;
class NumberExprAST;

class Parser {
private:
    std::vector<Token> tokens;
    size_t currentTokenIndex;

    Token& currentToken();
    void consume();
    bool isAtEnd();

public:
    // Constructor
    Parser(const std::vector<Token>& tokens);

    // Parse methods
    std::unique_ptr<ExprAST> parse();
    std::unique_ptr<ExprAST> parseAssignment();
    //std::unique_ptr<ExprAST> parseExpression();
    //std::unique_ptr<ExprAST> parseTerm();
    //std::unique_ptr<ExprAST> parseFactor();
};

#endif // PARSER_H
