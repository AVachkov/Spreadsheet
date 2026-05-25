#include "spreadsheet.h"
#include <iostream>

int main()
{
    Spreadsheet s;
    s.open("tests/table.txt");
    s.saveas("tests/output.txt");

    s.print(std::cout);

    return 0;
}