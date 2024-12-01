#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <memory>
#include <vector>
#include "Token.h"  // Make sure Token.h is in the same directory
#include "ExprAST.h"
#include "Constants.h"

// Forward declarations of the AST classes
class ExprAST;
class AssignExprAST;
class BinaryExprAST;
class NumberExprAST;

class Parser {
private:
    std::vector<Token> _tokens;
    size_t _currentTokenIndex;

    Token& currentToken();
    void consume();
    bool isAtEnd();

    std::unique_ptr<ExprAST> _head;

public:
    // Constructor
    Parser(const std::vector<Token>& tokens);

    // Parse methods
    std::unique_ptr<ExprAST> parse();
    std::unique_ptr<ExprAST> parseAssignment();
    
    // Getter for the root AST
    ExprAST* getAst();

    //std::unique_ptr<ExprAST> parseExpression();
    //std::unique_ptr<ExprAST> parseTerm();
    //std::unique_ptr<ExprAST> parseFactor();
};

#endif // PARSER_H
