#include "SyntexAnalysis.h"

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
    bool isThereNoContinuesSign = true; // indicates if there is * or / signs in the current session
    for (auto pr : tokens)
    {
        int chrIndex = 0;
        
        if (pr.second == MULTIPLICATION || pr.second == DIVISION)
        {
            if (!isThereNoContinuesSign)
            {
                throw SyntaxError("There are  too many operaters ", chrIndex);
            }
            isThereNoContinuesSign = true;
        }
        else if (pr.second == INT)
        {
            isThereNoContinuesSign = false;
        }
    }
}
