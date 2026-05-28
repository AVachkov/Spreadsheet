#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include <iostream>
#include <string>

/** @enum CellType
 *  @brief Възможни типове клетки в електронната таблица.
 */
enum class CellType
{
    NONE,
    ERROR,
    WHOLE_NUMBER,
    DECIMAL_NUMBER,
    TEXT,
    FORMULA
};

/** @struct Address
 *  @brief Структура за координати на клетка (ред и колона, започващи от 1).
 */
struct Address
{
    size_t row; ///< Номер на ред.
    size_t col; ///< Номер на колона.

    Address();
    Address(size_t _row, size_t _col);
    Address &operator=(const Address &rhs);

    /** @brief Проверява дали низът съответства на формат за адрес (напр. R1C1).
     * @param s Низ за проверка.
     * @param out Обект, в който се записват координатите при успех (резултат на функцията).
     * @return true, ако низът е валиден адрес, false иначе.
     */
    static bool isAddress(const std::string &s, Address &out);
};

/** @enum NumberType
 * @brief Възможни типове на числената стойност.
 */
enum class NumberType
{
    NONE,
    WHOLE_NUMBER,
    DECIMAL_NUMBER
};

/// @class Number
/// @brief Обединява и управлява цели и дробни числа в един общ обект.
class Number
{
  public:
    int wholeNumber;      ///< Стойност за цяло число.
    double decimalNumber; ///< Стойност за дробно число.

    Number();
    Number(int _wholeNumber);
    Number(double _decimalNumber);

    /**
     * @brief Метод за извличане на числената стойност като дробно число.
     * @return Стойността като double.
     */
    double getValue() const;

    /**
     * @brief Метод за достъп до типа на числото.
     * @return Стойност от изброимия тип NumberType.
     */
    NumberType getType() const;

    /**
     * @brief Оператор за събиране на две числа.
     * @param rhs Десен операнд.
     * @return Резултатът от събирането.
     */
    Number operator+(Number rhs);

    /**
     * @brief Оператор за изваждане на две числа.
     * @param rhs Десен операнд.
     * @return Резултатът от изваждането.
     */
    Number operator-(Number rhs);

    /**
     * @brief Оператор за умножение на две числа.
     * @param rhs Десен операнд.
     * @return Резултатът от умножението.
     */
    Number operator*(Number rhs);

    /**
     * @brief Оператор за деление на две числа.
     * @param rhs Делител.
     * @details Проверка за деление на 0.
     * @return Резултатът от делението.
     */
    Number operator/(Number rhs);

    /**
     * @brief Оператор за степенуване на число.
     * @param rhs Степенен показател.
     * @return Резултатът от степенуването.
     */
    Number operator^(Number rhs);

    /**
     * @brief Преобразува низ в обект от тип Number.
     * @param str Низ за анализиране.
     * @param number Обект, в който се записва резултатът при успех (резултат на функцията).
     * @return true, ако низът е успешно разпознат като число, false иначе.
     */
    static bool parseNumber(const std::string &str, Number &number);

  private:
    NumberType type; ///< Вътрешен тип на текущото число.
};

/**
 * @brief Оператор за извеждане на числената стойност в изходен поток.
 * @param out Изходен поток.
 * @param n Число за извеждане.
 * @return Референция към изходния поток.
 */
std::ostream &operator<<(std::ostream &out, const Number &n);

#endif