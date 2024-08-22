#include "INode.h"


int Node::getLevel(Tokens type)
{
    if (type == ADDITION || type == SUBSTRACTION)
    {
        return 0;
    }
    else if (type == MULTIPLICATION || type == DIVISION)
    {
        return 1;
    }
}

Node::Node(std::string value, Tokens type, Node* father) :
    m_value(value), m_type(type)
{
}

int Node::getAmountOfChilds() const
{
    return m_kids.size();
}

Node* Node::getChild(const int& index)
{
    return m_kids.at(index);
}

void Node::addChild(Node* node)
{
    m_kids.push_back(node);
}

void Node::addChild(std::string value, Tokens type)
{
    Node* child = new Node(value, type, this);

    this->m_kids.push_back(child);
}

Tokens Node::getType() const
{
    return m_type;
}

Node* Node::getFather() const
{
    return m_father;
}
