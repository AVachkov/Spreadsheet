#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include <iostream>
#include <string>

enum class CellType
{
    NONE,
    ERROR,
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
    Address(size_t _row, size_t _col);
    Address &operator=(const Address &rhs);
    bool operator==(const Address &rhs); // TODO

    static bool isAddress(const std::string &s, Address &out);
    // static Address parseAddress(const std::string &s); // use this inside isAddress
};

enum class NumberType
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

    static bool parseNumber(const std::string &str, Number &number); // remove static

  private:
    NumberType type;
};

std::ostream &operator<<(std::ostream &out, const Number &n);

#endif