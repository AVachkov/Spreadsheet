#include "spreadsheet.h"
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

    if (deserialize(file))
    {
        std::cout << "Successfully opened " << fileName << '\n';
        is_open = true;
    }
    else
        throw "Couldn't deserialize file.\n";
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
}

void Spreadsheet::saveas(const std::string &_fileName) const
{
    if (!is_open)
        return;

    if (_fileName.empty())
        throw "File name cannot be empty.\n";

    std::ofstream ofs(_fileName);
    serialize(ofs);
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

bool Spreadsheet::serialize(std::ostream &out) const
{
    if (!is_open)
        return false;

    for (const std::vector<Cell> &row : cells)
    {
        bool isFirst = true;
        for (const Cell &cell : row)
        {
            if (isFirst)
                isFirst = false;
            else
                out << ',';
            out << cell;
        }
        out << '\n';
    }

    return true;
}

bool Spreadsheet::deserialize(std::istream &in)
{
    if (!is_open)
        return false;

    std::vector<Cell> row;
    std::string data;

    char ch;
    while (in.get(ch))
    {
        if (ch == ',')
        {
            processCell(data, row);
            data.clear();
        }
        else if (ch == '\n')
        {
            processCell(data, row);
            cells.push_back(row);
            data.clear();
            row.clear();
        }
        else
        {
            data += ch;
        }
    }

    // if the file didn't end with \n
    if (!data.empty())
    {
        processCell(data, row);
        cells.push_back(row);
    }

    int longestCell = cells[0].size();
    for (const std::vector<Cell> &r : cells)
    {
        if (r.size() > longestCell)
            longestCell = r.size();
    }

    for (std::vector<Cell> &r : cells)
    {
        while (r.size() < longestCell)
        {
            Cell empty;
            r.push_back(empty);
        }
    }

    return true;
}

void Spreadsheet::processCell(std::string &data, std::vector<Cell> &row)
{
    trim(data);
    Number number;
    if (data.empty())
    {
        Cell cell;
        row.push_back(cell);
    }
    else if (data[0] == '=')
    {
        // formula
    }
    else if (isNumber(data, number))
    {
        if (number == Number::WHOLE_NUMBER)
        {
            int wholeNumber = std::stoi(data); // stringToInt(data);
            Cell cell(wholeNumber);
            row.push_back(cell);
        }
        else if (number == Number::DECIMAL_NUMBER)
        {
            double decimalNumber = std::stod(data); // stringToDouble(data);
            Cell cell(decimalNumber);
            row.push_back(cell);
        }
    }
    else
    {
        Cell cell(data);
        row.push_back(cell);
    }
}

bool Spreadsheet::isNumber(const std::string &str, Number &number)
{
    number = Number::NONE;
    if (str.empty())
        return false;

    size_t start = 0;
    if (str[0] == '-')
        start = 1;

    int dots = 0;
    for (size_t i = start; i < str.size(); ++i)
    {
        if (!isDigit(str[i]) && str[i] != '.')
            return false;

        if (str[i] == '.')
            ++dots;

        if (dots > 1)
            return false;
    }

    if (str != ".")
    {
        if (dots == 0)
            number = Number::WHOLE_NUMBER;
        else
            number = Number::DECIMAL_NUMBER;

        return true;
    }

    return false;
}

bool Spreadsheet::isDigit(char c)
{
    return c >= '0' && c <= '9';
}

void Spreadsheet::trim(std::string &str)
{
    if (str.empty())
        return;

    int i = 0;
    while (i < str.size() && !isLetterOrSymbol(str[i]))
    {
        if (str[i] == ' ' || str[i] == '\n' || str[i] == '\t')
            str.erase(i, 1);
        else
            ++i;
    }

    i = str.size() - 1;
    while (i >= 0 && !isLetterOrSymbol(str[i]))
    {
        if (str[i] == ' ' || str[i] == '\n' || str[i] == '\t')
            str.erase(i, 1);

        --i;
    }
}

bool Spreadsheet::isLetterOrSymbol(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '!' && c <= '@') || (c >= '[' && c <= '`') || (c >= '{' && c <= '~');
}

// int Spreadsheet::stringToInt(const std::string &s)
// {
//     bool negative = false;
//     int result = 0;
//     for (char c : s)
//     {
//         if (c == '-')
//         {
//             negative = true;
//         }
//         else
//         {
//             result *= 10;
//             result += (c - '0');
//         }
//     }

//     if (negative)
//         result = -result;

//     return result;
// }

// double Spreadsheet::stringToDouble(const std::string &s)
// {
// }