#include "formula.h"
#include "cell.h"
#include "baseTypes.h"
#include "utilities.h"
#include <vector>
#include <string>
#include <iostream>
#include <cctype>

Formula::Formula() : formula(""), cells(nullptr), result(), is_valid(false)
{
}

Formula::Formula(const std::string &_formula, std::vector<std::vector<Cell>> *_cells) : formula("ERROR"), cells(_cells), result(), is_valid(false)
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
    clearUnnecessaryWhitespaces(f);
    if (f.empty() || f.size() < 2)
        // if shorter than 2 chars - return, because the shortest correct formula is "=n" where n∈ℝ.
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
        else
        {
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
                if (!Number::parseNumber(data, n))
                    return false;
            }

            numbers.push_back(n);
            data.clear();
        }
    }

    if (processOperations(numbers, operators))
    {
        outRes = numbers[0];
        return true;
    }
    return false;
}

void Formula::clearUnnecessaryWhitespaces(std::string &s)
{
    size_t i = 0;
    while (i < s.size())
    {
        if (isdigit(s[i]))
        // skip whitespaces between digits
        {
            bool isThereOperatorInBetween = false;
            int charsBetweenDigits = 0;
            size_t j = i + 1;
            while (j < s.size() && !isdigit(s[j]))
            {
                if (Formula::isOperator(s[j]))
                {
                    isThereOperatorInBetween = true;
                }

                ++j;
                ++charsBetweenDigits;
            }

            if (charsBetweenDigits > 0 && !isThereOperatorInBetween)
            {
                i = j;
            }
        }

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
        case CellType::WHOLE_NUMBER:
            n = c.getWholeNumber();
            break;
        case CellType::DECIMAL_NUMBER:
            n = c.getDecimalNumber();
            break;
        case CellType::FORMULA:
            n = c.getFormulaResult();
            break;
        case CellType::TEXT:
        case CellType::NONE:
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

bool Formula::processOperations(
    std::vector<Number> &numbers_from_expression,
    std::vector<char> &operators_from_expression)
{
    if (numbers_from_expression.size() == 1 && operators_from_expression.size() == 0)
        return true;

    if (numbers_from_expression.size() - operators_from_expression.size() != 1)
        return false;

    return executeSingleOperator('^', [](Number n1, Number n2)
                                 { return n1 ^ n2; }, numbers_from_expression, operators_from_expression) &&
           executeMultipleOperators({'*', '/'}, {[](Number n1, Number n2)
                                                 { return n1 * n2; }, [](Number n1, Number n2)
                                                 { return n1 / n2; }},
                                    numbers_from_expression, operators_from_expression) &&
           executeSingleOperator('-', [](Number n1, Number n2)
                                 { return n1 - n2; }, numbers_from_expression, operators_from_expression) &&
           executeSingleOperator('+', [](Number n1, Number n2)
                                 { return n1 + n2; }, numbers_from_expression, operators_from_expression);
}

bool Formula::executeSingleOperator(
    char op,
    const binaryFunction &binOp,
    std::vector<Number> &numbers_from_expression,
    std::vector<char> &operators_from_expression)
{
    if (!isOperator(op))
        return false;

    size_t i = 0;
    while (i < operators_from_expression.size())
    {
        if (operators_from_expression[i] == op)
        {
            numbers_from_expression[i] = binOp(numbers_from_expression[i], numbers_from_expression[i + 1]);
            if (numbers_from_expression[i].getType() == NumberType::NONE)
            {
                return false;
            }
            numbers_from_expression.erase(numbers_from_expression.begin() + (i + 1));
            operators_from_expression.erase(operators_from_expression.begin() + i);
        }
        else
        {
            ++i;
        }
    }
    return true;
}

bool Formula::executeMultipleOperators(
    const std::vector<char> &operators_to_execute,
    const std::vector<binaryFunction> &functions_to_execute,
    std::vector<Number> &numbers_from_expression,
    std::vector<char> &operators_from_expression)
{
    for (char op : operators_to_execute)
    {
        if (!isOperator(op))
            return false;
    }

    if (operators_to_execute.size() != functions_to_execute.size())
        return false;

    size_t i = 0;
    while (i < operators_from_expression.size())
    {
        char currentOpFromExpr = operators_from_expression[i];
        int index_of_op_and_func = -1;
        for (size_t j = 0; j < operators_to_execute.size(); ++j)
        {
            if (currentOpFromExpr == operators_to_execute[j])
            {
                index_of_op_and_func = j;
            }
        }

        if (index_of_op_and_func != -1)
        {
            const binaryFunction &func = functions_to_execute[index_of_op_and_func];
            numbers_from_expression[i] = func(numbers_from_expression[i], numbers_from_expression[i + 1]);
            if (numbers_from_expression[i].getType() == NumberType::NONE)
            {
                return false;
            }
            numbers_from_expression.erase(numbers_from_expression.begin() + (i + 1));
            operators_from_expression.erase(operators_from_expression.begin() + i);
        }
        else
        {
            ++i;
        }
    }
    return true;
}

std::ostream &operator<<(std::ostream &out, const Formula &formula)
{
    out << formula.getResult();
    return out;
}