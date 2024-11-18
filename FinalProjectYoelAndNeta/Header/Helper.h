#pragma once

#include <iostream>
#include <vector>
#include "Constents.h"
#include <regex>

class Helper
{
public:
	static bool checkIdentifier(const std::string& id);
	static bool isFloat(const std::string& num);
	static std::vector<std::string> definedTypes;
	static std::vector<char> separeters;
};