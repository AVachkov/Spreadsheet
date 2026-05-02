#ifndef CELL_H
#define CELL_H

#include <string>
#include <formula.h>
class Cell
{
private:
    int wholeNumber;
    double decimalNumber;
    std::string text;
    Formula formula;
};

#endif