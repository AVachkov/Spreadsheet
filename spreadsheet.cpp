#include "spreadsheet.h"
#include <string>
#include <fstream>
#include <iostream>
#include "utilities.h"

Spreadsheet::Spreadsheet() : is_open(false)
{
}

void Spreadsheet::open(const std::string &_fileName)
{
    if (is_open)
    {
        std::cout << "File " << fileName << "already opened. To open another file, first close the current one.\n";
        return;
    }

    if (_fileName.empty())
        throw "File name cannot be empty.\n";

    fileName = _fileName;
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        file.open(fileName, std::ios::out);
        return;
    }

    is_open = true;
    deserialize(file);
    std::cout << "Successfully opened " << fileName << '\n';
}

void Spreadsheet::close()
{
    for (std::vector<Cell> &row : cells)
        row.clear();
    cells.clear();

    std::cout << "Successfully closed " << fileName << '\n';
    fileName = "";
    is_open = false;
}

void Spreadsheet::save() const
{
    if (!is_open)
        return;

    std::ofstream ofs(fileName);
    ofs.clear();
    serialize(ofs);
    std::cout << "Successfully saved " << fileName << '\n';
}

void Spreadsheet::saveas(const std::string &_fileName) const
{
    if (!is_open)
        return;

    if (_fileName.empty())
        throw "File name cannot be empty.\n";

    std::ofstream ofs(_fileName);
    serialize(ofs);
    std::cout << "Successfully saved as " << _fileName << '\n';
}

void Spreadsheet::help() const
{
    std::cout << "The following commands are supported:\n";
    std::cout << "open <file>     opens <file>\n";
    std::cout << "close           closes currently open file\n";
    std::cout << "save            saves the currently open file\n";
    std::cout << "saveas <file>   saves the currently open file in <file>\n";
    std::cout << "help            prints the information\n";
    std::cout << "exit            exits the program\n";
}

void Spreadsheet::exit() const
{
    std::exit(0);
    std::cout << "Exiting the program...\n";
}

void Spreadsheet::serialize(std::ostream &out) const
{
    if (!is_open)
        return;

    for (const std::vector<Cell> &row : cells)
    {
        bool isFirst = true;
        for (const Cell &cell : row)
        {
            if (isFirst)
                isFirst = false;
            else
                out << ", ";
            out << cell << std::flush;
        }
        out << '\n';
    }
}

void Spreadsheet::deserialize(std::istream &in)
{
    if (!is_open)
        return;

    std::vector<Cell> row;
    std::string token;
    size_t rowNumber = 1;

    char ch;
    while (in.get(ch))
    {
        if (ch == ',')
        {
            processCell(token, row, rowNumber);
            token.clear();
        }
        else if (ch == '\n')
        {
            processCell(token, row, rowNumber);
            cells.push_back(row);
            token.clear();
            row.clear();
            ++rowNumber;
        }
        else
        {
            token += ch;
        }
    }

    // if the file didn't end with \n
    if (!token.empty())
    {
        processCell(token, row, rowNumber);
        cells.push_back(row);
    }

    int longestCell = cells[0].size();
    for (const std::vector<Cell> &r : cells)
    {
        if (r.size() > longestCell)
            longestCell = r.size();
    }

    for (size_t i = 0; i < cells.size(); ++i)
    {
        while (cells[i].size() < longestCell)
        {
            Cell empty(Address(i + 1, cells[i].size() + 1));
            cells[i].push_back(empty);
        }
    }
}

void Spreadsheet::processCell(std::string &token, std::vector<Cell> &row, size_t rowNumber)
{
    trim(token);
    Number number;
    if (token.empty())
    {
        Cell cell(Address(rowNumber, row.size() + 1));
        row.push_back(cell);
    }
    else if (token[0] == '=')
    {
        Formula formula(token, &cells);
        if (formula.isValid())
        {
            Cell cell(Address(rowNumber, row.size() + 1), formula.getResult());
            row.push_back(cell);
        }
    }
    else if (Number::isNumber(token, number))
    {
        if (number.getType() == NumberType::WHOLE_NUMBER)
        {
            Cell cell(Address(rowNumber, row.size() + 1), number.wholeNumber);
            row.push_back(cell);
        }
        else if (number.getType() == NumberType::DECIMAL_NUMBER)
        {
            Cell cell(Address(rowNumber, row.size() + 1), number.decimalNumber);
            row.push_back(cell);
        }
    }
    else
    {
        processBackslashInText(token);
        Cell cell(Address(rowNumber, row.size() + 1), token);
        row.push_back(cell);
    }
}

void Spreadsheet::processBackslashInText(std::string &token)
{
    if (token.empty())
        return;

    bool backslash = false;
    for (int i = 0; i < token.size(); ++i)
    {
        if ((backslash && token[i] == '\"') || (backslash && token[i] == '\\'))
        {
            token.erase(i - 1, 1);
            --i;
        }
        backslash = false;

        if (token[i] == '\\')
        {
            backslash = true;
        }
    }
}