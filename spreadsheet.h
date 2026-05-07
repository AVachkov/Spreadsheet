#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include "cell.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

class Spreadsheet
{
public:
    Spreadsheet();

    void open(const std::string &);
    void close();
    void save() const;
    void saveas(const std::string &) const;
    void help() const;
    void exit() const;

private:
    std::vector<std::vector<Cell>> cells;
    std::string fileName;
    bool is_open;

    bool serialize(std::ostream &) const;
    bool deserialize(std::istream &);

    static void processCell(std::string &, std::vector<Cell> &);

    enum Number
    {
        NONE,
        WHOLE_NUMBER,
        DECIMAL_NUMBER
    };

    static bool isNumber(const std::string &, Number &number);
    static bool isDigit(char);
    static void trim(std::string &);
    static bool isLetterOrSymbol(char);
    // static int stringToInt(const std::string &);
    // static double stringToDouble(const std::string &);
};

#endif