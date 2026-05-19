#include "cell.h"
#include "formula.h"
#include <iostream>

Cell::Cell(Address _address) : wholeNumber(0), decimalNumber(0.0), text(""), definedType(DefinedType::NONE), address(_address)
{
}

Cell::Cell(Address _address, int _wholeNumber) : wholeNumber(_wholeNumber), decimalNumber(0.0),
                                                 text(""), definedType(DefinedType::WHOLE_NUMBER),
                                                 address(_address)
{
}

Cell::Cell(Address _address, double _decimalNumber) : wholeNumber(0), decimalNumber(_decimalNumber),
                                                      text(""), definedType(DefinedType::DECIMAL_NUMBER),
                                                      address(_address)
{
}

Cell::Cell(Address _address, const std::string &_text) : wholeNumber(0), decimalNumber(0.0),
                                                         text(_text), definedType(DefinedType::TEXT),
                                                         address(_address)
{
}

Cell::Cell(Address _address, const Number &n) : wholeNumber(0), decimalNumber(0.0),
                                                text(""), definedType(DefinedType::FORMULA),
                                                address(_address)
{
    if (n.getType() == NumberType::WHOLE_NUMBER || n.getType() == NumberType::DECIMAL_NUMBER)
        wholeNumber = n.getValue();
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

Formula Cell::getFormula() const
{
    return formula;
}

DefinedType Cell::getType() const
{
    return definedType;
}

std::ostream &operator<<(std::ostream &out, const Cell &cell)
{
    switch (cell.getType())
    {
    case DefinedType::WHOLE_NUMBER:
        out << cell.getWholeNumber();
        break;
    case DefinedType::DECIMAL_NUMBER:
        out << cell.getDecimalNumber();
        break;
    case DefinedType::TEXT:
        out << cell.getText();
        break;
    case DefinedType::FORMULA:
        out << cell.getFormula();
        break;
    default:
        break;
    }
    return out;
}