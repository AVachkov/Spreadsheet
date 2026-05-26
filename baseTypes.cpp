#include "baseTypes.h"
#include <cctype>
#include <cmath>
#include <exception>
#include <iostream>
#include <string>

Address::Address() : row(1), col(1) {}

Address::Address(size_t _row, size_t _col) : row(_row), col(_col) {}

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
        if (!isdigit(s[i]))
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
        if (!isdigit(s[i]))
            return false;

        col *= 10;
        col += (s[i] - '0');
        ++i;
    }

    out.row = row;
    out.col = col;
    return true;
}

Number::Number() : wholeNumber(0), decimalNumber(0.0), type(NumberType::NONE) {}

Number::Number(int _wholeNumber) : wholeNumber(_wholeNumber), decimalNumber(0.0), type(NumberType::WHOLE_NUMBER) {}

Number::Number(double _decimalNumber) : wholeNumber(0), decimalNumber(_decimalNumber), type(NumberType::DECIMAL_NUMBER) {}

NumberType Number::getType() const { return type; }

double Number::getValue() const
{
    if (type == NumberType::WHOLE_NUMBER)
        return (double)wholeNumber;
    else if (type == NumberType::DECIMAL_NUMBER)
        return decimalNumber;

    return 0.0;
}

Number Number::operator+(Number rhs) { return getValue() + rhs.getValue(); }

Number Number::operator-(Number rhs) { return getValue() - rhs.getValue(); }

Number Number::operator*(Number rhs) { return getValue() * rhs.getValue(); }

Number Number::operator/(Number rhs)
{
    if (rhs.getValue() == 0.0)
        return Number();

    return getValue() / rhs.getValue();
}

Number Number::operator^(Number rhs) { return std::pow(getValue(), rhs.getValue()); }

bool Number::parseNumber(const std::string &str, Number &number)
{
    number = Number();

    if (str.empty())
        return false;

    size_t start = 0;
    if (str[0] == '-' || str[0] == '+')
    {
        if (str.size() == 1)
            return false;

        start = 1;
    }

    int dots = 0;
    for (size_t i = start; i < str.size(); ++i)
    {
        if (!isdigit(str[i]) && str[i] != '.')
            return false;

        if (str[i] == '.')
            ++dots;

        if (dots > 1)
            return false;
    }

    if (str == ".")
        return false;

    bool containsDigit = false;
    for (char c : str)
    {
        if (isdigit(c))
        {
            containsDigit = true;
            break;
        }
    }

    if (!containsDigit)
        return false;

    if (dots == 0)
    {
        number.type = NumberType::WHOLE_NUMBER;
        number.wholeNumber = std::stoi(str);
    }
    else
    {
        number.type = NumberType::DECIMAL_NUMBER;
        number.decimalNumber = std::stod(str);
    }

    return true;
}

std::ostream &operator<<(std::ostream &out, const Number &n)
{
    if (n.getType() == NumberType::WHOLE_NUMBER)
        out << n.wholeNumber;
    else if (n.getType() == NumberType::DECIMAL_NUMBER)
        out << n.decimalNumber;
    else
        out << "0";

    return out;
}