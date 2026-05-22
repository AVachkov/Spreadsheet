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
    Number getFormulaResult() const;
    CellType getType() const;

    void setContainsAddress(bool flag);
    void setFormula(std::string f);

private:
    // Number number
    int wholeNumber;
    double decimalNumber;
    std::string text;
    std::string formula;
    Number formulaResult;

    CellType definedType;
    Address address;
    bool containsAddress;
};

std::ostream &operator<<(std::ostream &out, const Cell &cell);

#endif