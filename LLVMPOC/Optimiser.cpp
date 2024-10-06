#include "Optimiser.h"

Optimiser::Optimiser(const std::vector<std::pair<std::string, Tokens>>& tokens) :
	m_tokens(tokens)
{
	m_optimisePlusAndMinus();
}

std::vector<std::pair<std::string, Tokens>> Optimiser::getOptimised() const
{
	return m_optimisedTokens;
}

void Optimiser::m_optimisePlusAndMinus()
{
	int val = 1;
	bool wasTherePlusOrMinus = false;
	for (auto pr : m_tokens)
	{
		if (pr.second == ADDITION)
		{
			val *= 1;
			wasTherePlusOrMinus = true;
		}
		else if (pr.second == SUBTRACTION)
		{
			val *= -1;
			wasTherePlusOrMinus = true;
		}
		else if (pr.second == INT || pr.second == LPAREN)
		{
			if (wasTherePlusOrMinus)
			{
				m_optimisedTokens.push_back({ val == 1 ? std::string(1, ADDITION_LITERAL) : std::string(1, SUBSTRICTION_LITERAL) , val == 1 ? ADDITION : SUBTRACTION });
			}
			m_optimisedTokens.push_back(pr);
			wasTherePlusOrMinus = false;
			val = 1;
		}
		else
		{
			m_optimisedTokens.push_back(pr);
		}
	}

	//for (int i = 0; i < m_optimisedTokens.size(); i++)
	//{
	//	if (m_optimisedTokens[i].second == SUBTRACTION)
	//	{
	//		m_optimisedTokens[i + 1].first = "-" + m_optimisedTokens[i + 1].first;
	//		m_optimisedTokens[i].first = ADDITION_LITERAL;
	//		m_optimisedTokens[i].second = ADDITION;
	//	}
	//	else if (m_optimisedTokens[i].second == DIVISION)
	//	{
	//		m_optimisedTokens[i + 1].first = "1";
	//		m_optimisedTokens.insert(m_optimisedTokens.begin() + i + 2, )
	//		m_optimisedTokens[i].first = MULTIPLICATION_LITERAL;
	//		m_optimisedTokens[i].second = MULTIPLICATION;
	//	}
	//}
}
