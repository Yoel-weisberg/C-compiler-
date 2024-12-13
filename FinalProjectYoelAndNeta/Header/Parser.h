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
class NumberExprAST;

class Parser {
private:
    std::vector<Token> _tokens;
    size_t _currentTokenIndex;

    Token& currentToken();
    void consume();
    bool isAtEnd();

    // !NOTE!
    // I changed the '_head' variable to a vector because we haven't implemented 
    // functions yet. so that each line is represented by an AST of it's own, 
    // without being connected to the rest of the code, 
    // causing only the first line to be executed. 
    // But in order to have a functioning symbol table, all of the data needs to be there.
    std::vector<std::unique_ptr<ExprAST>> _head;

public:
    // Constructor
    Parser(const std::vector<Token>& tokens);

    // Parse methods
    std::unique_ptr<ExprAST> parse();
    std::unique_ptr<ExprAST> parseAssignment();
    std::unique_ptr<ExprAST> ptrAssignmentParsing();
    std::unique_ptr<ExprAST> regularAssignmentParsing();
    std::unique_ptr<ExprAST> arrAssignmentParsing(const std::string& type);
    
    // Getter for the root AST
    ExprAST* getAst();
    std::vector<std::unique_ptr<ExprAST>>& getMainHead(); // Temporary - until we'll implement functions
};

#endif // PARSER_H
