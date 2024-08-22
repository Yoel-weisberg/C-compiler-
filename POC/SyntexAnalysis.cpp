#include "SyntexAnalysis.h"

SyntexAnalysis::SyntexAnalysis(const std::vector<std::pair<std::string, Tokens>>& tokens) :
    tokens(tokens)
{
    checkPernthesis();
    noTwoOperationAfterEachOther();
}

void SyntexAnalysis::checkPernthesis()
{
    int lprenCount = 0;
    int chrIndex = 0;
    for (auto pr : tokens)
    {
        chrIndex += pr.first.size();
        // Check for left parenthesis and increment the counter
        if (pr.second == LPREN)
        {
            lprenCount++;
        }
        // Check for right parenthesis and decrement the counter

        else if (pr.second == RPREN)
        {
            lprenCount--;

            // If count goes negative, there's an unmatched right parenthesis
            if (lprenCount < 0)
            {
                throw SyntaxError("Unmatched right parenthesis", chrIndex);
            }
        }
    }

    // After loop, if count is positive, there are unmatched left parentheses
    if (lprenCount > 0)
    {
        throw SyntaxError("Unmatched left parenthesis", chrIndex);
    }
}

void SyntexAnalysis::noTwoOperationAfterEachOther()
{
    bool isThereSign = true; // indicates if there is * or / signs in the current session
    int chrIndex = 0;
    bool isTherePren = false;
    bool isThereANumber = false;
    for (auto pr : tokens)
    {
        chrIndex += pr.first.size();
        if (pr.second == MULTIPLICATION || pr.second == DIVISION)
        {
            if (isThereSign || isTherePren)
            {
                throw SyntaxError("There are  too many operaters ", chrIndex);
            }

            isThereSign = true;
        }
        else if (pr.second == LPREN || pr.second == RPREN)
        {
            isTherePren = true;
        }
        else if (pr.second == INT)
        {
            isThereSign = false;
            isTherePren = false;
            isThereANumber = true;
        }
        else if (pr.second == ADDITION || pr.second == SUBSTRACTION)
        {
            if (isTherePren)
            {
                throw SyntaxError("There are  too many operaters ", chrIndex);
            }
            isThereSign = true;
        }
    }
    if (!isThereANumber)
    {
        throw SyntaxError("There is no integer in the expression", chrIndex);
    }
}
