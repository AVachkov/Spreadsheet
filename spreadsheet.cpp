#include "spreadsheet.h"
#include "utilities.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

Spreadsheet::Spreadsheet() : is_open(false) {}

void Spreadsheet::open(const std::string &_fileName) {
    if (is_open) {
        std::cout << "File " << fileName << "already opened. To open another file, first close the current one.\n";
        return;
    }

    if (_fileName.empty())
        throw std::invalid_argument("File name cannot be empty.");

    fileName = _fileName;
    std::ifstream file(fileName);
    if (!file.is_open()) {
        file.open(fileName, std::ios::out);
        return;
    }

    try {
        is_open = true;
        deserialize(file);
    } catch (const std::exception &e) {
        std::cerr << "Table couldn't be loaded. Reason: " << e.what() << '\n';
        close();
        return;
    }

    std::cout << "Successfully opened " << fileName << '\n';
}

void Spreadsheet::close() {
    for (std::vector<Cell> &row : cells)
        row.clear();
    cells.clear();

    std::cout << "Successfully closed " << fileName << '\n';
    fileName = "";
    is_open = false;
}

void Spreadsheet::save() const {
    if (!is_open)
        return;

    std::ofstream ofs(fileName);
    ofs.clear();
    serialize(ofs);
    std::cout << "Successfully saved " << fileName << '\n';
}

void Spreadsheet::saveas(const std::string &_fileName) const {
    if (!is_open)
        return;

    if (_fileName.empty())
        throw std::invalid_argument("File name cannot be empty.");

    std::ofstream ofs(_fileName);
    serialize(ofs);
    std::cout << "Successfully saved as " << _fileName << '\n';
}

void Spreadsheet::help() const {
    std::cout << "The following commands are supported:\n";
    std::cout << "open <file>     opens <file>\n";
    std::cout << "close           closes currently open file\n";
    std::cout << "save            saves the currently open file\n";
    std::cout << "saveas <file>   saves the currently open file in <file>\n";
    std::cout << "help            prints the information\n";
    std::cout << "exit            exits the program\n";
}

void Spreadsheet::exit() const {
    std::cout << "Exiting the program...\n";
    std::exit(0);
}

void Spreadsheet::serialize(std::ostream &out) const {
    if (!is_open)
        return;

    for (const std::vector<Cell> &row : cells) {
        bool isFirst = true;
        for (const Cell &cell : row) {
            if (isFirst)
                isFirst = false;
            else
                out << ", ";
            out << cell;
        }
        out << '\n';
    }
}

void Spreadsheet::deserialize(std::istream &in) {
    if (!is_open)
        return;

    std::vector<Cell> row;
    std::string token;
    size_t rowNumber = 1;

    char ch;
    while (in.get(ch)) {
        if (ch == ',') {
            parseCell(token, row, rowNumber);
            token.clear();
        } else if (ch == '\n') {
            parseCell(token, row, rowNumber);
            cells.push_back(row);
            token.clear();
            row.clear();
            ++rowNumber;
        } else {
            token += ch;
        }
    }

    // if the file didn't end with \n
    if (!token.empty()) {
        parseCell(token, row, rowNumber);
        cells.push_back(row);
    }

    alignAllCells();

    solveFormulasWithAdresses();
}

void Spreadsheet::parseCell(std::string &token, std::vector<Cell> &row, size_t rowNumber) {
    trim(token);
    Number number;
    if (token.empty()) {
        Cell cell(Address(rowNumber, row.size() + 1));
        row.push_back(cell);
    } else if (token.size() > 0 && token[0] == '=') {
        std::vector<std::string> tokens;
        std::vector<char> operators;
        extractFormulaTokens(token, tokens, operators);
        Formula formula(token);
        formula.solveFormula(tokens, operators);

        if (formula.isValid() || (!token.empty() && token[0] == '=')) {
            Cell cell(Address(rowNumber, row.size() + 1), formula);
            row.push_back(cell);
        } else {
            Cell cell(Address(rowNumber, row.size() + 1), "ERROR");
            row.push_back(cell);
        }
    } else if (Number::parseNumber(token, number)) {
        if (number.getType() == NumberType::WHOLE_NUMBER) {
            Cell cell(Address(rowNumber, row.size() + 1), number.wholeNumber);
            row.push_back(cell);
        } else if (number.getType() == NumberType::DECIMAL_NUMBER) {
            Cell cell(Address(rowNumber, row.size() + 1), number.decimalNumber);
            row.push_back(cell);
        }
    } else {
        processBackslashInText(token);
        Cell cell(Address(rowNumber, row.size() + 1), token);
        row.push_back(cell);
    }
}

