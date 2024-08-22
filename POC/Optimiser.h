#pragma once
#include <vector>
#include <string>
#include "Constents.h"


class Optimiser {
public:
	Optimiser(const std::vector<std::pair<std::string, Tokens>>& tokens);
	std::vector<std::pair<std::string, Tokens>> getOptimised() const;

private:
	void m_optimisePlusAndMinus();
	std::vector<std::pair<std::string, Tokens>> m_tokens;
	std::vector<std::pair<std::string, Tokens>> m_optimisedTokens;
 };
