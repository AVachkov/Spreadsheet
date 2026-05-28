#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <vector>

/**
 * @brief Премахва празните символи (интервали, табулации и т.н.) от началото и края на низ.
 * @param str Низ за модификация (резултат на функцията).
 */
void trim(std::string &str);

/**
 * @brief Проверява дали даден символ е празен (интервал, табулация, нов ред и т.н.).
 * @param c Символ за проверка.
 * @return true, ако символът е празен, false иначе.
 */
bool isWhitespace(char c);

/**
 * @brief Проверява дали даден символ е поддържан математически оператор.
 * @param c Символ за проверка.
 * @return true, ако символът е оператор, false иначе.
 */
bool isOperator(char c);

/**
 * @brief Преобразува всички главни букви в низа в малки.
 * @param s Низ за модификация (резултат на функцията).
 */
void toLowercase(std::string &s);

/**
 * @brief Разделя низ на масив от компоненти въз основа на празните символи в него.
 * @param s Текст за разделяне.
 * @return Вектор от извлечените компоненти.
 */
std::vector<std::string> split(const std::string &s);

#endif