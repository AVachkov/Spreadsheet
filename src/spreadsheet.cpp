#include "spreadsheet.h"
#include "utilities.h"
#include <exception>
#include <fstream>

Spreadsheet::Spreadsheet() : is_open(false), file_path("") {}

Spreadsheet::Spreadsheet(const Spreadsheet &original) : is_open(original.is_open), file_path(original.file_path)
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

void Spreadsheet::open(const std::string &_filePath)
{
    if (is_open)
    {
        throw std::invalid_argument("File " + file_path + " already open. To open another file, first close the current one.");
    }

    if (_filePath.length() < 4 ||
        (_filePath.substr(_filePath.length() - 4) != ".txt" && _filePath.substr(_filePath.length() - 4) != ".csv"))
    {
        throw std::invalid_argument("Unsupported file format.");
    }

    file_path = _filePath;
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        std::ofstream createNewFile(file_path);
        is_open = true;
        return;
    }

    try
    {
        is_open = true;
        deserialize(file);
        file.close();
    }
    catch (const std::invalid_argument &e)
    {
        close();
        throw;
    }
    catch (const std::runtime_error &e)
    {
        close();
        throw;
    }
}

void Spreadsheet::close()
{
    if (!is_open)
        throw std::runtime_error("No file is open.");

    free();
}

void Spreadsheet::save() const
{
    if (!is_open)
        throw std::runtime_error("No file is open.");

    std::ofstream ofs(file_path);
    ofs.clear();
    serialize(ofs);
}

void Spreadsheet::saveas(const std::string &_filePath) const
{
    if (!is_open)
        throw std::runtime_error("No file is open.");

    if (_filePath.empty())
        throw std::invalid_argument("File name cannot be empty.");

    std::ofstream ofs(_filePath);
    serialize(ofs);
}

void Spreadsheet::help(std::ostream &out) const
{
    out << "The following commands are supported:\n";
    out << "open <file>                     opens <file>\n";
    out << "close                           closes currently open file\n";
    out << "save                            saves the currently open file\n";
    out << "saveas <file>                   saves the currently open file in <file>\n";
    out << "edit R<row>C<col> <new_value>   updates cell content with <new_value>\n";
    out << "print                           displays the table in a formatted grid\n";
    out << "help                            prints the information\n";
    out << "exit                            exits the program\n";
}

