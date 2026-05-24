#ifndef CELL_H
#define CELL_H

#include "baseTypes.h"
#include "formula.h"
#include <iostream>
#include <string>

class Cell {
  public:
    Cell(const Cell &original) = default;
    Cell &operator=(const Cell &rhs) = default;
    ~Cell() = default;

    Cell(Address _address);
    Cell(Address _address, int _wholeNumber);
    Cell(Address _address, double _decimalNumber);
    Cell(Address _address, const std::string &_text);
    Cell(Address _address, const Formula &_formula);

    int getWholeNumber() const;
    double getDecimalNumber() const;
    std::string getText() const;
    const Formula &getFormula() const;
    CellType getType() const;

    void markError();
    Number getNumericValue() const;

    Cell &operator=(const Formula &rhs);

  private:
    int wholeNumber;
    double decimalNumber;
    std::string text;
    Formula formula;

    CellType definedType;
    Address address;
};

std::ostream &operator<<(std::ostream &out, const Cell &cell);

#endif