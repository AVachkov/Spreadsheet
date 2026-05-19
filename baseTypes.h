#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include <string>
#include <iostream>

enum class DefinedType // rename to CellDefinedType or CellType
{
    NONE,
    WHOLE_NUMBER,
    DECIMAL_NUMBER,
    TEXT,
    FORMULA
};

struct Address // stored as actual spreadsheet indices (starting from 1).
{
    size_t row;
    size_t col;

    Address();
    Address(size_t, size_t);
    Address &operator=(const Address &);

    static bool isAddress(const std::string &, Address &);
};

enum NumberType
{
    NONE,
    WHOLE_NUMBER,
    DECIMAL_NUMBER
};

class Number
{
public:
    int wholeNumber;
    double decimalNumber;

    Number();
    Number(int _wholeNumber);
    Number(double _decimalNumber);
    Number(const std::string &s);
    double getValue() const;
    NumberType getType() const;

    Number &operator=(int w);
    Number &operator=(double d);

    Number operator+(Number rhs);
    Number operator-(Number rhs);
    Number operator*(Number rhs);
    Number operator/(Number rhs);
    Number operator^(Number rhs);

    static bool isNumber(const std::string &str, Number &number);
    static bool isDigit(char c);

private:
    NumberType type;
};

std::ostream &operator<<(std::ostream &out, const Number &n);

#endif