void Spreadsheet::extractFormulaTokens(std::string &formula, std::vector<std::string> &tokensOut,
                                       std::vector<char> &operatorsOut) {
    clearUnnecessaryWhitespaces(formula);

    tokensOut.clear();
    operatorsOut.clear();

    size_t i = 1;
    if (formula.size() >= 2 && isOperator(formula[1]))
    // handle leading operator for the first number (e.g. -5, +12)
    {
        std::string data;
        while (i < formula.size() && !isOperator(formula[i])) {
            data += formula[i];
            ++i;
        }

        tokensOut.push_back(data);
    }

    while (i < formula.size()) {
        if (isOperator(formula[i])) {
            operatorsOut.push_back(formula[i]);
            ++i;
        } else {
            std::string data;
            while (i < formula.size() && !isOperator(formula[i])) {
                data += formula[i];
                ++i;
            }

            tokensOut.push_back(data);
            data.clear();
        }
    }
}

void Spreadsheet::solveFormulasWithAdresses() {
    for (int i = 0; i < cells.size(); ++i) {
        for (int j = 0; j < cells[i].size(); ++j) {
            getOrCalculateCellValue(Address(i + 1, j + 1));
        }
    }
}

Number Spreadsheet::getOrCalculateCellValue(Address a) {
    if (!isAddressValid(a))
        return Number(0);

    Cell &cell = cells[a.row - 1][a.col - 1];

    if (cell.getType() == CellType::TEXT && cell.getText() == "CALCULATING") {
        throw std::runtime_error("Circular address calling detected at R" + std::to_string(a.row) + "C" +
                                 std::to_string(a.col));
    }

    if (cell.getType() == CellType::WHOLE_NUMBER)
        return Number(cell.getWholeNumber());

    if (cell.getType() == CellType::DECIMAL_NUMBER)
        return Number(cell.getDecimalNumber());

    if (cell.getType() == CellType::FORMULA) {
        Formula formula = cell.getFormula();

        if (formula.isValid())
            return Number(formula.getResult());

        std::vector<std::string> tokens;
        std::vector<char> operators;

        std::string formulaText = formula.getFormulaText();
        extractFormulaTokens(formulaText, tokens, operators);

        cell = Cell(a, "CALCULATING");

        try {
            formula.solveFormula(tokens, operators, [this](Address a) { return this->getOrCalculateCellValue(a); });
        } catch (const std::runtime_error &e) {
            cell = Cell(a, "ERROR");
            throw;
        }

        cell = formula;

        return formula.getResult();
    }

    return Number(0);
}

void Spreadsheet::processBackslashInText(std::string &token) {
    if (token.empty())
        return;

    bool backslash = false;
    for (int i = 0; i < token.size(); ++i) {
        if ((backslash && token[i] == '\"') || (backslash && token[i] == '\\')) {
            token.erase(i - 1, 1);
            --i;
        }
        backslash = false;

        if (token[i] == '\\') {
            backslash = true;
        }
    }
}

void Spreadsheet::clearUnnecessaryWhitespaces(std::string &s) {
    size_t i = 0;
    while (i < s.size()) {
        if (isdigit(s[i]))
        // skip whitespaces between digits
        {
            bool isThereOperatorInBetween = false;
            int charsBetweenDigits = 0;
            size_t j = i + 1;
            while (j < s.size() && !isdigit(s[j])) {
                if (isOperator(s[j])) {
                    isThereOperatorInBetween = true;
                }

                ++j;
                ++charsBetweenDigits;
            }

            if (charsBetweenDigits > 0 && !isThereOperatorInBetween) {
                i = j;
            }
        }

        if (isWhitespace(s[i]))
            s.erase(i, 1);
        else
            ++i;
    }
}

bool Spreadsheet::isAddressValid(Address a) const {
    return a.row > 0 && (a.row - 1) < cells.size() && a.col > 0 && (a.col - 1) < cells[a.row - 1].size();
}

void Spreadsheet::alignAllCells() {
    if (cells.empty())
        return;

    int longestRow = cells[0].size();
    for (const std::vector<Cell> &r : cells) {
        if (r.size() > longestRow)
            longestRow = r.size();
    }

    for (size_t i = 0; i < cells.size(); ++i) {
        while (cells[i].size() < longestRow) {
            Cell empty(Address(i + 1, cells[i].size() + 1));
            cells[i].push_back(empty);
        }
    }
}