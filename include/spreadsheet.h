#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include "cell.h"
#include <iostream>
#include <string>
#include <vector>

/**
 * @class Spreadsheet
 * @brief Главен клас, реализиращ логиката на електронната таблица.
 * Класът управлява двумерен вектор от хетерогенни клетки,
 * поддържа текущия отворен файл и предоставя интерфейс за всички основни
 * команди на приложението.
 */
class Spreadsheet
{
  public:
    Spreadsheet();
    Spreadsheet(const Spreadsheet &original);
    Spreadsheet &operator=(const Spreadsheet &rhs);
    ~Spreadsheet();

    /**
     * @brief Отваря и зарежда файл в паметта.
     * @param _filePath Път към файла (.txt или .csv).
     * @throw std::invalid_argument При невалиден формат
     * или вече отворен файл.
     */
    void open(const std::string &_filePath);

    /**
     * @brief Затваря текущия файл и изчиства паметта.
     * @throw std::runtime_error При опит за затваряне без отворен файл.
     */
    void close();

    /**
     * @brief Записва промените обратно в текушия отворен файл.
     * @throw std::runtime_error При опит за запис без отворен файл.
     */
    void save() const;

    /**
     * @brief Запазва данните в нов файл.
     * @param _filePath Път до новия файл
     * @throw std::runtime_error При опит за запис без отворен файл.
     * @throw std::invalid_argument при празен път към файл.
     */
    void saveas(const std::string &_filePath) const;

    /**
     * @brief Извежда списък с поддържаните команди.
     * @param out Изходен поток за принтиране (по подразбиране е std::cout).
     */
    void help(std::ostream &out = std::cout) const;

    /**
     * @brief Отпечатва таблицата подравнена в конзолата.
     * @param out Изходен поток за принтиране (по подразбиране е std::cout).
     * @throw std::runtime_error При опит за принтиране без отворен файл.
     */
    void print(std::ostream &out = std::cout) const;

    /**
     * @brief Редактира съдържанието на дадена клетка.
     * @param address Координати на клетката (ред, колона).
     * @param newValueStr Ново съдържание като низ.
     * @throw std::runtime_error При опит за редакция без отворен файл.
     * @throw std::invalid_argument При адрес извън границите на заредената таблица.
     */
    void edit(Address address, const std::string &newValueStr);

    /**
     * @brief Гетър за file_path
     * @return Пътят към текущия отворен файл.
     */
    std::string getFilePath() const;

  private:
    std::vector<std::vector<Cell *>> cells; ///< Матрица от указатели към хетерогенни клетки.
    std::string file_path;                  ///< Път към на текущия отворен файл.
    bool is_open;                           ///< Флаг за статус на файла.

    /**
     * @brief Освобождава динамично заделената памет за клетките.
     */
    void free();

    /**
     * @brief Помощна функция за голямата 4-ка
     * @param original Обект за копиране.
     */
    void copyfrom(const Spreadsheet &original);

    /**
     * @brief Записва данните от таблицата в изходен поток.
     * @param out Изходен поток за запис.
     */
    void serialize(std::ostream &out) const;

    /**
     * @brief Зарежда данните за таблицата от входен поток.
     * @param in Входен поток за четене.
     */
    void deserialize(std::istream &in);

    /**
     * @brief Анализира низ и връща подходящ тип клетка.
     * @param token Необработената клетка като текст.
     * @param a Адрес на клетката.
     * @return Указател към новосъздадената клетка.
     */
    Cell *parseCell(const std::string &token, Address a);

    /**
     * @brief Преизчислява всички формули, които съдържат препратки
     * към други клетки.
     */
    void solveFormulasWithAdresses();

    /**
     * @brief Връща или преизчислява рекурсивно стойността на дадена клетка.
     * @param a Адрес на клетката.
     * @return Числената стойност на клетката.
     * @throw std::runtime_error При откриване на циклично извикване
     * на клетки или формула, която извиква себе си.
     */
    Number getOrCalculateCellValue(Address a);

    /**
     * @brief Разделя формула на компоненти (елементи) и оператори.
     * @param formula Низът на формулата.
     * @param tokensOut Вектор за извлечените компоненти (резултат от функцията).
     * @param operatorsOut Вектор за извлечените оператори (резултат от функцията).
     */
    static void tokenize(const std::string &formula, std::vector<std::string> &tokensOut, std::vector<char> &operatorsOut);

    /**
     * @brief Подравнява двумерния вектор с клетки, ако във файла липсват.
     */
    void alignAllCells();

    /**
     * @brief Проверява дали адресът е валиден за текущата таблица.
     * @param a Адрес за проверка.
     * @return true, ако адресът е в границите на матрицата, false иначе.
     */
    bool isAddressValid(Address a) const;

    /**
     * @brief Изчиства ненужните интервали от израз.
     * Оставя интервалите между числа.
     * @param s Низ за модификация.
     */
    static void clearUnnecessaryWhitespaces(std::string &s);

    /**
     * @brief Трие обратно наклонените черти в текста
     * където следва специален символ.
     * @param token Текст за обработка.
     */
    static void processBackslashInText(std::string &token);

    /**
     * @brief Премахва кавичките в началото и края на текст.
     * @param text Текст за справка.
     * @return Текстът без кавички.
     */
    static std::string removeQuotesFromText(const std::string &text);

    /**
     * @brief Проверява дали низът е заграден в кавички.
     * @param s Низ за проверка.
     * @return true, ако низът е заграден в кавички, false иначе.
     */
    static bool isText(const std::string &s);

    /**
     * @brief Обработва грешка при неизвестен тип данни в клетка.
     * @param s Невалидният низ.
     * @param a Адрес на клетката (за съобщението).
     * @throw std::runtime_error Винаги хвърля изключение с описание на грешния тип.
     */
    static void handleUnknownCell(const std::string &s, Address a);
};

#endif