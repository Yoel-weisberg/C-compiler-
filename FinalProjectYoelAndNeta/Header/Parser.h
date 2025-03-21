#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <memory>   
#include <vector>
#include <sstream>
#include "Token.h"  // Make sure Token.h is in the same directory
#include "ExprAST.h"
#include "Constants.h"
#include "CompilationErrors.h"

// Forward declarations of the AST classes
class ExprAST;
class AssignExprAST;
class BinaryExprAST;
class FloatNumberExprAST;

class Parser {
private:
    std::vector<Token> _tokens;
    size_t _currentTokenIndex;
    std::string _IdentifierStr;
    bool isAtEnd();

    std::unique_ptr<ExprAST> _head;
    int getTokenPrecedence();

    std::map<Tokens_type, int> _BinopPrecedence;

public:
    // Constructor
    Parser(const std::vector<Token>& tokens);
    
    bool isFinished() const;

    void consume(int times = 1);
    Token& currentToken();

    // Parse methods
    std::unique_ptr<ExprAST> parse();

    std::unique_ptr<ExprAST> parseAssignment();
    std::unique_ptr<ExprAST> ptrAssignmentParsing();
    std::unique_ptr<ExprAST> regularAssignmentParsing();

    std::unique_ptr<ExprAST> arrAssignmentParsing(const std::string& type);

    std::unique_ptr<ExprAST> parseIfStatement();

    std::unique_ptr<ExprAST> parseFloatNumberExpr();
    std::unique_ptr<ExprAST> parseIntagerNumberExpr();

    std::unique_ptr<ExprAST> parseReturnStatement();
    std::unique_ptr<ExprAST> parseParenExpr();
    std::unique_ptr<ExprAST> parseIdentifierExpr();
    std::unique_ptr<ExprAST> parseExpression();

    std::unique_ptr<ExprAST> parsePrimary();

    std::unique_ptr<ExprAST> parseWhileLoop();
    std::unique_ptr<ExprAST> parseDoWhileLoop();
    std::unique_ptr<ExprAST> parseForLoop();

    std::unique_ptr<ExprAST> parseStructDefinition();
    std::unique_ptr<ExprAST> parseStruct();
    std::unique_ptr<ExprAST> parseStructAssignment();
    bool isStructVariable(const std::string& varName);

    std::unique_ptr<ExprAST> parseBinOpRHS(int ExprPrec,
        std::unique_ptr<ExprAST> LHS);
    std::unique_ptr<ExprAST> parseUnaryOp(std::unique_ptr<ExprAST> LHS);
    std::unique_ptr<FunctionAST> parseTopLevelExpr();
    std::unique_ptr<PrototypeAST> parsePrototype();
    std::unique_ptr<FunctionAST> parseFunctionDefinition();
    std::unique_ptr<ExprAST> parseVoid();
    // Getter for the root AST
    ExprAST* getAst();

    bool isUnaryOp();
};

#endif // PARSER_H
