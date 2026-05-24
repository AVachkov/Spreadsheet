#ifndef CELL_H
#define CELL_H

#include "baseTypes.h"
#include "formula.h"
#include <iostream>
#include <string>

class Cell {
  public:
    Cell(Address _address);
    Cell(const Cell &original) = default;
    Cell &operator=(const Cell &rhs) = default;
    virtual ~Cell();

    virtual CellType getType() const = 0;
    virtual Number getNumericValue() const = 0;
    virtual void print(std::ostream &out) const = 0;

    Address getAddress() const;

  protected:
    Address address;
};

class WholeNumberCell : public Cell {
  public:
    WholeNumberCell(Address _address, int _wholeNumber);
    CellType getType() const override;
    Number getNumericValue() const override;
    void print(std::ostream &out) const override;

  private:
    int whole_number;
};

class DecimalNumberCell : public Cell {
  public:
    DecimalNumberCell(Address _address, double _decimalNumber);
    CellType getType() const override;
    Number getNumericValue() const override;
    void print(std::ostream &out) const override;

  private:
    double decimal_number;
};

class TextCell : public Cell {
  public:
    TextCell(Address _address, const std::string &_text);
    CellType getType() const override;
    Number getNumericValue() const override;
    void print(std::ostream &out) const override;

    std::string getText() const;

  private:
    std::string text;
};

class FormulaCell : public Cell {
  public:
    FormulaCell(Address _address, const Formula &_formula);
    CellType getType() const override;
    Number getNumericValue() const override;
    void print(std::ostream &out) const override;

    Formula getFormula() const;

  private:
    Formula formula;
};

class EmptyCell : public Cell {
  public:
    EmptyCell(Address _address, const std::string &_status = "");
    CellType getType() const override;
    Number getNumericValue() const override;
    void print(std::ostream &out) const override;

    std::string getStatus() const;

  private:
    std::string status;
};

class ErrorCell : public Cell {
  public:
    ErrorCell(Address _address);
    CellType getType() const override;
    Number getNumericValue() const override;
    void print(std::ostream &out) const override;
};

std::ostream &operator<<(std::ostream &out, const Cell *cell);

#endif