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
class FloatNumberExprAST;

class Parser {
private:
    std::vector<Token> tokens;
    size_t currentTokenIndex;

    Token& currentToken();
    void consume();
    bool isAtEnd();

    std::unique_ptr<ExprAST> head;

public:
    // Constructor
    Parser(const std::vector<Token>& tokens);

    // Parse methods
    std::unique_ptr<ExprAST> parse();
    std::unique_ptr<ExprAST> parseAssignment();
    std::unique_ptr<ExprAST> parseIfStatement();
    std::unique_ptr<ExprAST> ParseNumberExpr();
    std::unique_ptr<ExprAST> ParseParenExpr();
    std::unique_ptr<ExprAST> ParseIdentifierExpr();
    std::unique_ptr<ExprAST> ParseExpression();
    std::unique_ptr<ExprAST> ParsePrimary();
    std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec,
        std::unique_ptr<ExprAST> LHS);

    // Getter for the root AST
    ExprAST* getAst();

};

#endif // PARSER_H
