#ifndef FORMULA_H
#define FORMULA_H

#include "baseTypes.h"
#include <functional>
#include <string>
#include <vector>

/**
 * @class Formula
 * @brief Представя математически израз и управлява неговото пресмятане.
 */
class Formula
{
  public:
    Formula(const Formula &original) = default;
    Formula &operator=(const Formula &rhs) = default;
    ~Formula() = default;

    /**
     * @brief Конструктор за създаване на формула по подаден текст.
     * @param formulaAsText Текстът на формулата.
     */
    Formula(const std::string &formulaAsText);

    /**
     * @brief Изчислява крайната стойност на формулата въз основа на нейните елементи.
     * @param tokens_from_expression Вектор с компоненти (елементи) на израза.
     * @param operators_from_expression Вектор с математически оператори.
     * @param getCellValue Функция за извличане на числената стойност на друга клетка
     * от таблицата по адрес (по подразбиране е nullptr).
     */
    void solveFormula(const std::vector<std::string> &tokens_from_expression, std::vector<char> &operators_from_expression,
                      std::function<Number(Address)> getCellValue = nullptr);

    /**
     * @brief Метод за достъп до крайния резултат от изчислената формула.
     * @return Резултатът като обект от тип Number.
     */
    Number getResult() const;

    /**
     * @brief Метод за достъп до оригиналния текст на формулата.
     * @return Текстът на формулата като низ.
     */
    std::string getFormulaText() const;

    /**
     * @brief Метод за достъп до флага is_valid.
     * @return true, ако формулата е валидна, false иначе.
     */
    bool isValid() const;

    /**
     * @brief Метод за достъп до флага syntax_error.
     * @return true, ако има синтактична грешка, false иначе.
     */
    bool syntaxError() const;

  private:
    std::string formula_as_text; ///< Оригинален текст на формулата.
    bool is_valid;               ///< Флаг за валидност на резултата.
    Number result;               ///< Краен резултат от изчислението.
    bool syntax_error;           ///< Флаг за синтактична грешка.

    /**
     * @brief Отбелязва формулата като синтактично некоректна.
     */
    void markSyntaxError();

    /**
     * @brief Изпълнява в ред на приоритета на математическите операции над числата в израза.
     * @param numbers_from_expression Вектор с числата в израза.
     * @param operators_from_expression Вектор с математическите оператори в израза.
     * @return true при успешно изпълнение, false иначе.
     */
    static bool processOperations(std::vector<Number> &numbers_from_expression, std::vector<char> &operators_from_expression);

    /**
     * @brief Изпълнява един конкретен оператор върху съответните числа.
     * @param op Символ на търсения оператор.
     * @param binOp Двуместна функция, имплементираща операцията.
     * @param numbers_from_expression Вектор с числата в израза.
     * @param operators_from_expression Вектор с операторите в израза.
     * @return true при успешно изпълнение, false иначе.
     */
    using binaryFunction = std::function<Number(Number, Number)>;
    static bool executeSingleOperator(char op, const binaryFunction &binOp, std::vector<Number> &numbers_from_expression,
                                      std::vector<char> &operators_from_expression);

    /**
     * @brief Изпълнява група от оператори, които са с еднакъв приоритет, но
     * трябва да се изпълнят в реда, в който са изписани.
     * @param operators_to_execute Списък от оператори за изпълнение.
     * @param functions_to_execute Съответните функции за изпълнение на операторите.
     * @param numbers_from_expression Вектор с числата в израза.
     * @param operators_from_expression Вектор с операторите в израза.
     * @return true при успешно изпълнение, false иначе.
     */
    static bool executeMultipleOperators(const std::vector<char> &operators_to_execute,
                                         const std::vector<binaryFunction> &functions_to_execute,
                                         std::vector<Number> &numbers_from_expression,
                                         std::vector<char> &operators_from_expression);
};

#endif