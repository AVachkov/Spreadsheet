#include "utilities.h"

void trim(std::string &str) {
    if (str.empty())
        return;

    int i = 0;
    while (i < str.size() && !isLetterOrSymbol(str[i])) {
        if (str[i] == ' ' || str[i] == '\n' || str[i] == '\t')
            str.erase(i, 1);
        else
            ++i;
    }

    i = str.size() - 1;
    while (i >= 0 && !isLetterOrSymbol(str[i])) {
        if (str[i] == ' ' || str[i] == '\n' || str[i] == '\t')
            str.erase(i, 1);

        --i;
    }
}

bool isLetterOrSymbol(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '!' && c <= '@') || (c >= '[' && c <= '`') ||
           (c >= '{' && c <= '~');
}

bool isWhitespace(char c) { return c <= 32; }

bool isOperator(char c) { return c == '+' || c == '-' || c == '*' || c == '/' || c == '^'; }
