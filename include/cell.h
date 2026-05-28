#ifndef CELL_H
#define CELL_H

#include "baseTypes.h"
#include "formula.h"
#include <iostream>
#include <string>

/**
 * @class Cell
 * @brief Базов абстрактен клас, представящ клетка от таблицата.
 */
class Cell
{
  public:
    Cell(const Cell &original) = default;
    Cell &operator=(const Cell &rhs) = default;
    virtual ~Cell();

    /**
     * @brief Конструктор за инициализиране на клетка по адрес.
     * @param _address Координати на клетката.
     */
    Cell(Address _address);

    /**
     * @brief Създава динамично копие на текущата клетка от подходящ тип.
     * @return Указател към новото копие.
     */
    virtual Cell *clone() const = 0;

    /**
     * @brief Връща типа на клетката.
     * @return Стойност от изброимия тип CellType.
     */
    virtual CellType getType() const = 0;

    /**
     * @brief Извлича числената стойност на клетката.
     * @return Обект от тип Number с числената стойност.
     */
    virtual Number getNumericValue() const = 0;

    /**
     * @brief Отпечатва стойността на клетката.
     * @param out Изходен поток за отпечатване.
     */
    virtual void print(std::ostream &out) const = 0;

    /**
     * @brief Изчислява дължината на съдържанието на стойността.
     * @return Брой символи на съдържанието на стойността.
     */
    virtual size_t getContentLength() const = 0;

    /**
     * @brief Връща необработеното съдържание на клетката като текст.
     * @return Низ с необработено съдържание.
     */
    virtual std::string getRawContent() const = 0;

  protected:
    Address address; ///< Координати на клетката в таблицата.
};

/**
 * @class WholeNumberCell
 * @brief Клетка, съдържаща цяло число.
 */
class WholeNumberCell : public Cell
{
  public:
    WholeNumberCell(const WholeNumberCell &original) = default;
    WholeNumberCell &operator=(const WholeNumberCell &rhs) = default;

    /**
     * @brief Конструктор за клетка с цяло число.
     * @param _address Координати на клетката.
     * @param _wholeNumber Цялото число за съхранение.
     */
    WholeNumberCell(Address _address, int _wholeNumber);

    Cell *clone() const override;
    CellType getType() const override;
    Number getNumericValue() const override;
    void print(std::ostream &out) const override;
    size_t getContentLength() const override;
    std::string getRawContent() const override;

  private:
    int whole_number; ///< Съхраняваното цяло число.
};

/**
 * @class DecimalNumberCell
 * @brief Клетка, съдържаща дробно число.
 */
class DecimalNumberCell : public Cell
{
  public:
    DecimalNumberCell(const DecimalNumberCell &original) = default;
    DecimalNumberCell &operator=(const DecimalNumberCell &rhs) = default;

    /**
     * @brief Конструктор за клетка с дробно число.
     * @param _address Координати на клетката.
     * @param _decimalNumber Дробното число за съхранение.
     */
    DecimalNumberCell(Address _address, double _decimalNumber);

    Cell *clone() const override;
    CellType getType() const override;
    Number getNumericValue() const override;
    void print(std::ostream &out) const override;
    size_t getContentLength() const override;
    std::string getRawContent() const override;

  private:
    double decimal_number; ///< Съхраняваното дробно число.

    /**
     * @brief Помощна функция за форматиране на числото като низ.
     * @details Нужна заради излишните нули, по-големи числа и
     * научен запис на числа в изходен поток.
     * @return Форматираният низ без излишни нули.
     */
    std::string getFormattedString() const;
};

/**
 * @class TextCell
 * @brief Клетка, съдържаща символен низ (текст).
 */
class TextCell : public Cell
{
  public:
    TextCell(const TextCell &original) = default;
    TextCell &operator=(const TextCell &rhs) = default;

    /**
     * @brief Конструктор за текстова клетка.
     * @param _address Координати на клетката.
     * @param _text Символният низ за съхранение.
     */
    TextCell(Address _address, const std::string &_text);

    Cell *clone() const override;
    CellType getType() const override;
    Number getNumericValue() const override;
    void print(std::ostream &out) const override;
    size_t getContentLength() const override;
    std::string getRawContent() const override;

  private:
    std::string text; ///< Съхраняваният текст.
};

/**
 * @class FormulaCell
 * @brief Клетка, съдържаща математическа формула с препратки.
 */
class FormulaCell : public Cell
{
  public:
    FormulaCell(const FormulaCell &original) = default;
    FormulaCell &operator=(const FormulaCell &rhs) = default;

    /**
     * @brief Конструктор за клетка с формула.
     * @param _address Координати на клетката.
     * @param _formula Обект от тип формула.
     */
    FormulaCell(Address _address, const Formula &_formula);

    Cell *clone() const override;
    CellType getType() const override;
    Number getNumericValue() const override;
    void print(std::ostream &out) const override;
    size_t getContentLength() const override;
    std::string getRawContent() const override;

    /**
     * @brief Метод за достъп до обекта формула.
     * @return Копие на формулата в клетката.
     */
    Formula getFormula() const;

  private:
    Formula formula; ///< Съхраняваната формула.
};

/**
 * @class EmptyCell
 * @brief Представя празна клетка в таблицата.
 */
class EmptyCell : public Cell
{
  public:
    EmptyCell(const EmptyCell &original) = default;
    EmptyCell &operator=(const EmptyCell &rhs) = default;

    /**
     * @brief Конструктор за празна клетка.
     * @param _address Координати на клетката.
     * @param _status Статус на клетката (по подразбиране е празен низ).
     */
    EmptyCell(Address _address, const std::string &_status = "");

    Cell *clone() const override;
    CellType getType() const override;
    Number getNumericValue() const override;
    void print(std::ostream &out) const override;
    size_t getContentLength() const override;
    std::string getRawContent() const override;

    /**
     * @brief Метод за достъп до статуса на клетката.
     * @return Низ със статуса.
     */
    std::string getStatus() const;

  private:
    std::string status; ///< Статус на клетката.
};

/**
 * @class ErrorCell
 * @brief Клетка, представляваща грешно съдържание.
 */
class ErrorCell : public Cell
{
  public:
    ErrorCell(const ErrorCell &original) = default;
    ErrorCell &operator=(const ErrorCell &rhs) = default;

    /**
     * @brief Конструктор за клетка с грешка.
     * @param _address Координати на клетката.
     * @param _originalContent Оригиналният невалиден текст.
     */
    ErrorCell(Address _address, const std::string &_originalContent);

    Cell *clone() const override;
    CellType getType() const override;
    Number getNumericValue() const override;
    void print(std::ostream &out) const override;
    size_t getContentLength() const override;
    std::string getRawContent() const override;

  private:
    std::string original_content; ///< Оригиналният невалиден вход.
};

#endif