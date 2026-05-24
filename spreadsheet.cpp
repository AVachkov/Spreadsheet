#include "spreadsheet.h"
#include "utilities.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

Spreadsheet::Spreadsheet() : is_open(false), file_name("") {}

void Spreadsheet::open(const std::string &_fileName) {
    if (is_open) {
        std::cout << "File " << file_name << "already opened. To open another file, first close the current one.\n";
        return;
    }

    if (_fileName.empty())
        throw std::invalid_argument("File name cannot be empty.");

    file_name = _fileName;
    std::ifstream file(file_name);
    if (!file.is_open()) {
        file.open(file_name, std::ios::out);
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

    std::cout << "Successfully opened " << file_name << '\n';
}

void Spreadsheet::close() {
    for (std::vector<Cell *> &row : cells) {
        for (const Cell *cell : row) {
            delete cell;
        }
        row.clear();
    }

    cells.clear();

    std::cout << "Successfully closed " << file_name << '\n';
    file_name.clear();
    is_open = false;
}

void Spreadsheet::save() const {
    if (!is_open)
        return;

    std::ofstream ofs(file_name);
    ofs.clear();
    serialize(ofs);
    std::cout << "Successfully saved " << file_name << '\n';
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

    for (const std::vector<Cell *> &row : cells) {
        bool isFirst = true;
        for (const Cell *cell : row) {
            if (isFirst)
                isFirst = false;
            else
                out << ", ";
            cell->print(out);
        }
        out << '\n';
    }
}

void Spreadsheet::deserialize(std::istream &in) {
    if (!is_open)
        return;

    std::vector<Cell *> row;
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

void Spreadsheet::parseCell(std::string &data, std::vector<Cell *> &row, size_t rowNumber) {
    trim(data);
    Number number;
    if (data.empty()) {
        row.push_back(new EmptyCell(Address(rowNumber, row.size() + 1)));
    } else if (Number::parseNumber(data, number)) {
        if (number.getType() == NumberType::WHOLE_NUMBER) {
            row.push_back(new WholeNumberCell(Address(rowNumber, row.size() + 1), number.wholeNumber));
        } else if (number.getType() == NumberType::DECIMAL_NUMBER) {
            row.push_back(new DecimalNumberCell(Address(rowNumber, row.size() + 1), number.decimalNumber));
        }
    } else if (!data.empty() && data[0] == '=') {
        std::vector<std::string> tokens_from_expression;
        std::vector<char> operators_from_expression;
        extractFormulaTokens(data, tokens_from_expression, operators_from_expression);

        Formula formula(data);
        formula.solveFormula(tokens_from_expression, operators_from_expression);

        if ((!data.empty() && data[0] == '=') || formula.isValid()) {
            row.push_back(new FormulaCell(Address(rowNumber, row.size() + 1), formula));
        } else {
            row.push_back(new ErrorCell(Address(rowNumber, row.size() + 1)));
        }
    } else {
        processBackslashInText(data);
        row.push_back(new TextCell(Address(rowNumber, row.size() + 1), data));
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

    Cell *cell = cells[a.row - 1][a.col - 1];

    if (cell->getType() == CellType::NONE) {
        EmptyCell *ec = dynamic_cast<EmptyCell *>(cell);
        if (ec != nullptr && ec->getStatus() == "CALCULATING") {
            throw std::runtime_error("Circular address calling detected at R" + std::to_string(a.row) + "C" +
                                     std::to_string(a.col));
        }
    }

    if (cell->getType() == CellType::WHOLE_NUMBER || cell->getType() == CellType::DECIMAL_NUMBER)
        return Number(cell->getNumericValue());

    if (cell->getType() == CellType::FORMULA) {
        FormulaCell *fc = dynamic_cast<FormulaCell *>(cell);
        if (fc == nullptr)
            return Number(0);

        Formula formula = fc->getFormula();

        if (formula.isValid()) {
            return Number(formula.getResult());
        }

        std::vector<std::string> tokens;
        std::vector<char> operators;

        std::string formulaText = formula.getFormulaText();
        extractFormulaTokens(formulaText, tokens, operators);

        delete cells[a.row - 1][a.col - 1];
        cells[a.row - 1][a.col - 1] = new EmptyCell(a, "CALCULATING");

        try {
            formula.solveFormula(tokens, operators, [this](Address a) { return this->getOrCalculateCellValue(a); });
        } catch (const std::runtime_error &e) {
            delete cells[a.row - 1][a.col - 1];
            cells[a.row - 1][a.col - 1] = new ErrorCell(a);
            throw;
        }

        delete cells[a.row - 1][a.col - 1];
        cells[a.row - 1][a.col - 1] = new FormulaCell(a, formula);

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

    size_t longestRow = cells[0].size();
    for (const std::vector<Cell *> &r : cells) {
        if (r.size() > longestRow)
            longestRow = r.size();
    }

    for (size_t i = 0; i < cells.size(); ++i) {
        while (cells[i].size() < longestRow) {
            cells[i].push_back(new EmptyCell(Address(i + 1, cells[i].size() + 1)));
        }
    }
}