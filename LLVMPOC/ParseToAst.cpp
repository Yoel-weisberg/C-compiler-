#include "ParseToAst.h"

void printAST(const INode* node, int indent)
{
    if (node == nullptr) return;
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << node->value << " (" << node->token << ")" << std::endl;
    printAST(node->left, indent + 1);
    printAST(node->right, indent + 1);
}

INode* Parser::parsePrimary()
{
    const auto& token = tokens[pos];
    if (token.second == INT) {
        pos++;
        return new IntConstantNode(token.first);
    }
    else if (token.second == SUBTRACTION) {
        pos++; // Skip the '-' token

        // Parse the primary expression after the '-'
        INode* node = parsePrimary();

        // Create a unary negation node
        return new BinaryNode('-', nullptr, std::unique_ptr<INode>(node));
    }
    else if (token.second == LPAREN) {
        pos++;

        // parsing the code inside the parentheses
        INode* node = parseExpression();

        if (tokens[pos].second != RPAREN) {
            LogErrorV("Expected closing parenthesis");
            return nullptr;
        }

        // skipping the closing parenthesis
        pos++;
        return node;
    }

    return LogErrorV("Unknown token when expecting an expression");
}


INode* Parser::parseMultiplicationDivision()
{
    INode* node = parsePrimary();

    while (pos < tokens.size() && (tokens[pos].second == MULTIPLICATION || tokens[pos].second == DIVISION)) {
        auto op = tokens[pos++];
        INode* rightNode = parsePrimary();
        INode* newNode = new BinaryNode(op.first, std::unique_ptr<INode>(node), std::unique_ptr<INode>(rightNode));
        node = newNode;
    }

    return node;
}

INode* Parser::parseAdditionSubtraction()
{
    INode* node = parseMultiplicationDivision();

    while (pos < tokens.size() && (tokens[pos].second == ADDITION || tokens[pos].second == SUBTRACTION)) {
        auto op = tokens[pos++];
        INode* rightNode = parseMultiplicationDivision();
        INode* newNode = new BinaryNode(op.first, std::unique_ptr<INode>(node), std::unique_ptr<INode>(rightNode));
        node = newNode;
    }

    return node;
}

