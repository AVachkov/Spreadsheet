#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include <cell.h>
#include <vector>
#include <string>

class Spreadsheet
{
public:
    Spreadsheet();

    void open(const std::string &);
    void close();
    void save();
    void saveas(const std::string &);
    void help() const;
    void exit() const;

private:
    std::vector<std::vector<Cell>> cells;
};

#endif