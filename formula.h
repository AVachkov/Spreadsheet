#ifndef FORMULA_H
#define FORMULA_H

#include "baseTypes.h"
#include <functional>
#include <string>
#include <vector>

class Formula {
  public:
    Formula(const Formula &original) = default;
    Formula &operator=(const Formula &rhs) = default;
    ~Formula() = default;

    Formula(const std::string &formulaAsText);

    void solveFormula(const std::vector<std::string> &tokens_from_expression,
                      std::vector<char> &operators_from_expression,
                      std::function<Number(Address)> getCellValue = nullptr);

    Number getResult() const;
    std::string getFormulaText() const;
    bool isValid() const;

    Formula &operator=(const std::string &_formulaAsText);

  private:
    std::string formula_as_text;
    bool is_valid;
    Number result;

    static bool processOperations(std::vector<Number> &numbers_from_expression,
                                  std::vector<char> &operators_from_expression);

    using binaryFunction = std::function<Number(Number, Number)>;
    static bool executeSingleOperator(char op, const binaryFunction &binOp,
                                      std::vector<Number> &numbers_from_expression,
                                      std::vector<char> &operators_from_expression);

    static bool executeMultipleOperators(const std::vector<char> &operators_to_execute,
                                         const std::vector<binaryFunction> &functions_to_execute,
                                         std::vector<Number> &numbers_from_expression,
                                         std::vector<char> &operators_from_expression);

    void markSyntaxError();
};

std::ostream &operator<<(std::ostream &out, const Formula &formula);

#endif