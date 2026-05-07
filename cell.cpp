#include "cell.h"
#include <iostream>

Cell::Cell() : wholeNumber(0), decimalNumber(0.0), text(""), /*formula(),*/ definedType(DefinedType::NONE)
{
}

Cell::Cell(int _wholeNumber) : wholeNumber(_wholeNumber), decimalNumber(0.0), text(""), /*formula(),*/ definedType(DefinedType::WHOLE_NUMBER)
{
}

Cell::Cell(double _decimalNumber) : wholeNumber(0), decimalNumber(_decimalNumber), text(""), /*formula(),*/ definedType(DefinedType::DECIMAL_NUMBER)
{
}

Cell::Cell(const std::string &_text) : wholeNumber(0), decimalNumber(0.0), text(_text), /*formula(),*/ definedType(DefinedType::TEXT)
{
}

Cell::Cell(const Formula &_formula)
{
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
