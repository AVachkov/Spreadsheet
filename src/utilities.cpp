#include "utilities.h"
#include <cctype>

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

void toLowercase(std::string &s)
{
    for (char &c : s)
    {
        c = std::tolower(c);
    }
}

std::vector<std::string> split(const std::string &s)
{
    std::vector<std::string> tokens;

    std::string token;
    for (char c : s)
    {
        if (isWhitespace(c))
        {
            if (!token.empty())
            {
                tokens.push_back(token);
                token.clear();
            }
        }
        else
        {
            token += c;
        }
    }

    if (!token.empty())
        tokens.push_back(token);

    return tokens;
}
