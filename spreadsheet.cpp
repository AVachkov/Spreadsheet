#include "spreadsheet.h"
#include "utilities.h"
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

Spreadsheet::Spreadsheet() : is_open(false), file_name("") {}

Spreadsheet::Spreadsheet(const Spreadsheet &original) : is_open(original.is_open), file_name(original.file_name)
{
    copyfrom(original);
}

Spreadsheet &Spreadsheet::operator=(const Spreadsheet &rhs)
{
    if (this != &rhs)
    {
        free();
        copyfrom(rhs);
    }
    return *this;
}

Spreadsheet::~Spreadsheet() { free(); }

void Spreadsheet::open(const std::string &_fileName)
{
    if (is_open)
    {
        std::cout << "File " << file_name << "already opened. To open another file, first close the current one.\n";
        return;
    }

    if (_fileName.empty())
        throw std::invalid_argument("File name cannot be empty.");

    file_name = _fileName;
    std::ifstream file(file_name);
    if (!file.is_open())
    {
        std::ofstream createNewFile(file_name);
        return;
    }

    try
    {
        is_open = true;
        deserialize(file);
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "Table couldn't be loaded. Reason: " << e.what() << '\n';
        close();
        return;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        close();
        return;
    }

    std::cout << "Successfully opened " << file_name << '\n';
}

void Spreadsheet::close()
{
    free();

    std::cout << "Successfully closed " << file_name << '\n';
    file_name.clear();
    is_open = false;
}

void Spreadsheet::save() const
{
    if (!is_open)
        return;

    std::ofstream ofs(file_name);
    ofs.clear();
    serialize(ofs);
    std::cout << "Successfully saved " << file_name << '\n';
}

void Spreadsheet::saveas(const std::string &_fileName) const
{
    if (!is_open)
        return;

    if (_fileName.empty())
        throw std::invalid_argument("File name cannot be empty.");

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
    std::cout << "Exiting the program...\n";
    std::exit(0);
}

void Spreadsheet::print(std::ostream &out) const
{
    if (!is_open || cells.empty())
        return;

    std::vector<size_t> longestTextInEachCol;
    longestTextInEachCol.reserve(cells.size());

    for (size_t c = 0; c < cells[0].size(); ++c)
    {
        size_t maxLen = 0;
        for (size_t r = 0; r < cells.size(); ++r)
        {
            size_t currLen = cells[r][c]->getContentLength();

            if (currLen > maxLen)
            {
                maxLen = currLen;
            }
        }

        longestTextInEachCol.push_back(maxLen);
    }

    for (size_t r = 0; r < cells.size(); ++r)
    {
        for (size_t c = 0; c < cells[r].size(); ++c)
        {
            out << "| ";

            int blankSpaces = longestTextInEachCol[c] - cells[r][c]->getContentLength();

            out << std::string(blankSpaces, ' ');
            cells[r][c]->print(out);
            out << " ";
        }
        out << " |\n";
    }
}

void Spreadsheet::edit(Address address, const Cell *newCell)
{
    if (isAddressValid(address))
    {
        delete cells[address.row - 1][address.col - 1];
        cells[address.row - 1][address.col - 1] = newCell->clone();

        try
        {
            solveFormulasWithAdresses();
        }
        catch (const std::runtime_error &e)
        {
            std::cerr << "Error: " << e.what() << '\n';
        }
    }
    else
    {
        std::cout << "Address out of spreadsheet bonds.\n";
    }
}

void Spreadsheet::free()
{
    for (size_t r = 0; r < cells.size(); ++r)
    {
        for (size_t c = 0; c < cells[r].size(); ++c)
        {
            delete cells[r][c];
            cells[r][c] = nullptr;
        }
        cells[r].clear();
    }
    cells.clear();
}

void Spreadsheet::copyfrom(const Spreadsheet &original)
{
    file_name = original.file_name;
    is_open = original.is_open;
    cells.reserve(original.cells.size());
    for (size_t r = 0; r < original.cells.size(); ++r)
    {
        std::vector<Cell *> newRow;
        newRow.reserve(original.cells[r].size());
        for (size_t c = 0; c < original.cells[r].size(); ++c)
        {
            newRow.push_back(original.cells[r][c]->clone());
        }
        cells.push_back(newRow);
    }
}

void Spreadsheet::serialize(std::ostream &out) const
{
    if (!is_open)
        return;

    for (const std::vector<Cell *> &row : cells)
    {
        bool isFirst = true;
        for (const Cell *cell : row)
        {
            if (isFirst)
                isFirst = false;
            else
                out << ", ";
            cell->print(out);
        }
        out << '\n';
    }
}

