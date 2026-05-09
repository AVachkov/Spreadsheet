#include "functions.h"
#include <string> // for std::stoi and std::stod

Functions::Functions()
{
}

Functions::Functions(const Functions &f)
{
}

void Functions::trim(std::string &str)
{
    if (str.empty())
        return;

    int i = 0;
    while (i < str.size() && !isLetterOrSymbol(str[i]))
    {
        if (str[i] == ' ' || str[i] == '\n' || str[i] == '\t')
            str.erase(i, 1);
        else
            ++i;
    }

    i = str.size() - 1;
    while (i >= 0 && !isLetterOrSymbol(str[i]))
    {
        if (str[i] == ' ' || str[i] == '\n' || str[i] == '\t')
            str.erase(i, 1);

        --i;
    }
}

bool Functions::isLetterOrSymbol(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '!' && c <= '@') || (c >= '[' && c <= '`') || (c >= '{' && c <= '~');
}

int Functions::stringToInt(const std::string &s)
{
    return std::stoi(s);
    // bool negative = false;
    // int result = 0;
    // for (char c : s)
    // {
    //     if (c == '-')
    //     {
    //         negative = true;
    //     }
    //     if (c == '+')
    //     {
    //     }
    //     else
    //     {
    //         result *= 10;
    //         result += (c - '0');
    //     }
    // }

    // if (negative)
    //     result = -result;

    // return result;
}

double Functions::stringToDouble(const std::string &s)
{
    return std::stod(s);
}