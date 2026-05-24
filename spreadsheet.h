#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include "cell.h"
#include <iostream>
#include <string>
#include <vector>

class Spreadsheet {
  public:
    Spreadsheet();

    void open(const std::string &_fileName);
    void close();
    void save() const;
    void saveas(const std::string &_fileName) const;
    void help() const;
    void exit() const;

  private:
    std::vector<std::vector<Cell>> cells;
    std::string fileName;
    bool is_open;

    std::vector<Address> calculationPath;

    void serialize(std::ostream &out) const;
    void deserialize(std::istream &in);

    Number getOrCalculateCellValue(Address a);
    void parseCell(std::string &token, std::vector<Cell> &row, size_t rowNumber);

    static void processBackslashInText(std::string &token);
    static void extractFormulaTokens(std::string &formula, std::vector<std::string> &tokensOut,
                                     std::vector<char> &operatorsOut);
    static void clearUnnecessaryWhitespaces(std::string &s);
    bool isAddressValid(Address a) const;
    void alignAllCells();
    void solveFormulasWithAdresses();
};

#endif