#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "Constents.h"

struct ASTNode {
    std::string value;
    Tokens token;
    ASTNode* left;
    ASTNode* right;

    ASTNode(std::string val, Tokens tok)
        : value(val), token(tok), left(nullptr), right(nullptr) {}

    ASTNode(std::string val, Tokens tok, ASTNode* left, ASTNode* right) : value(val), token(tok), left(left), right(right) {}

    bool haveKids() { return (left != nullptr || right != nullptr);  }

    int toInt() { return std::stoi(value); }

    ~ASTNode() {
        delete left;
        delete right;
    }
};

class Parser {
public:
    Parser(const std::vector<std::pair<std::string, Tokens>>& tokens) : tokens(tokens), pos(0) {}

    ASTNode* parseExpression() {
        return parseAdditionSubtraction();
    }

private:
    std::vector<std::pair<std::string, Tokens>> tokens;
    int pos;
    ASTNode* parsePrimary();
    ASTNode* parseMultiplicationDivision();
    ASTNode* parseAdditionSubtraction();
};

void printAST(const ASTNode* node, int indent = 1);