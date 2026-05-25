#include "cell.h"

Cell::Cell(Address _address) : address(_address) {}

Cell::~Cell() {}

Address Cell::getAddress() const { return address; }

WholeNumberCell::WholeNumberCell(Address _address, int _wholeNumber) : Cell(_address), whole_number(_wholeNumber) {}

Cell *WholeNumberCell::clone() const { return new WholeNumberCell(*this); }

CellType WholeNumberCell::getType() const { return CellType::WHOLE_NUMBER; }

Number WholeNumberCell::getNumericValue() const { return Number(whole_number); }

void WholeNumberCell::print(std::ostream &out) const { out << whole_number; }

size_t WholeNumberCell::getContentLength() const
{
    if (whole_number == 0)
        return 1;

    size_t res = whole_number < 0 ? 1 : 0;
    int copy = whole_number;
    while (copy)
    {
        ++res;
        copy /= 10;
    }

    return res;
}

DecimalNumberCell::DecimalNumberCell(Address _address, double _decimalNumber) : Cell(_address), decimal_number(_decimalNumber) {}

Cell *DecimalNumberCell::clone() const { return new DecimalNumberCell(*this); }

CellType DecimalNumberCell::getType() const { return CellType::DECIMAL_NUMBER; }

Number DecimalNumberCell::getNumericValue() const { return Number(decimal_number); }

void DecimalNumberCell::print(std::ostream &out) const { out << getFormattedString(); }

size_t DecimalNumberCell::getContentLength() const { return getFormattedString().length(); }

std::string DecimalNumberCell::getFormattedString() const // заради по-големи числа и научен запис на числа в поток
{
    std::string s = std::to_string(decimal_number);

    while (s.length() > 0 && s[s.length() - 1] == '0')
    {
        s.pop_back();
    }

    if (s.length() > 0 && s[s.length() - 1] == '.')
    {
        s.pop_back();
    }

    return s;
}

TextCell::TextCell(Address _address, const std::string &_text) : Cell(_address), text(_text) {}

Cell *TextCell::clone() const { return new TextCell(*this); }

CellType TextCell::getType() const { return CellType::TEXT; }

Number TextCell::getNumericValue() const { return Number(0); }

void TextCell::print(std::ostream &out) const { out << text; }

size_t TextCell::getContentLength() const // заради текст на кирилица
{
    size_t length = 0;

    for (size_t i = 0; i < text.length(); ++i)
    {
        char c = text[i];

        if ((c & 0xC0) != 0x80)
        {
            length++;
        }
    }

    return length;
}

std::string TextCell::getText() const { return text; }

FormulaCell::FormulaCell(Address _address, const Formula &_formula) : Cell(_address), formula(_formula) {}

Cell *FormulaCell::clone() const { return new FormulaCell(*this); }

CellType FormulaCell::getType() const { return CellType::FORMULA; }

Number FormulaCell::getNumericValue() const { return formula.getResult(); }

void FormulaCell::print(std::ostream &out) const // заради по-големи числа и научен запис на числа в поток
{
    switch (formula.getResult().getType())
    {
        case NumberType::WHOLE_NUMBER:
        {
            out << formula.getResult().wholeNumber;
            break;
        }
        case NumberType::DECIMAL_NUMBER:
        {
            DecimalNumberCell dnc(Address(), formula.getResult().decimalNumber);
            dnc.print(out);
            break;
        }
        default:
        {
            out << formula.getResult();
            break;
        }
    }
}

size_t FormulaCell::getContentLength() const
{
    switch (formula.getResult().getType())
    {
        case NumberType::WHOLE_NUMBER:
        {
            WholeNumberCell wnc(Address(), formula.getResult().wholeNumber);
            return wnc.getContentLength();
        }
        case NumberType::DECIMAL_NUMBER:
        {
            DecimalNumberCell dnc(Address(), formula.getResult().decimalNumber);
            return dnc.getContentLength();
        }
        default:
            return 1;
    }
    return 1;
}

Formula FormulaCell::getFormula() const { return formula; }

EmptyCell::EmptyCell(Address _address, const std::string &_status) : Cell(_address), status(_status) {}

Cell *EmptyCell::clone() const { return new EmptyCell(*this); }

CellType EmptyCell::getType() const { return CellType::NONE; }

Number EmptyCell::getNumericValue() const { return Number(0); }

void EmptyCell::print(std::ostream &out) const { out << ""; }

size_t EmptyCell::getContentLength() const { return 0; }

std::string EmptyCell::getStatus() const { return status; }

std::ostream &operator<<(std::ostream &out, const Cell *cell)
{
    cell->print(out);
    return out;
}

ErrorCell::ErrorCell(Address _address) : Cell(_address) {}

Cell *ErrorCell::clone() const { return new ErrorCell(*this); }

CellType ErrorCell::getType() const { return CellType::ERROR; }

Number ErrorCell::getNumericValue() const { return Number(0); }

void ErrorCell::print(std::ostream &out) const { out << "ERROR"; }

size_t ErrorCell::getContentLength() const
{
    const size_t ERROR_MESSAGE_SIZE = 5;
    return ERROR_MESSAGE_SIZE;
}
