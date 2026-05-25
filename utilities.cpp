#include "utilities.h"

void trim(std::string &str)
{
    if (str.empty())
        return;

    while (!str.empty() && isWhitespace(str[0]))
        str.erase(0, 1);

    while (!str.empty() && isWhitespace(str[str.size() - 1]))
        str.pop_back();
}

bool isWhitespace(char c) { return c <= 32; }

bool isOperator(char c) { return c == '+' || c == '-' || c == '*' || c == '/' || c == '^'; }
