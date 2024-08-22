#pragma once
#include <vector>
#include <string>
#include "Constents.h"

#define LEFT_CHILD 0
#define RIGHT_CHILD 1

class Node {
public:
	static int getLevel(Tokens type);
	Node(std::string value, Tokens type, Node* father);
	int getAmountOfChilds() const; // checking if node has kids
	Node* getChild(const int& index);
	void addChild(Node* node);
	void addChild(std::string value, Tokens type);
	Tokens getType() const;
	Node* getFather() const;
private:
	std::vector<Node*> m_kids;
	std::string m_value;
	Tokens m_type;
	Node* m_father;
};
