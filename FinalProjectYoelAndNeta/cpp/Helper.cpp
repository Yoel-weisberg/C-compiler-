#include "../Header/Helper.h"

bool Helper::checkIdentifier(const std::string& id)
{
    if (macroKey.empty()) {
        return false; // Empty strings are not valid identifiers
    }

    // Check the first character: it must be a letter or underscore
    if (!(std::isalpha(macroKey[0]) || macroKey[0] == '_')) {
        return false;
    }

    // Check the rest of the characters: they must be letters, digits, or underscores
    for (size_t i = 1; i < macroKey.length(); ++i) {
        if (!(std::isalnum(macroKey[i]) || macroKey[i] == '_')) {
            return false;
        }
    }

    return true;
}
