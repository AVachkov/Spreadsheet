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

    void serialize(std::ostream &) const;
    void deserialize(std::istream &);

    void processCell(std::string &, std::vector<Cell> &, size_t);
    static void processBackslashInText(std::string &);
};

#endif