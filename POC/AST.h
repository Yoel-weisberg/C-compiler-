#pragma once
#include <vector>
#include "Constents.h"
#include <string>
#include "INode.h"

class AST {
public:
	AST(std::vector<std::pair<std::string, Tokens>> listOfTopkens);

private:
	Node* m_rootNode;
};
