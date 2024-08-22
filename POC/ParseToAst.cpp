#include "ParseToAst.h"

void printAST(const ASTNode* node, int indent)
{
    if (node == nullptr) return;
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << node->value << " (" << node->token << ")" << std::endl;
    printAST(node->left, indent + 1);
    printAST(node->right, indent + 1);
}

ASTNode* Parser::parsePrimary()
{
    const auto& token = tokens[pos];
    if (token.second == INT) {
        pos++;
        return new ASTNode(token.first, token.second);
    }
    else if (token.second == LPAREN) {
        pos++;

        // parsing the code inside the pernthesis
        ASTNode* node = parseExpression();

        // skipping the closing prenthesis
        pos++;

        return node;
    }
}

ASTNode* Parser::parseMultiplicationDivision()
{
    ASTNode* node = parsePrimary();

    while (pos < tokens.size() && (tokens[pos].second == MULTIPLICATION || tokens[pos].second == DIVISION)) {
        auto op = tokens[pos++];
        ASTNode* rightNode = parsePrimary();
        ASTNode* newNode = new ASTNode(op.first, op.second);
        newNode->left = node;
        newNode->right = rightNode;
        node = newNode;
    }

    return node;
}

ASTNode* Parser::parseAdditionSubtraction()
{
    ASTNode* node = parseMultiplicationDivision();

    while (pos < tokens.size() && (tokens[pos].second == ADDITION || tokens[pos].second == SUBTRACTION)) {
        auto op = tokens[pos++];
        ASTNode* rightNode = parseMultiplicationDivision();
        ASTNode* newNode = new ASTNode(op.first, op.second);
        newNode->left = node;
        newNode->right = rightNode;
        node = newNode;
    }

    return node;
}
