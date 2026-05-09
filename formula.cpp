#include "cell.h"
#include "formula.h"
#include "functions.h"
#include <vector>
#include <string>

Formula::Formula() : formula(""), cells(nullptr), result(), is_valid(false)
{
}

Formula::Formula(const std::string &_formula, std::vector<std::vector<Cell>> *_cells) : formula(""), cells(_cells), result(), is_valid(false)
{
    // formula = _formula;
    // is_valid = true;
}

bool Formula::solveFormula(const std::string &_formula, std::vector<std::vector<Cell>> *_cells)
{
    std::vector<Number> numbers;
    std::vector<char> operators;

    if (_formula.empty() || _formula.size() < 4)
        return false;

    if (_formula[0] != '=')
        return false;

    size_t i = 1;
    while (i < _formula.size())
    {
        if (isOperator(_formula[i]))
        {
            operators.push_back(_formula[i]);
            ++i;
        }

        std::string data;
        while (i < _formula.size() && !isOperator(_formula[i]))
        {
            data += _formula[i];
            ++i;
        }

        Functions::trim(data);

        Number n;
        n = 0;
        Address address;
        if (Address::isAddress(data, address))
        {
            if (isAddressValid(address))
            {
                const Cell &c = (*cells)[address.row - 1][address.col - 1];
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
                default:
                    break;
                }
            }
        }
        // else if (...) nested formula ?
        else
        {
            Number::isNumber(data, n);
        }

        numbers.push_back(n);
        data.clear();
    }

    if (processOperations(numbers, operators))
        return true;
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

    executeSingleOperation('^', power, numbers, operators);
    executeSingleOperation('*', multiplication, numbers, operators);
    executeSingleOperation('/', division, numbers, operators);
    executeSingleOperation('+', sum, numbers, operators);
    executeSingleOperation('-', difference, numbers, operators);

    return true;
}

void Formula::executeSingleOperation(char op, const std::function<Number(Number, Number)> &binOp, std::vector<Number> &numbers, std::vector<char> &operators) // better do it with std::list
{
    if (!isOperator(op))
        return;

    size_t i = 0;
    while (i < operators.size())
    {
        while (operators[i] != op)
        {
            ++i;
        }

        numbers[i] = binOp(numbers[i], numbers[i + 1]);
        numbers.erase(numbers.begin() + i + 1);
        operators.erase(operators.begin() + i);
    }
}

bool Formula::isOperator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

Number Formula::sum(Number n1, Number n2)
{
    return n1 + n2;
}

Number Formula::difference(Number n1, Number n2)
{
    return n1 - n2;
}

Number Formula::multiplication(Number n1, Number n2)
{
    return n1 * n2;
}

Number Formula::division(Number n1, Number n2)
{
    return n1 / n2;
}

Number Formula::power(Number n1, Number n2)
{
    return n1 ^ n2;
}

bool Formula::isValid() const
{
    return is_valid;
}