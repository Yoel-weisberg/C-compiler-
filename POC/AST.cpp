#include "AST.h"

AST::AST(std::vector<std::pair<std::string, Tokens>> listOfTopkens)
{
	bool isThereAddition = false;
	if (listOfTopkens.size() == 1)
	{
		m_rootNode = new Node(listOfTopkens[0].first, listOfTopkens[0].second, nullptr);
	}
	
	// checking for the plus sign

	for (auto pr : listOfTopkens)
	{
		if (pr.second == ADDITION || pr.second == SUBSTRACTION)
		{
			m_rootNode = new Node(std::string(1, ADDITION_LITERAL), ADDITION, nullptr);
			isThereAddition = true;
		}
	}
	if (!isThereAddition)
	{
		m_rootNode = new Node(std::string(1, MULTIPLICATION_LITERAL), MULTIPLICATION, nullptr);
	}
	Node* currentTop = m_rootNode;
	for (int i = 0; i < listOfTopkens.size(); i++)
	{
		if (listOfTopkens[i].second == INT)
		{
			if (i == listOfTopkens.size() - 1)
			{
				currentTop->addChild(listOfTopkens[i].first, listOfTopkens[i].second);
			}
			else
			{
				if (Node::getLevel(m_rootNode->getType()) < Node::getLevel(listOfTopkens[i + 1].second))
				{
					Node* newTop = new Node(listOfTopkens[i + 1].first, listOfTopkens[i + 1].second, currentTop);
					currentTop->addChild(newTop);
					currentTop = newTop;
					currentTop->addChild(listOfTopkens[i].first, listOfTopkens[i].second);
				}
				else if (Node::getLevel(m_rootNode->getType()) > Node::getLevel(listOfTopkens[i + 1].second))
				{
					currentTop->addChild(listOfTopkens[i].first, listOfTopkens[i].second);
					currentTop = m_rootNode;
				}
				else
				{
					currentTop->addChild(listOfTopkens[i].first, listOfTopkens[i].second);
				}
			}
		}
		else if (listOfTopkens[i].second == LPREN)
		{
			int curreIndex = 0;
			// finding the most right pren
			for (int i = 0; i < listOfTopkens.size(); i++)
			{
				if (listOfTopkens[i].second == RPREN)
				{
					curreIndex = i;
				}
			}
			std::vector<std::pair<std::string, Tokens>> subVector(listOfTopkens.begin() + i, listOfTopkens.begin() + curreIndex);
			currentTop->addChild(AST(subVector).m_rootNode);
		}
	}
}

