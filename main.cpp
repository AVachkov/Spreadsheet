#include <iostream>
#include "spreadsheet.h"

int main()
{
    Spreadsheet s;
    s.open("table.txt");
    s.saveas("table2.txt");

    return 0;
}