#include "cell.h"

Cell::Cell(Address _address) : address(_address) {}

Cell::~Cell() {}

Address Cell::getAddress() const { return address; }

WholeNumberCell::WholeNumberCell(Address _address, int _wholeNumber) : Cell(_address), whole_number(_wholeNumber) {}

CellType WholeNumberCell::getType() const { return CellType::WHOLE_NUMBER; }

Number WholeNumberCell::getNumericValue() const { return Number(whole_number); }

void WholeNumberCell::print(std::ostream &out) const { out << whole_number; }

DecimalNumberCell::DecimalNumberCell(Address _address, double _decimalNumber)
    : Cell(_address), decimal_number(_decimalNumber) {}

CellType DecimalNumberCell::getType() const { return CellType::DECIMAL_NUMBER; }

Number DecimalNumberCell::getNumericValue() const { return Number(decimal_number); }

void DecimalNumberCell::print(std::ostream &out) const { out << decimal_number; }

TextCell::TextCell(Address _address, const std::string &_text) : Cell(_address), text(_text) {}

CellType TextCell::getType() const { return CellType::TEXT; }

Number TextCell::getNumericValue() const { return Number(0); }

void TextCell::print(std::ostream &out) const { out << text; }

std::string TextCell::getText() const { return text; }

FormulaCell::FormulaCell(Address _address, const Formula &_formula) : Cell(_address), formula(_formula) {}

CellType FormulaCell::getType() const { return CellType::FORMULA; }

Number FormulaCell::getNumericValue() const { return formula.getResult(); }

void FormulaCell::print(std::ostream &out) const { out << formula.getResult(); }

Formula FormulaCell::getFormula() const { return formula; }

EmptyCell::EmptyCell(Address _address, const std::string &_status) : Cell(_address), status(_status) {}

CellType EmptyCell::getType() const { return CellType::NONE; }

Number EmptyCell::getNumericValue() const { return Number(0); }

void EmptyCell::print(std::ostream &out) const { out << ""; }

std::string EmptyCell::getStatus() const { return status; }

std::ostream &operator<<(std::ostream &out, const Cell *cell) {
    cell->print(out);
    return out;
}

ErrorCell::ErrorCell(Address _address) : Cell(_address) {}

CellType ErrorCell::getType() const { return CellType::ERROR; }

Number ErrorCell::getNumericValue() const { return Number(0); }

void ErrorCell::print(std::ostream &out) const { out << "ERROR"; }
