#include "spreadsheet.h"
#include "formula.h"
#include "functions.h"
#include <string>
#include <fstream>
#include <iostream>

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
    std::string data;
    size_t rowNumber = 1;

    char ch;
    while (in.get(ch))
    {
        if (ch == ',')
        {
            processCell(data, row, rowNumber);
            data.clear();
        }
        else if (ch == '\n')
        {
            processCell(data, row, rowNumber);
            cells.push_back(row);
            data.clear();
            row.clear();
            ++rowNumber;
        }
        else
        {
            data += ch;
        }
    }

    // if the file didn't end with \n
    if (!data.empty())
    {
        processCell(data, row, rowNumber);
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

void Spreadsheet::processCell(std::string &data, std::vector<Cell> &row, size_t rowNumber)
{
    Functions::trim(data);
    Number number;
    if (data.empty())
    {
        Cell cell(Address(rowNumber, row.size() + 1));
        row.push_back(cell);
    }
    else if (data[0] == '=')
    {
        // formula
        Formula formula(data, &cells);
        if (formula.isValid())
        {
        }
    }
    else if (Number::isNumber(data, number))
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
        processBackslashInText(data);
        Cell cell(Address(rowNumber, row.size() + 1), data);
        row.push_back(cell);
    }
}

void Spreadsheet::processBackslashInText(std::string &data)
{
    if (data.empty())
        return;

    bool backslash = false;
    for (int i = 0; i < data.size(); ++i)
    {
        if ((backslash && data[i] == '\"') || (backslash && data[i] == '\\'))
        {
            data.erase(i - 1, 1);
            --i;
        }
        backslash = false;

        if (data[i] == '\\')
        {
            backslash = true;
        }
    }
}