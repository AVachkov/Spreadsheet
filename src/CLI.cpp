#include "CLI.h"
#include "baseTypes.h"
#include "utilities.h"
#include <string>
#include <vector>

CLI::CLI() {}

void CLI::run(std::ostream &out, std::istream &in)
{
    spreadsheet.help(out);

    std::string line;
    out << "\n> ";

    while (std::getline(in, line))
    {
        trim(line);
        std::vector<std::string> tokens = split(line);

        if (tokens.empty())
        {
            out << "\n> ";
            continue;
        }

        std::string cmd = tokens[0];
        toLowercase(cmd);

        if (cmd == "open")
        {
            if (tokens.size() != 2)
            {
                out << "Wrong arguments. Try again.\n";
                out << "\n> ";
                continue;
            }

            const std::string &filePath = tokens[1];
            try
            {
                spreadsheet.open(filePath);
                out << "Successfully opened " << filePath << '\n';
            }
            catch (const std::exception &e)
            {
                out << "Error: " << e.what() << '\n';
                out << "Data load failed. Terminating...\n";
                return;
            }
        }
        else if (cmd == "close")
        {
            try
            {
                std::string filePath = spreadsheet.getFilePath();
                spreadsheet.close();
                out << "Successfully closed " << filePath << '\n';
            }
            catch (const std::exception &e)
            {
                out << "Error: " << e.what() << '\n';
            }
        }
        else if (cmd == "save")
        {
            try
            {
                spreadsheet.save();
                out << "Successfully saved " << spreadsheet.getFilePath() << '\n';
            }
            catch (const std::exception &e)
            {
                out << "Error: " << e.what() << '\n';
            }
        }
        else if (cmd == "saveas")
        {
            if (tokens.size() != 2)
            {
                out << "Wrong arguments. Try again.\n";
                out << "\n> ";
                continue;
            }

            const std::string &filePath = tokens[1];
            try
            {
                spreadsheet.saveas(filePath);
                out << "Successfully saved as " << filePath << '\n';
            }
            catch (const std::exception &e)
            {
                out << "Error: " << e.what() << '\n';
            }
        }
        else if (cmd == "edit")
        {
            if (tokens.size() < 3)
            {
                out << "Wrong arguments. Try again.\n";
                out << "\n> ";
                continue;
            }

            const std::string &addressStr = tokens[1];

            Address a;
            if (!Address::isAddress(tokens[1], a))
            {
                out << addressStr << " is invalid address. Try again.\n";
                out << "\n> ";
                continue;
            }

            std::string newValStr = tokens[2];
            for (size_t i = 3; i < tokens.size(); ++i)
            {
                newValStr += " " + tokens[i];
            }

            if (tokens.size() > 3)
            {
                if (!newValStr.empty() && (newValStr[0] != '"' || newValStr[newValStr.size() - 1] != '"'))
                {
                    if (newValStr[0] != '=')
                    {
                        out << "Wrong arguments. Text with spaces must be in quotes. Try again.\n";
                        out << "\n> ";
                        continue;
                    }
                }
            }

            try
            {
                spreadsheet.edit(a, newValStr);
                out << "Successfully edited cell R" << a.row << "C" << a.col << '\n';
            }
            catch (const std::exception &e)
            {
                out << "Error: " << e.what() << '\n';
            }
        }
        else if (cmd == "print")
        {
            try
            {
                spreadsheet.print(out);
            }
            catch (const std::exception &e)
            {
                out << "Error: " << e.what() << '\n';
            }
        }
        else if (cmd == "help")
        {
            spreadsheet.help(out);
        }
        else if (cmd == "exit")
        {
            out << "Exiting the program...\n";
            return;
        }
        else
        {
            out << "Invalid command \"" << line << "\". Enter \"help\" to see supported commands.\n";
        }

        out << "\n> ";
    }
}