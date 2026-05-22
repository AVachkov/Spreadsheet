#include "cell.h"
#include "formula.h"
#include <iostream>

Cell::Cell(Address _address) : wholeNumber(0), decimalNumber(0.0), text(""), formulaResult(), definedType(CellType::NONE), address(_address), containsAddress(false), formula("")
{
}

Cell::Cell(Address _address, int _wholeNumber) : wholeNumber(_wholeNumber), decimalNumber(0.0),
                                                 text(""), formulaResult(), definedType(CellType::WHOLE_NUMBER),
                                                 address(_address), containsAddress(false), formula("")
{
}

Cell::Cell(Address _address, double _decimalNumber) : wholeNumber(0), decimalNumber(_decimalNumber),
                                                      text(""), formulaResult(), definedType(CellType::DECIMAL_NUMBER), address(_address),
                                                      containsAddress(false), formula("")
{
}

Cell::Cell(Address _address, const std::string &_text) : wholeNumber(0), decimalNumber(0.0),
                                                         text(_text), formulaResult(), definedType(CellType::TEXT),
                                                         address(_address), containsAddress(false), formula("")
{
}

Cell::Cell(Address _address, const Number &n) : wholeNumber(0), decimalNumber(0.0),
                                                text(""), formulaResult(n), definedType(CellType::FORMULA),
                                                address(_address), containsAddress(false), formula("")
{
}

int Cell::getWholeNumber() const
{
    return wholeNumber;
}

double Cell::getDecimalNumber() const
{
    return decimalNumber;
}

std::string Cell::getText() const
{
    return text;
}

Number Cell::getFormulaResult() const
{
    return formulaResult;
}

CellType Cell::getType() const
{
    return definedType;
}

void Cell::setContainsAddress(bool flag)
{
    containsAddress = flag;
}

void Cell::setFormula(std::string f)
{
    formula = f;
}

std::ostream &operator<<(std::ostream &out, const Cell &cell)
{
    switch (cell.getType())
    {
    case CellType::WHOLE_NUMBER:
        out << cell.getWholeNumber();
        break;
    case CellType::DECIMAL_NUMBER:
        out << cell.getDecimalNumber();
        break;
    case CellType::TEXT:
        out << cell.getText();
        break;
    case CellType::FORMULA:
        out << cell.getFormulaResult();
        break;
    default:
        break;
    }
    return out;
}