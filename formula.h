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
    static void clearAllWhitespaces(std::string &s);
    static bool isOperator(char c);
    bool parseAddress(Address address, Number &n) const;
    bool isAddressValid(Address a) const;
    static bool processOperations(std::vector<Number> &numbers, std::vector<char> &operators);
    static void executeSingleOperation(char op, const std::function<Number(Number, Number)> &binOp, std::vector<Number> &numbers, std::vector<char> &operators);
};

std::ostream &operator<<(std::ostream &out, const Formula &formula);

#endif