void Spreadsheet::deserialize(std::istream &in)
{
    if (!is_open)
        return;

    std::vector<Cell *> row;
    std::string token;
    size_t rowNumber = 1;

    try
    {
        bool escapeChar = false;

        char ch;
        while (in.get(ch))
        {
            if (escapeChar)
            {
                token += ch;
                escapeChar = false;
            }
            else if (ch == ',')
            {
                parseCell(token, row, rowNumber);
                token.clear();
            }
            else if (ch == '\n')
            {
                parseCell(token, row, rowNumber);
                cells.push_back(row);
                token.clear();
                row.clear();
                ++rowNumber;
            }
            else if (ch == '\r')
            {
                continue;
            }
            else if (ch == '\\')
            {
                token += ch;
                escapeChar = true;
            }
            else
            {
                token += ch;
            }
        }

        // if the file didn't end with \n
        if (!token.empty())
        {
            parseCell(token, row, rowNumber);
            cells.push_back(row);
        }
    }
    catch (...)
    {
        for (Cell *cell : row)
        {
            delete cell;
        }
        row.clear();

        throw;
    }

    alignAllCells();

    solveFormulasWithAdresses();
}

void Spreadsheet::parseCell(std::string &data, std::vector<Cell *> &row, size_t rowNumber)
{
    trim(data);
    Number number;

    if (data.empty())
    {
        row.push_back(new EmptyCell(Address(rowNumber, row.size() + 1)));
    }
    else if (Number::parseNumber(data, number))
    {
        if (number.getType() == NumberType::WHOLE_NUMBER)
        {
            row.push_back(new WholeNumberCell(Address(rowNumber, row.size() + 1), number.wholeNumber));
        }
        else if (number.getType() == NumberType::DECIMAL_NUMBER)
        {
            row.push_back(new DecimalNumberCell(Address(rowNumber, row.size() + 1), number.decimalNumber));
        }
    }
    else if (!data.empty() && data[0] == '=')
    {
        std::vector<std::string> tokens_from_expression;
        std::vector<char> operators_from_expression;
        tokenize(data, tokens_from_expression, operators_from_expression);

        Formula formula(data);
        formula.solveFormula(tokens_from_expression, operators_from_expression);

        if (formula.syntaxError())
        {
            row.push_back(new ErrorCell(Address(rowNumber, row.size() + 1)));
        }
        else if ((!data.empty() && data[0] == '=') || formula.isValid())
        {
            row.push_back(new FormulaCell(Address(rowNumber, row.size() + 1), formula));
        }
        else
        {
            row.push_back(new ErrorCell(Address(rowNumber, row.size() + 1)));
        }
    }
    else
    {
        if (isText(data))
        {
            std::string text = removeQuotesFromText(data);
            processBackslashInText(text);
            row.push_back(new TextCell(Address(rowNumber, row.size() + 1), text));
        }
        else
        {
            handleUnknownCell(data, rowNumber, row.size() + 1);
        }
    }
}

void Spreadsheet::tokenize(std::string &formula, std::vector<std::string> &tokensOut, std::vector<char> &operatorsOut)
{
    clearUnnecessaryWhitespaces(formula);

    tokensOut.clear();
    operatorsOut.clear();

    size_t i = 1;
    if (formula.size() >= 2 && isOperator(formula[1]))
    // handle leading operator for the first number (e.g. -5, +12)
    {
        std::string data;
        data += formula[1];
        ++i;
        while (i < formula.size() && !isOperator(formula[i]))
        {
            data += formula[i];
            ++i;
        }

        tokensOut.push_back(data);
    }

    while (i < formula.size())
    {
        if (isOperator(formula[i]))
        {
            operatorsOut.push_back(formula[i]);
            ++i;
        }
        else
        {
            std::string data;
            while (i < formula.size() && !isOperator(formula[i]))
            {
                data += formula[i];
                ++i;
            }

            tokensOut.push_back(data);
            data.clear();
        }
    }
}

void Spreadsheet::solveFormulasWithAdresses()
{
    for (int i = 0; i < cells.size(); ++i)
    {
        for (int j = 0; j < cells[i].size(); ++j)
        {
            getOrCalculateCellValue(Address(i + 1, j + 1));
        }
    }
}

