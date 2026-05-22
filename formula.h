#ifndef FORMULA_H
#define FORMULA_H

#include "baseTypes.h"
#include <vector>
#include <string>
#include <functional>
// #include <list>

class Cell; // because of circular file problem

class Formula
{
public:
    Formula();
    Formula(const std::string &_formula, std::vector<std::vector<Cell>> *_cells);

    Number getResult() const;
    bool isValid() const;

private:
    std::string formula;
    std::vector<std::vector<Cell>> *cells;

    bool is_valid;
    Number result;

    bool solveFormula(const std::string &_formula, std::vector<std::vector<Cell>> *_cells, Number &outRes);

    // solveFormula helpers:
    static void clearUnnecessaryWhitespaces(std::string &s);
    static bool isOperator(char c);
    bool parseAddress(Address address, Number &n) const;
    bool isAddressValid(Address a) const;
    static bool processOperations(std::vector<Number> &numbers_from_expression, std::vector<char> &operators_from_expression);

    using binaryFunction = std::function<Number(Number, Number)>;
    static bool executeSingleOperator(char op,
                                      const binaryFunction &binOp,
                                      std::vector<Number> &numbers_from_expression,
                                      std::vector<char> &operators_from_expression);

    static bool executeMultipleOperators(const std::vector<char> &operators_to_execute,
                                         const std::vector<binaryFunction> &functions_to_execute,
                                         std::vector<Number> &numbers_from_expression,
                                         std::vector<char> &operators_from_expression); // try doing it with list
};

std::ostream &operator<<(std::ostream &out, const Formula &formula);

#endif