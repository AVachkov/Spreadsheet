#include "cell.h"
#include "formula.h"
#include <iostream>

Cell::Cell(Address _address)
    : wholeNumber(0), decimalNumber(0.0), text(""), formula(""), definedType(CellType::NONE), address(_address) {}

Cell::Cell(Address _address, int _wholeNumber)
    : wholeNumber(_wholeNumber), decimalNumber(0.0), text(""), formula(""), definedType(CellType::WHOLE_NUMBER),
      address(_address) {}

Cell::Cell(Address _address, double _decimalNumber)
    : wholeNumber(0), decimalNumber(_decimalNumber), text(""), formula(""), definedType(CellType::DECIMAL_NUMBER),
      address(_address) {}

Cell::Cell(Address _address, const std::string &_text)
    : wholeNumber(0), decimalNumber(0.0), text(_text), formula(""), definedType(CellType::TEXT), address(_address) {}

Cell::Cell(Address _address, const Formula &_formula)
    : wholeNumber(0), decimalNumber(0.0), text(), formula(_formula), definedType(CellType::FORMULA), address(_address) {
}

// Cell::Cell(Address _address, const Number &n) : wholeNumber(0), decimalNumber(0.0),
//                                                 text(""), formula(n), definedType(CellType::FORMULA),
//                                                 address(_address)
// {
// }

int Cell::getWholeNumber() const { return wholeNumber; }

double Cell::getDecimalNumber() const { return decimalNumber; }

std::string Cell::getText() const { return text; }

const Formula &Cell::getFormula() const { return formula; }

// Number Cell::getFormulaResult() const
// {
//     return formulaResult;
// }

CellType Cell::getType() const { return definedType; }

// bool Cell::containsAddress() const
// {
//     return contains_address;
// }

// void Cell::setContainsAddress(bool flag)
// {
//     contains_address = flag;
// }

// void Cell::setFormula(std::string f)
// {
//     text = f;
//     definedType = CellType::FORMULA;
// }

void Cell::markError() {
    wholeNumber = 0;
    decimalNumber = 0.0;
    text = "";
    formula = Formula("");
    definedType = CellType::NONE;
    address = Address();
}

Number Cell::getNumericValue() const {
    switch (definedType) {
    case CellType::WHOLE_NUMBER:
        return wholeNumber;
    case CellType::DECIMAL_NUMBER:
        return decimalNumber;
    case CellType::TEXT:
    case CellType::FORMULA:
        if (formula.isValid())
            return formula.getResult();
        return Number(0);
    default:
        return Number(0);
    }
    return Number(0);
}

Cell &Cell::operator=(const Formula &rhs) {
    text.clear();
    wholeNumber = 0;
    decimalNumber = 0.0;
    definedType = CellType::FORMULA;
    formula = rhs;
    return *this;
}

std::ostream &operator<<(std::ostream &out, const Cell &cell) {
    switch (cell.getType()) {
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
        out << cell.getFormula().getResult();
        break;
    default:
        break;
    }
    return out;
}