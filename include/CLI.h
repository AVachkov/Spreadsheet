#ifndef CLI_H
#define CLI_H

#include "spreadsheet.h"
#include <iostream>

/**
 * @class CLI
 * @brief Клас, отговарящ за текстовия потребителски интерфейс (Console Line Interface).
 */
class CLI
{
  public:
    /**
     * @brief Конструктор по подразбиране.
     */
    CLI();

    /**
     * @brief Стартира основния цикъл за четене и изпълнение на команди.
     * @param out Изходен поток за съобщения и визуализация (по подразбиране е std::cout).
     * @param in Входен поток за приемане на команди от потребителя (по подразбиране е std::cin).
     */
    void run(std::ostream &out = std::cout, std::istream &in = std::cin);

  private:
    Spreadsheet spreadsheet; ///< Обект за управление на електронната таблица.
};

#endif