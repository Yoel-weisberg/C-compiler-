#include "Parser.h"

class TopLevelParser
{
public:
	TopLevelParser(const std::vector<Token>& tokens);
	void mainLoop();
private:
	void HandleTopLevelExpression();

	Parser parser;
};