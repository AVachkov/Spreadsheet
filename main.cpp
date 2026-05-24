#include "spreadsheet.h"
#include <iostream>

int main() {
    Spreadsheet s;
    s.open("table.txt");
    s.saveas("des.txt");

    return 0;
}