Number Spreadsheet::getOrCalculateCellValue(Address a)
{
    if (!isAddressValid(a))
        return Number();

    Cell *cell = cells[a.row - 1][a.col - 1];

    if (cell->getType() == CellType::NONE)
    {
        EmptyCell *ec = dynamic_cast<EmptyCell *>(cell);
        if (ec != nullptr && ec->getStatus() == "CALCULATING")
        {
            throw std::runtime_error("Circular address calling detected at R" + std::to_string(a.row) + "C" +
                                     std::to_string(a.col));
        }
    }

    if (cell->getType() == CellType::WHOLE_NUMBER || cell->getType() == CellType::DECIMAL_NUMBER)
        return Number(cell->getNumericValue());

    if (cell->getType() == CellType::FORMULA)
    {
        FormulaCell *fc = dynamic_cast<FormulaCell *>(cell);
        if (fc == nullptr)
            return Number();

        Formula formula = fc->getFormula();

        if (formula.isValid())
        {
            return Number(formula.getResult());
        }

        std::vector<std::string> tokens;
        std::vector<char> operators;

        std::string formulaText = formula.getFormulaText();
        tokenize(formulaText, tokens, operators);

        delete cells[a.row - 1][a.col - 1];
        cells[a.row - 1][a.col - 1] = new EmptyCell(a, "CALCULATING");

        try
        {
            formula.solveFormula(tokens, operators, [this](Address a) { return this->getOrCalculateCellValue(a); });
        }
        catch (const std::runtime_error &e)
        {
            delete cells[a.row - 1][a.col - 1];
            cells[a.row - 1][a.col - 1] = new ErrorCell(a);
            throw;
        }

        delete cells[a.row - 1][a.col - 1];

        if (formula.syntaxError())
        {
            cells[a.row - 1][a.col - 1] = new ErrorCell(a);
            return Number(0);
        }
        else
        {
            cells[a.row - 1][a.col - 1] = new FormulaCell(a, formula);
            return formula.getResult();
        }
    }

    return Number(0);
}

void Spreadsheet::processBackslashInText(std::string &token)
{
    if (token.empty())
        return;

    bool backslash = false;
    for (int i = 0; i < token.size(); ++i)
    {
        if ((backslash && token[i] == '\"') || (backslash && token[i] == '\\') || (backslash && token[i] == ','))
        {
            token.erase(i - 1, 1);
            --i;

            backslash = false;
            continue;
        }
        backslash = false;

        if (token[i] == '\\')
        {
            backslash = true;
        }
    }
}

std::string Spreadsheet::removeQuotesFromText(const std::string &text)
{
    if (text.size() < 2)
        return std::string();

    std::string result;
    result.reserve(text.size() - 2);
    for (size_t i = 1; i < text.size() - 1; ++i)
    {
        result += text[i];
    }
    return result;
}

bool Spreadsheet::isText(const std::string &s) { return s.size() >= 2 && s[0] == '\"' && s[s.size() - 1] == '\"'; }

void Spreadsheet::handleUnknownCell(const std::string &s, size_t rowNumber, size_t colNumber)
{
    size_t i = 0;
    std::string arg = "";
    while (i < s.size())
    {
        if (!isWhitespace(s[i]))
        {
            arg += s[i];
            ++i;
        }
        else
        {
            Number dummy;
            if (Number::parseNumber(arg, dummy) || (!arg.empty() && arg[0] == '='))
            {
                throw std::runtime_error("missing comma detected on row: " + std::to_string(rowNumber) + " after \"" + arg +
                                         "\"");
            }

            ++i;
            arg.clear();
        }
    }

    if (!arg.empty())
    {
        Number dummy;
        if (Number::parseNumber(arg, dummy) || (!arg.empty() && arg[0] == '='))
        {
            throw std::runtime_error("missing comma detected on row: " + std::to_string(rowNumber) + " after \"" + arg + "\"");
        }
    }

    throw std::runtime_error("row " + std::to_string(rowNumber) + ", col " + std::to_string(colNumber) + ", " + s +
                             " is unknown data type");
}

void Spreadsheet::clearUnnecessaryWhitespaces(std::string &s)
{
    size_t i = 0;
    while (i < s.size())
    {
        if (isdigit(s[i]))
        // skip whitespaces between digits
        {
            bool isThereOperatorInBetween = false;
            int charsBetweenDigits = 0;
            size_t j = i + 1;
            while (j < s.size() && !isdigit(s[j]))
            {
                if (isOperator(s[j]))
                {
                    isThereOperatorInBetween = true;
                }

                ++j;
                ++charsBetweenDigits;
            }

            if (charsBetweenDigits > 0 && !isThereOperatorInBetween)
            {
                i = j;
            }
        }

        if (isWhitespace(s[i]))
            s.erase(i, 1);
        else
            ++i;
    }
}

bool Spreadsheet::isAddressValid(Address a) const
{
    return a.row > 0 && (a.row - 1) < cells.size() && a.col > 0 && (a.col - 1) < cells[a.row - 1].size();
}

void Spreadsheet::alignAllCells()
{
    if (cells.empty())
        return;

    size_t longestRow = cells[0].size();
    for (const std::vector<Cell *> &r : cells)
    {
        if (r.size() > longestRow)
            longestRow = r.size();
    }

    for (size_t i = 0; i < cells.size(); ++i)
    {
        while (cells[i].size() < longestRow)
        {
            cells[i].push_back(new EmptyCell(Address(i + 1, cells[i].size() + 1)));
        }
    }
}