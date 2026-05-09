#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include <string>

enum class DefinedType
{
    NONE,
    WHOLE_NUMBER,
    DECIMAL_NUMBER,
    TEXT,
    FORMULA
};

struct Address
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
    Number(int);
    Number(double);
    double getValue() const;
    NumberType getType() const;

    Number &operator=(int);
    Number &operator=(double);

    Number operator+(Number);
    Number operator-(Number);
    Number operator*(Number);
    Number operator/(Number);
    Number operator^(Number);

    static bool isNumber(const std::string &, Number &);
    static bool isDigit(char);

private:
    NumberType type;
};

#endif