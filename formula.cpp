#include "formula.h"
#include "utilities.h"
#include <iostream>
#include <string>
#include <vector>

Formula::Formula(const std::string &formulaAsText)
    : formula_as_text(formulaAsText), result(), is_valid(false), syntax_error(false)
{
}

void Formula::solveFormula(const std::vector<std::string> &tokens_from_expression, std::vector<char> &operators_from_expression,
                           std::function<Number(Address)> getCellValue)
{
    is_valid = false;

    std::vector<Number> numbers;
    numbers.reserve(tokens_from_expression.size());

    for (const std::string &token : tokens_from_expression)
    {
        Address a;
        Number n;

        if (Address::isAddress(token, a))
        {
            if (getCellValue != nullptr)
            {
                Number cellValue = getCellValue(a);
                if (cellValue.getType() == NumberType::NONE)
                {
                    markSyntaxError();
                    return;
                }
                numbers.push_back(cellValue);
            }
            else
            {
                return;
            }
        }
        else
        {
            if (!Number::parseNumber(token, n))
            {
                markSyntaxError();
                return;
            }
            numbers.push_back(n);
        }
    }

    if (processOperations(numbers, operators_from_expression))
    {
        result = numbers[0];
        is_valid = true;
    }
    else
    {
        markSyntaxError();
    }
}

bool Formula::processOperations(std::vector<Number> &numbers_from_expression, std::vector<char> &operators_from_expression)
{
    if (numbers_from_expression.size() == 1 && operators_from_expression.size() == 0)
        return true;

    if (numbers_from_expression.size() - operators_from_expression.size() != 1)
        return false;

    return executeSingleOperator(
               '^', [](Number n1, Number n2) { return n1 ^ n2; }, numbers_from_expression, operators_from_expression) &&
           executeMultipleOperators({'*', '/'},
                                    {[](Number n1, Number n2) { return n1 * n2; }, [](Number n1, Number n2) { return n1 / n2; }},
                                    numbers_from_expression, operators_from_expression) &&
           executeSingleOperator(
               '-', [](Number n1, Number n2) { return n1 - n2; }, numbers_from_expression, operators_from_expression) &&
           executeSingleOperator(
               '+', [](Number n1, Number n2) { return n1 + n2; }, numbers_from_expression, operators_from_expression);
}

bool Formula::executeSingleOperator(char op, const binaryFunction &binOp, std::vector<Number> &numbers_from_expression,
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

bool Formula::executeMultipleOperators(const std::vector<char> &operators_to_execute,
                                       const std::vector<binaryFunction> &functions_to_execute,
                                       std::vector<Number> &numbers_from_expression, std::vector<char> &operators_from_expression)
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

Number Formula::getResult() const { return result; }

std::string Formula::getFormulaText() const { return formula_as_text; }

bool Formula::isValid() const { return is_valid; }

bool Formula::syntaxError() const { return syntax_error; }

Formula &Formula::operator=(const std::string &_formulaAsText)
{
    formula_as_text = _formulaAsText;
    return *this;
}

void Formula::markSyntaxError()
{
    formula_as_text.clear();
    is_valid = false;
    result = Number();
    syntax_error = true;
}

std::ostream &operator<<(std::ostream &out, const Formula &formula)
{
    out << formula.getResult();
    return out;
}