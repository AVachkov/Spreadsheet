#include "spreadsheet.h"
#include <string>
#include <fstream>
#include <iostream>

Spreadsheet::Spreadsheet()
{
}

void Spreadsheet::open(const std::string &_fileName)
{
    if (_fileName.empty())
        throw "File name cannot be empty.\n";

    fileName = _fileName;
    file.open(fileName);

    if (!file.is_open())
        file.open(fileName, std::ios::out);

    if (!deserialize())
        throw "Couldn't deserialize file.";

    if (file.is_open())
        std::cout << "Successfully opened " << fileName << '\n';
}

void Spreadsheet::close()
{
    for (std::vector<Cell> &row : cells)
    {
        row.clear();
    }
    cells.clear();

    if (!file.is_open())
    {
        fileName = "";
        return;
    }

    std::cout << "Successfully closed " << fileName << '\n';
    fileName = "";
    file.close();
}

void Spreadsheet::save()
{
    if (!file.is_open())
        return;

    file.clear();
    serialize(file);
}

void Spreadsheet::saveas(const std::string &_fileName)
{
    if (!file.is_open())
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
    if (!file.is_open())
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

bool Spreadsheet::deserialize()
{
    if (!file.is_open())
        return false;

    std::vector<std::string> lines;
    while (!file.eof())
    {
        std::string line;
        std::getline(file, line);

        lines.push_back(line);
    }

    for (const std::string &line : lines)
    {
        std::string part;
        // part.reserve(line.size());
        std::vector<Cell> currentRow;
        for (size_t i = 0; i < line.size(); ++i)
        {
            if (line[i] == ',')
            {
                // extracted part
                // parse part. add to cells

                if (part.empty())
                {
                    Cell cell;
                    currentRow.push_back(cell);
                }

                if (part[0] == '=')
                {
                    // save as formula
                }
                part = "";
            }
            else
            {
                part += line[i];
            }
        }
        cells.push_back(currentRow);
    }

    return true;
}