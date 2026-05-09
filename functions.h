#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>

class Functions
{
public:
    static void trim(std::string &);
    static bool isLetterOrSymbol(char);
    static int stringToInt(const std::string &);
    static double stringToDouble(const std::string &);

private:
    Functions(); // no one can create an instance
    Functions(const Functions &);
};

#endif