#include <iostream>
#include "spreadsheet.h"

int main()
{
    Spreadsheet table;
    table.open("table.txt");

    table.close();

    table.save();

    return 0;
}