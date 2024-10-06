#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "Constents.h"
#include "INode.h"
#include "IntConstantNode.h"
#include "BinaryOperationNode.h"

class Parser {
public:
    Parser(const std::vector<std::pair<std::string, Tokens>>& tokens) : tokens(tokens), pos(0) {}

    INode* parseExpression() {
        return parseAdditionSubtraction();
    }

private:
    std::vector<std::pair<std::string, Tokens>> tokens;
    int pos;
    INode* parsePrimary();
    INode* parseMultiplicationDivision();
    INode* parseAdditionSubtraction();
};
