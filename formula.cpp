#include "formula.h"
#include "cell.h"
#include "baseTypes.h"
#include "utilities.h"
#include <vector>
#include <string>
#include <iostream>

Formula::Formula() : formula(""), cells(nullptr), result(), is_valid(false)
{
}

Formula::Formula(const std::string &_formula, std::vector<std::vector<Cell>> *_cells) : formula(""), cells(_cells), result(), is_valid(false)
{
    if (solveFormula(_formula, cells, result))
    {
        formula = _formula;
        is_valid = true;
    }
}

Number Formula::getResult() const
{
    return result;
}

bool Formula::isValid() const
{
    return is_valid;
}

bool Formula::solveFormula(const std::string &_formula, std::vector<std::vector<Cell>> *_cells, Number &outRes)
{
    outRes = Number();

    std::string f = _formula;
    clearAllWhitespaces(f);
    if (f.empty() || f.size() < 4)
        // if shorter than 4 chars - return, because the shortest correct formula is "=a␣SYM␣b", where a,b∈ℝ, SYM∈{+,-,*,/,^}.
        return false;

    if (f[0] != '=')
        return false;

    std::vector<Number> numbers;
    std::vector<char> operators;

    size_t i = 1;
    if (isOperator(_formula[1]))
    // handle leading operator for the first number (e.g. -5, +12)
    {
        if (f[1] != '+' && f[1] != '-')
            return false;

        bool isNegative = f[1] == '-' ? true : false;
        ++i;

        std::string data;
        while (i < f.size() && !isOperator(f[i]))
        {
            data += f[i];
            ++i;
        }
        Number n(data);
        if (isNegative)
            n = n * -1;

        numbers.push_back(n);
    }

    while (i < f.size())
    {
        if (isOperator(f[i]))
        {
            operators.push_back(f[i]);
            ++i;
        }

        std::string data;
        while (i < f.size() && !isOperator(f[i]))
        {
            data += f[i];
            ++i;
        }

        Number n;
        Address address;
        if (Address::isAddress(data, address))
        {
            if (!parseAddress(address, n))
                return false;
        }
        else
        {
            Number::isNumber(data, n);
        }

        numbers.push_back(n);
        data.clear();
    }

    if (processOperations(numbers, operators))
    {
        outRes = numbers[0];
        return true;
    }
    return false;
}

void Formula::clearAllWhitespaces(std::string &s)
{
    size_t i = 0;
    while (i < s.size())
    {
        if (isWhitespace(s[i]))
            s.erase(i, 1);
        else
            ++i;
    }
}

bool Formula::isOperator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

bool Formula::parseAddress(Address address, Number &n) const
{
    if (isAddressValid(address))
    {
        const Cell &c = (*cells)[address.row - 1][address.col - 1]; // address stored as actual spreadsheet indices (starting from 1).
        switch (c.getType())
        {
        case DefinedType::WHOLE_NUMBER:
            n = c.getWholeNumber();
            break;
        case DefinedType::DECIMAL_NUMBER:
            n = c.getDecimalNumber();
            break;
        case DefinedType::FORMULA:
            if (c.getFormula().result.getType() == NumberType::WHOLE_NUMBER)
            {
                n = c.getFormula().result.wholeNumber;
            }
            else if (c.getFormula().result.getType() == NumberType::DECIMAL_NUMBER)
            {
                n = c.getFormula().result.decimalNumber;
            }
            break;
        case DefinedType::TEXT:
        case DefinedType::NONE:
            n = 0;
        }
        return true;
    }
    return false;
}

bool Formula::isAddressValid(Address a) const
{
    return cells != nullptr &&
           a.row > 0 && (a.row - 1) < cells->size() &&
           a.col > 0 && (a.col - 1) < (*cells)[a.row - 1].size();
}

bool Formula::processOperations(std::vector<Number> &numbers, std::vector<char> &operators)
{
    // 2 + 3 * 4^2 - 10/5
    // numbers: 2, 3, 4, 2, 10, 5
    // operators: +, *, ^, -, /

    // 2 + 3 * 16 - 10/5
    // numbers: 2, 3, 16, 10, 5
    // operators: +, *, -, /

    if (numbers.size() - operators.size() != 1)
        return false;

    executeSingleOperation('^', [](Number n1, Number n2)
                           { return n1 ^ n2; }, numbers, operators);
    executeSingleOperation('*', [](Number n1, Number n2)
                           { return n1 * n2; }, numbers, operators);
    executeSingleOperation('/', [](Number n1, Number n2)
                           { return n1 / n2; }, numbers, operators);
    executeSingleOperation('+', [](Number n1, Number n2)
                           { return n1 + n2; }, numbers, operators);
    executeSingleOperation('-', [](Number n1, Number n2)
                           { return n1 - n2; }, numbers, operators);

    return true;
}

void Formula::executeSingleOperation(char op, const std::function<Number(Number, Number)> &binOp, std::vector<Number> &numbers, std::vector<char> &operators) // better do it with std::list
{
    if (!isOperator(op))
        return;

    size_t i = 0;
    while (i < operators.size())
    {
        if (operators[i] == op)
        {
            numbers[i] = binOp(numbers[i], numbers[i + 1]);
            numbers.erase(numbers.begin() + (i + 1));
            operators.erase(operators.begin() + i);
        }
        ++i;
    }
}

std::ostream &operator<<(std::ostream &out, const Formula &formula)
{
    out << formula.getResult();
    return out;
}