void Spreadsheet::print(std::ostream &out) const
{
    if (!is_open)
        throw std::runtime_error("No file is open.");

    if (cells.empty())
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

void Spreadsheet::edit(Address address, const std::string &newValueStr)
{
    if (!is_open)
        throw std::runtime_error("No file is open.");

    if (!isAddressValid(address))
        throw std::invalid_argument("Address out of spreadsheet bounds.");

    std::string newValCpy = newValueStr;
    Cell *newCell = parseCell(newValCpy, address);

    Cell *oldCell = cells[address.row - 1][address.col - 1];

    cells[address.row - 1][address.col - 1] = newCell;

    try
    {
        solveFormulasWithAdresses();
        delete oldCell;
    }
    catch (const std::runtime_error &e)
    {
        cells[address.row - 1][address.col - 1] = oldCell;
        delete newCell;
        throw;
    }
}

std::string Spreadsheet::getFilePath() const { return file_path; }

void Spreadsheet::free()
{
    file_path.clear();
    is_open = false;

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
    file_path = original.file_path;
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
            out << cell->getRawContent();
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
        bool inQuotes = false;
        Cell *cell = nullptr;

        char ch;
        while (in.get(ch))
        {
            if (escapeChar)
            {
                token += ch;
                escapeChar = false;
            }
            else if (ch == '"')
            {
                inQuotes = !inQuotes;
                token += ch;
            }
            else if (ch == ',' && !inQuotes)
            {
                cell = parseCell(token, Address(rowNumber, row.size() + 1));
                row.push_back(cell);

                token.clear();
            }
            else if (ch == '\n' && !inQuotes)
            {
                cell = parseCell(token, Address(rowNumber, row.size() + 1));
                row.push_back(cell);
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

        if (!token.empty() || !row.empty())
        {
            cell = parseCell(token, Address(rowNumber, row.size() + 1));
            row.push_back(cell);
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

Cell *Spreadsheet::parseCell(const std::string &data, Address a)
{
    std::string dataCpy = data;
    trim(dataCpy);
    Number number;

    if (dataCpy.empty())
    {
        return new EmptyCell(a);
    }
    else if (Number::parseNumber(dataCpy, number))
    {
        if (number.getType() == NumberType::WHOLE_NUMBER)
        {
            return new WholeNumberCell(a, number.wholeNumber);
        }
        else if (number.getType() == NumberType::DECIMAL_NUMBER)
        {
            return new DecimalNumberCell(a, number.decimalNumber);
        }
    }
    else if (!dataCpy.empty() && dataCpy[0] == '=')
    {
        std::vector<std::string> tokens_from_expression;
        std::vector<char> operators_from_expression;
        tokenize(dataCpy, tokens_from_expression, operators_from_expression);

        Formula formula(dataCpy);
        formula.solveFormula(tokens_from_expression, operators_from_expression);

        if (formula.syntaxError())
        {
            return new ErrorCell(a, dataCpy);
        }
        else
        {
            return new FormulaCell(a, formula);
        }
    }
    else
    {
        if (isText(dataCpy))
        {
            std::string text = removeQuotesFromText(dataCpy);
            processBackslashInText(text);

            return new TextCell(a, text);
        }
        else
        {
            handleUnknownCell(dataCpy, a);
        }
    }

    return new ErrorCell(a, dataCpy);
}

void Spreadsheet::tokenize(const std::string &formula, std::vector<std::string> &tokensOut, std::vector<char> &operatorsOut)
{
    std::string formulaCpy = formula;
    clearUnnecessaryWhitespaces(formulaCpy);

    tokensOut.clear();
    operatorsOut.clear();

    size_t i = 1;
    if (formulaCpy.size() >= 2 && isOperator(formulaCpy[1]))
    {
        std::string data;
        data += formulaCpy[1];
        ++i;
        while (i < formulaCpy.size() && !isOperator(formulaCpy[i]))
        {
            data += formulaCpy[i];
            ++i;
        }

        tokensOut.push_back(data);
    }

    while (i < formulaCpy.size())
    {
        if (isOperator(formulaCpy[i]))
        {
            operatorsOut.push_back(formulaCpy[i]);
            ++i;
        }
        else
        {
            std::string data;
            while (i < formulaCpy.size() && !isOperator(formulaCpy[i]))
            {
                data += formulaCpy[i];
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
        return Number(0);

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
            cells[a.row - 1][a.col - 1] = new ErrorCell(a, formulaText);
            throw;
        }

        delete cells[a.row - 1][a.col - 1];

        if (formula.syntaxError())
        {
            cells[a.row - 1][a.col - 1] = new ErrorCell(a, formulaText);
            return Number(0);
        }
        else
        {
            cells[a.row - 1][a.col - 1] = new FormulaCell(a, formula);
            return formula.getResult();
        }
    }

    return Number(cell->getNumericValue());
}

void Spreadsheet::processBackslashInText(std::string &token)
{
    if (token.empty())
        return;

    bool backslash = false;
    for (int i = 0; i < token.size(); ++i)
    {
        if (backslash && (token[i] == '"' || token[i] == '\\' || token[i] == ','))
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

bool Spreadsheet::isText(const std::string &s) { return s.size() >= 2 && s[0] == '"' && s[s.size() - 1] == '"'; }

void Spreadsheet::handleUnknownCell(const std::string &s, Address a)
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
                throw std::runtime_error("Missing comma detected on row: " + std::to_string(a.row) + " after \"" + arg + "\"");
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
            throw std::runtime_error("Missing comma detected on row: " + std::to_string(a.row) + " after \"" + arg + "\"");
        }
    }

    throw std::runtime_error("row " + std::to_string(a.row) + ", col " + std::to_string(a.col) + ", " + s +
                             " is unknown data type");
}

void Spreadsheet::clearUnnecessaryWhitespaces(std::string &s)
{
    size_t i = 0;
    while (i < s.size())
    {
        if (isdigit(s[i]))
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