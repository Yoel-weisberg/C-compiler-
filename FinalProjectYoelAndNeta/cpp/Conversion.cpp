#include "Conversion.h"

std::string Conversion::llToStr(long long to_convert)
{
    std::string result = "";
    do {
        result += std::to_string(int(to_convert % 10));
        to_convert /= 10;
    } while (to_convert != 0);
    return result;
}
