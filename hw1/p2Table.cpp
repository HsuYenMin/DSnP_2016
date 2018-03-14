#include <iomanip>
#include <fstream>
#include "p2Table.h"

using namespace std;

int stringToInt(string item)
{
   int intTemp = 0; 
   size_t state = 0;
   for(size_t i = 0; i < item.length(); ++i){
      if(item[i] <= '9' && item[i] >= '0'){
         if(state == 0){
            state = 1;
            intTemp = item[i] - '0';
         }
         else if(state == 2){
            state = 3;
            intTemp = item[i] - '0';
         }
         else if(state == 1 || state == 3){
            intTemp *= 10;
            intTemp += item[i] - '0';
         }
      }
      else if(item[i] == '-' && state == 0)
         state = 2;
   }  
   if(state == 2)
      return INT_MAX;
   else if (state == 1)
      return intTemp;
   else 
      return (0 - intTemp);
}
// Implement member functions of class Row and Table here

bool
Table::read(const string& csvFile)
{//TODO
   ifstream ifs(csvFile.c_str());
   if(!ifs)
      return false;
   string s;
   char delim = 13;
   size_t nCol = 0;
   getline(ifs, s, delim);
   if(s != "")
   {
      ++nCol;
      for(size_t i = 0; i < s.length(); ++i)
      {
         if(s[i] == ',')
            ++nCol;
      }
   }
   Row rowTemp(nCol);
   // The first row is annoying. Why not reopen the
   // file after knowing nCol?
   size_t state = 0;
   size_t idx = 0;
   int intTemp = 0;
   char charTemp;
   s += ',';
   for(size_t i = 0; i < s.length(); ++i){
      if(s[i] <= '9' && s[i] >= '0'){
         if(state == 0){
            state = 1;
            intTemp = s[i] - '0';
         }
         else if(state == 1 || state == 2){
            intTemp *= 10;
            intTemp += s[i] - '0';
         }
      }
      else if(s[i] == '-' && state == 0)
         state = 2;
      else if(s[i] == ','){
         if(state == 0)
            rowTemp[idx++] = INT_MAX;
         else if (state == 1)
            rowTemp[idx++] = intTemp;
         else
            rowTemp[idx++] = 0 - intTemp;
         state = 0;
         intTemp = 0;
      }
   }
   _rows.push_back(rowTemp);
   idx = 0;
   while(ifs.get(charTemp)){
      if(charTemp <= '9' && charTemp >= '0'){
         if(state == 0 || state == 3){
            state = 1;
            intTemp = charTemp - '0';
         }
         else if(state == 1 || state == 2){
            intTemp *= 10;
            intTemp += charTemp - '0';
         }
      }
      else if(charTemp == '-' && (state == 0 || state == 3))
         state = 2;
      else if(charTemp == ','){
         if(state == 0 || state == 3)
            rowTemp[idx++] = INT_MAX;
         else if (state == 1)
            rowTemp[idx++] = intTemp;
         else
            rowTemp[idx++] = 0 - intTemp;
         state = 0;
         intTemp = 0;
      }
      else if(charTemp == 13){
         if(state == 3)
            break;
         if(state == 0)
            rowTemp[idx++] = INT_MAX;
         else if (state == 1)
            rowTemp[idx++] = intTemp;
         else if (state == 2)
            rowTemp[idx++] = 0 - intTemp;
         state = 3;
         intTemp = 0;
         idx = 0;
         _rows.push_back(rowTemp);
      }
   }
   return true;
}
void
Table::print()
{
   size_t nRow = _rows.size();
   size_t nCol = _rows[0].size();
      
   for(size_t i = 0; i < nRow; i++){
   for(size_t j = 0; j < nCol; j++){
      if(_rows[i][j] != INT_MAX)
         cout << setw(4) << right << _rows[i][j];
      else
         cout << setw(4) << right << " ";
      }
   cout << endl;
   }
}
int
Table::sum(size_t colIdx)
{
   int sum = 0;
   size_t nRow = _rows.size();
   for (size_t i = 0; i < nRow; i++)
   {
      if(_rows[i][colIdx] != INT_MAX)
         sum += _rows[i][colIdx]; 
   }
   return sum;
}
bool
Table::extremeIdx(size_t colIdx, int multi, size_t &eIdx){

   size_t idx = 0;
   bool hadFound = 0;
   size_t nRow = _rows.size();
   //find the first data include value
   for(size_t i = 0; i < nRow; i++){
      if(_rows[i][colIdx] != INT_MAX){
         idx = i ;
         hadFound = 1;
         break;
      }
   }
   //find the extreme value
   if(!hadFound)
      return false;
   for(size_t i = idx; i < nRow - 1; i++){
      if(_rows[i+1][colIdx] != INT_MAX){
         if(multi * _rows[idx][colIdx] < multi * _rows[i+1][colIdx]){
            idx = i + 1;
         }  
      }
   }
   eIdx = idx;
   return true;
}
int
Table::max(size_t colIdx)
{
   size_t idx = 0;
   if(extremeIdx(colIdx, 1, idx))
      return _rows[idx][colIdx];
   else
      return INT_MAX;
}
int
Table::min(size_t colIdx)
{
   size_t idx = 0;
   if(extremeIdx(colIdx, -1, idx))
      return _rows[idx][colIdx];
   else
      return INT_MAX;
}
size_t
Table::count(size_t colIdx)
{
   bool distinctCount[200];
   size_t sum = 0;
   size_t nRow = _rows.size();
   for(size_t i = 0; i < 200; i++)
   {
      distinctCount[i] = 0;
   }
   for(size_t i = 0; i < nRow; i++)
   {
      if(_rows[i][colIdx] != INT_MAX)
         distinctCount[_rows[i][colIdx] + 99] = 1;
   }
   for(size_t i = 0; i < 200; i++)
   {
      sum += distinctCount[i];
   }
   return sum;
}
float
Table::ave(size_t colIdx)
{
   float sum = 0;
   float ave = 0;
   size_t count = 0;
   size_t nRow = _rows.size();
   for (size_t i = 0; i < nRow; i++)
   {
      if(_rows[i][colIdx] != INT_MAX)
      {
         sum += _rows[i][colIdx];
         count++;
      }
   }
   ave = sum / (float)count;
   return ave;
}
void
Table::add()
{
   size_t nCol = _rows[0].size();
   _rows.push_back(Row(nCol));
   string addTemp; 
   for(size_t i = 0; i < nCol ; i++)
   {
      cin >> addTemp;
      _rows[_rows.size() - 1][i] = stringToInt(addTemp);
   }
}
