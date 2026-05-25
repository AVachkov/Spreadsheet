#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include "cell.h"
#include <iostream>
#include <string>
#include <vector>

class Spreadsheet
{
  public:
    Spreadsheet();
    Spreadsheet(const Spreadsheet &original);
    Spreadsheet &operator=(const Spreadsheet &rhs);
    ~Spreadsheet();

    void open(const std::string &_fileName);
    void close();
    void save() const;
    void saveas(const std::string &_fileName) const;
    void help() const;
    void exit() const;
    void print(std::ostream &out) const;

  private:
    std::vector<std::vector<Cell *>> cells;
    std::string file_name;
    bool is_open;

    void free();
    void copyfrom(const Spreadsheet &original);

    void serialize(std::ostream &out) const;
    void deserialize(std::istream &in);

    void parseCell(std::string &token, std::vector<Cell *> &row, size_t rowNumber);
    void solveFormulasWithAdresses();
    Number getOrCalculateCellValue(Address a);

    static void tokenize(std::string &formula, std::vector<std::string> &tokensOut, std::vector<char> &operatorsOut);
    void alignAllCells();
    bool isAddressValid(Address a) const;
    static void clearUnnecessaryWhitespaces(std::string &s);
    static void processBackslashInText(std::string &token);
    static std::string removeQuotesFromText(const std::string &text);
    static bool isText(const std::string &s);
    static void handleUnknownCell(const std::string &s, size_t rowNumber, size_t colNumber);
};

#endif