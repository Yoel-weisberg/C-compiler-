#include "../Header/Helper.h"

std::vector<std::string> Helper::definedTypes = { "float" };

std::vector<char> Helper::separeters = { SEMICOLUMN_LITERSL, LPAREN_LITERAL, RPAREN_LITERAL, EQUEL_SIGN_LITERAL };

bool Helper::checkIdentifier(const std::string& id)
{
    if (id.empty()) {
        return false; // Empty strings are not valid identifiers
    }

    // Check the first character: it must be a letter or underscore
    if (!(std::isalpha(id[0]) || id[0] == '_')) {
        return false;
    }

    // Check the rest of the characters: they must be letters, digits, or underscores
    for (size_t i = 1; i < id.length(); ++i) {
        if (!(std::isalnum(id[i]) || id[i] == '_')) {
            return false;
        }
    }

    return true;
}
