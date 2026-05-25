#ifndef CELL_H
#define CELL_H

#include "baseTypes.h"
#include "formula.h"
#include <iostream>
#include <string>

class Cell
{
  public:
    Cell(Address _address);
    Cell(const Cell &original) = default;
    Cell &operator=(const Cell &rhs) = default;
    virtual ~Cell();

    virtual Cell *clone() const = 0;
    virtual CellType getType() const = 0;
    virtual Number getNumericValue() const = 0;
    virtual void print(std::ostream &out) const = 0;
    virtual size_t getContentLength() const = 0;

    Address getAddress() const;

  protected:
    Address address;
};

class WholeNumberCell : public Cell
{
  public:
    WholeNumberCell(Address _address, int _wholeNumber);
    WholeNumberCell(const WholeNumberCell &original) = default;
    WholeNumberCell &operator=(const WholeNumberCell &rhs) = default;

    Cell *clone() const override;
    CellType getType() const override;
    Number getNumericValue() const override;
    void print(std::ostream &out) const override;
    size_t getContentLength() const override;

  private:
    int whole_number;
};

class DecimalNumberCell : public Cell
{
  public:
    DecimalNumberCell(Address _address, double _decimalNumber);
    DecimalNumberCell(const DecimalNumberCell &original) = default;
    DecimalNumberCell &operator=(const DecimalNumberCell &rhs) = default;

    Cell *clone() const override;
    CellType getType() const override;
    Number getNumericValue() const override;
    void print(std::ostream &out) const override;
    size_t getContentLength() const override;

  private:
    double decimal_number;
    std::string getFormattedString() const;
};

class TextCell : public Cell
{
  public:
    TextCell(Address _address, const std::string &_text);
    TextCell(const TextCell &original) = default;
    TextCell &operator=(const TextCell &rhs) = default;

    Cell *clone() const override;
    CellType getType() const override;
    Number getNumericValue() const override;
    void print(std::ostream &out) const override;
    size_t getContentLength() const override;

    std::string getText() const;

  private:
    std::string text;
};

class FormulaCell : public Cell
{
  public:
    FormulaCell(Address _address, const Formula &_formula);
    FormulaCell(const FormulaCell &original) = default;
    FormulaCell &operator=(const FormulaCell &rhs) = default;

    Cell *clone() const override;
    CellType getType() const override;
    Number getNumericValue() const override;
    void print(std::ostream &out) const override;
    size_t getContentLength() const override;

    Formula getFormula() const;

  private:
    Formula formula;
};

class EmptyCell : public Cell
{
  public:
    EmptyCell(Address _address, const std::string &_status = "");
    EmptyCell(const EmptyCell &original) = default;
    EmptyCell &operator=(const EmptyCell &rhs) = default;

    Cell *clone() const override;
    CellType getType() const override;
    Number getNumericValue() const override;
    void print(std::ostream &out) const override;
    size_t getContentLength() const override;

    std::string getStatus() const;

  private:
    std::string status;
};

class ErrorCell : public Cell
{
  public:
    ErrorCell(Address _address);
    ErrorCell(const ErrorCell &original) = default;
    ErrorCell &operator=(const ErrorCell &rhs) = default;

    Cell *clone() const override;
    CellType getType() const override;
    Number getNumericValue() const override;
    void print(std::ostream &out) const override;
    size_t getContentLength() const override;
};

std::ostream &operator<<(std::ostream &out, const Cell *cell);

#endif