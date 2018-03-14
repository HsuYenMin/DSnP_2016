#include <iostream>
#include <iomanip>
#include <climits>
#include <string>
#include "p2Table.h"

using namespace std;

int main()
{
   Table table;

   // TODO: read in the csv file
   string csvFile;
   string command;
   cout << "Please enter the file name: ";
   cin >> csvFile;
   if (table.read(csvFile))
      cout << "File \"" << csvFile << "\" was read in successfully." << endl;
   else exit(-1); // csvFile does not exist.

   // TODO read and execute commands
   while (true) {
      cin >> command;
      if(command == "PRINT")
         table.print();
      else if(command == "ADD")
         table.add();
      else if(command == "SUM")
      {
         int colIdx = 0;
         cin >> colIdx;
         if(colIdx < 0 || colIdx >= table[0].size())
            cout << "Error: Illegal column index " << '('<< colIdx << ")!!" << endl;
         else
            cout << "The summation of data in column #" << colIdx << " is " << table.sum(colIdx) << endl;
      }
      else if(command == "AVE")
      {
         int colIdx = 0;
         cin >> colIdx;
         if(colIdx < 0 || colIdx >= table[0].size())
            cout << "Error: Illegal column index " << '('<< colIdx << ")!!" << endl;
         else
            cout << "The average of data in column #" << colIdx << " is " << fixed 
                 << setprecision(1) << table.ave(colIdx) << endl;
      }
      else if(command == "MAX")
      {
         int colIdx = 0;
         cin >> colIdx;
         if(colIdx < 0 || colIdx >= table[0].size())
            cout << "Error: Illegal column index " << '('<< colIdx << ")!!" << endl;
         else if(table.max(colIdx) != INT_MAX)
            cout << "The maximum of data in column #" << colIdx << " is " << table.max(colIdx) << endl;
         else 
            cout << "Error: This is a NULL column!!\n"; 
      }
      else if(command == "MIN")
      {
         int colIdx = 0;
         cin >> colIdx;
         if(colIdx < 0 || colIdx >= table[0].size())
            cout << "Error: Illegal column index " << '('<< colIdx << ")!!" << endl;
         else if(table.min(colIdx) != INT_MAX)
            cout << "The minimum of data in column #" << colIdx << " is " << table.min(colIdx) << endl; 
         else 
            cout << "Error: This is a NULL column!!\n";
      }
      else if(command == "COUNT")
      {
         int colIdx = 0;
         cin >> colIdx;
         if(colIdx < 0 || colIdx >= table[0].size())
            cout << "Error: Illegal column index " << '('<< colIdx << ")!!" << endl;
         else if(table.count(colIdx) != 0)
            cout << "The distinct count of data in column #" << colIdx << " is " << table.count(colIdx) << endl;  
         else
            cout << "Error: This is a NULL column!!\n";
      }
      else if(command == "EXIT")
         break;
      else
         cout << "Error: unknown command: \"" << command << '\"' << endl;
   }
}
