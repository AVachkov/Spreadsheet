#include "cell.h"
#include <iostream>

Cell::Cell(Address _address) : wholeNumber(0), decimalNumber(0.0), text(""),
                               formula(), definedType(DefinedType::NONE), address(_address)
{
}

Cell::Cell(Address _address, int _wholeNumber) : wholeNumber(_wholeNumber), decimalNumber(0.0),
                                                 text(""), formula(), definedType(DefinedType::WHOLE_NUMBER),
                                                 address(_address)
{
}

Cell::Cell(Address _address, double _decimalNumber) : wholeNumber(0), decimalNumber(_decimalNumber),
                                                      text(""), formula(), definedType(DefinedType::DECIMAL_NUMBER),
                                                      address(_address)
{
}

Cell::Cell(Address _address, const std::string &_text) : wholeNumber(0), decimalNumber(0.0),
                                                         text(_text), formula(), definedType(DefinedType::TEXT),
                                                         address(_address)
{
}

Cell::Cell(Address _address, const Formula &_formula) : wholeNumber(0), decimalNumber(0.0),
                                                        text(""), formula(_formula), definedType(DefinedType::FORMULA),
                                                        address(_address)
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
    switch (cell.definedType)
    {
    case DefinedType::WHOLE_NUMBER:
        out << cell.wholeNumber;
        break;
    case DefinedType::DECIMAL_NUMBER:
        out << cell.decimalNumber;
        break;
    case DefinedType::TEXT:
        out << cell.text;
        break;
    case DefinedType::FORMULA:
        // out << cell.formula;
        break;
    default:
        break;
    }
    return out;
}