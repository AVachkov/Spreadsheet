#ifndef CELL_H
#define CELL_H

#include <string>
#include "formula.h"
#include <iostream>

enum class DefinedType
{
    NONE,
    WHOLE_NUMBER,
    DECIMAL_NUMBER,
    TEXT,
    FORMULA
};

class Cell
{
public:
    Cell();
    Cell(int);
    Cell(double);
    Cell(const std::string &);
    Cell(const Formula &);

    friend std::ostream &
    operator<<(std::ostream &, const Cell &);

private:
    int wholeNumber;
    double decimalNumber;
    std::string text;
    Formula formula;
    DefinedType definedType;
};

std::ostream &operator<<(std::ostream &, const Cell &);

#endif