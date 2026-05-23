#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include "cell.h"
#include "formula.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

class Spreadsheet
{
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

    void serialize(std::ostream &out) const;
    void deserialize(std::istream &in);

    void parseCell(std::string &token, std::vector<Cell> &row, size_t rowNumber);
    static void processBackslashInText(std::string &token);

    static void extractFormulaTokens(std::string &formula, std::vector<std::string> &tokensOut, std::vector<char> &operatorsOut);
    static void clearUnnecessaryWhitespaces(std::string &s);
    static bool isOperator(char c);
    bool isAddressValid(Address a) const;
};

#endif