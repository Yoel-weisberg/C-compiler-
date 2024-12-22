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

class ParserError;


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

    void consume();
    Token& currentToken();

    // Parse methods
    std::unique_ptr<ExprAST> parse();
    std::unique_ptr<ExprAST> parseAssignment();
    std::unique_ptr<ExprAST> ptrAssignmentParsing();
    std::unique_ptr<ExprAST> regularAssignmentParsing();
    std::unique_ptr<ExprAST> arrAssignmentParsing(const std::string& type);
    std::unique_ptr<ExprAST> parseIfStatement();
    std::unique_ptr<ExprAST> ParseFloatNumberExpr();
    std::unique_ptr<ExprAST> ParseParenExpr();
    std::unique_ptr<ExprAST> ParseIdentifierExpr();
    std::unique_ptr<ExprAST> ParseExpression();
    std::unique_ptr<ExprAST> ParsePrimary();
    std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec,
        std::unique_ptr<ExprAST> LHS);
    std::unique_ptr<FunctionAST> ParseTopLevelExpr();
    std::unique_ptr<PrototypeAST> ParsePrototype();
    // Getter for the root AST
    ExprAST* getAst();
};

#endif // PARSER_H
