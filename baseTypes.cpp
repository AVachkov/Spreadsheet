#include "baseTypes.h"
#include "functions.h" // remove later if we can use std::stoi/std::stod

Address::Address() : row(1), col(1)
{
}

Address::Address(size_t _row, size_t _col) : row(_row), col(_col)
{
}

Address &Address::operator=(const Address &rhs)
{
    if (this != &rhs)
    {
        row = rhs.row;
        col = rhs.col;
    }

    return *this;
}

bool Address::isAddress(const std::string &s, Address &out)
{
    out.row = 0;
    out.col = 0;

    if (s.empty() || s.size() < 4)
        return false;

    if (s[0] != 'R')
        return false;

    size_t row = 0, col = 0;

    size_t i = 1;
    while (i < s.size() && s[i] != 'C')
    {
        if (!Number::isDigit(s[i]))
            return false;

        row *= 10;
        row += (s[i] - '0');
        ++i;
    }

    if (i >= s.size() || s[i] != 'C')
        return false;

    ++i;
    while (i < s.size())
    {
        if (!Number::isDigit(s[i]))
            return false;

        col *= 10;
        col += (s[i] - '0');
        ++i;
    }

    out.row = row;
    out.col = col;
    return true;
}

Number::Number() : wholeNumber(0), decimalNumber(0.0), type(NumberType::NONE)
{
}

Number::Number(int _wholeNumber) : wholeNumber(_wholeNumber), decimalNumber(0.0), type(NumberType::WHOLE_NUMBER)
{
}

Number::Number(double _decimalNumber) : wholeNumber(0), decimalNumber(_decimalNumber), type(NumberType::DECIMAL_NUMBER)
{
}

double Number::getValue() const
{
    if (type == NumberType::DECIMAL_NUMBER)
        return decimalNumber;
    if (type == NumberType::WHOLE_NUMBER)
        return (double)wholeNumber;

    return 0.0;
}

NumberType Number::getType() const
{
    return type;
}

Number &Number::operator=(int w)
{
    wholeNumber = w;
    type = NumberType::WHOLE_NUMBER;
    return *this;
}

Number &Number::operator=(double d)
{
    decimalNumber = d;
    type = NumberType::DECIMAL_NUMBER;
    return *this;
}

Number Number::operator+(Number rhs)
{
}

bool Number::isNumber(const std::string &str, Number &number)
{
    number.wholeNumber = 0;
    number.decimalNumber = 0.0;
    number.type = NumberType::NONE;

    if (str.empty())
        return false;

    size_t start = 0;
    if (str[0] == '-' || str[0] == '+')
        start = 1;

    int dots = 0;
    for (size_t i = start; i < str.size(); ++i)
    {
        if (!isDigit(str[i]) && str[i] != '.')
            return false;

        if (str[i] == '.')
            ++dots;

        if (dots > 1)
            return false;
    }

    if (str != ".")
    {
        if (dots == 0)
        {
            number.type = NumberType::WHOLE_NUMBER;
            number.wholeNumber = Functions::stringToInt(str);
        }
        else
        {
            number.type = NumberType::DECIMAL_NUMBER;
            number.decimalNumber = Functions::stringToDouble(str);
        }

        return true;
    }

    return false;
}

bool Number::isDigit(char c)
{
    return c >= '0' && c <= '9';
}