#ifndef CELL_H
#define CELL_H

#include "formula.h"
#include "baseTypes.h"
#include "functions.h"
#include <string>
#include <iostream>

class Cell
{
public:
    Cell(Address);
    Cell(Address, int);
    Cell(Address, double);
    Cell(Address, const std::string &);
    Cell(Address, const Formula &);

    int getWholeNumber() const;
    double getDecimalNumber() const;
    std::string getText() const;
    Formula getFormula() const;
    DefinedType getType() const;

    friend std::ostream &operator<<(std::ostream &, const Cell &);

private:
    int wholeNumber;
    double decimalNumber;
    std::string text;
    Formula formula;

    DefinedType definedType;
    Address address;
};

#endif