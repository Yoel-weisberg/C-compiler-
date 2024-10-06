#include "ParseToAst.h"

INode* Parser::parsePrimary()
{
    const auto& token = tokens[pos];
    if (token.second == INT) {
        pos++;
        return new IntConstantNode(std::stoi(token.first));
    }
    else if (token.second == SUBTRACTION) {
        pos++; // Skip the '-' token

        // Parse the primary expression after the '-'
        std::unique_ptr<INode> node(parsePrimary());

        // Create a unary negation node
        return new BinaryNode('-', nullptr, std::move(node));
    }
    else if (token.second == LPAREN) {
        pos++;

        // parsing the code inside the parentheses
        std::unique_ptr<INode> node(parseExpression());

        if (tokens[pos].second != RPAREN) {
            INode::LogErrorV("Expected closing parenthesis");
            return nullptr;
        }

        // skipping the closing parenthesis
        pos++;
        return node.release(); // Release ownership since we return raw pointer
    }

    INode::LogErrorV("Unknown token when expecting an expression");
    return nullptr; // Ensure we return a pointer in every path
}



INode* Parser::parseMultiplicationDivision()
{
    INode* node = parsePrimary();

    while (pos < tokens.size() && (tokens[pos].second == MULTIPLICATION || tokens[pos].second == DIVISION)) {
        auto op = tokens[pos++];
        INode* rightNode = parsePrimary();
        INode* newNode = new BinaryNode(op.first[0], std::unique_ptr<INode>(node), std::unique_ptr<INode>(rightNode));
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
        INode* newNode = new BinaryNode(op.first[0], std::unique_ptr<INode>(node), std::unique_ptr<INode>(rightNode));
        node = newNode;
    }

    return node;
}

