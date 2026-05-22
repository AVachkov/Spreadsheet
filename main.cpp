#include <iostream>
#include "spreadsheet.h"

int main()
{
    Spreadsheet s;
    s.open("table.txt");
    s.saveas("des.txt");

    return 0;
}