#pragma once

#include <iostream>
#include <vector>
#include "Constents.h"

class Helper
{
public:
	static bool checkIdentifier(const std::string& id);
	static std::vector<std::string> definedTypes;
	static std::vector<char> separeters;
};