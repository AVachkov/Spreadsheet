#ifndef FORMULA_H
#define FORMULA_H

#include "baseTypes.h"
#include <vector>
#include <string>
#include <functional>

class Cell; // because of circular file problem

class Formula
{
public:
    Formula();
    Formula(const std::string &, std::vector<std::vector<Cell>> *);

    bool isValid() const;

private:
    std::string formula;
    std::vector<std::vector<Cell>> *cells;

    bool is_valid;
    Number result;

    bool solveFormula(const std::string &, std::vector<std::vector<Cell>> *);
    bool isAddressValid(Address) const;
    bool processOperations(std::vector<Number> &, std::vector<char> &);

    static bool isOperator(char);

    static void executeSingleOperation(char, const std::function<Number(Number, Number)> &, std::vector<Number> &, std::vector<char> &); // better do it with std::list

    static Number sum(Number, Number);
    static Number difference(Number, Number);
    static Number multiplication(Number, Number);
    static Number division(Number, Number);
    static Number power(Number, Number);
};

#endif