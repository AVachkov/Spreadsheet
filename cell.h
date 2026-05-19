#ifndef CELL_H
#define CELL_H

#include "formula.h"
#include "baseTypes.h"
#include <string>
#include <iostream>

class Cell
{
public:
    Cell(Address _address);
    Cell(Address _address, int _wholeNumber);
    Cell(Address _address, double _decimalNumber);
    Cell(Address _address, const std::string &_text);
    Cell(Address _address, const Number &n);

    int getWholeNumber() const;
    double getDecimalNumber() const;
    std::string getText() const;
    Formula getFormula() const;
    DefinedType getType() const;

private:
    int wholeNumber;
    double decimalNumber;
    std::string text;
    Formula formula;

    DefinedType definedType;
    Address address;
};

std::ostream &operator<<(std::ostream &out, const Cell &cell);

#endif