/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include "dbTable.h"
#include "../util/util.h"

using namespace std;
/*****************************************/
/*        Self-define Functions          */
/*****************************************/
bool isInt(string s)
{
    if ( s[0] != '-' && (s[0] > '9' || s[0] < '0') )
        return false;

    for (int i = 1 ; s[i] != '\0'; i++)
    {
        if ( s[i] > '9' || s[i] < '0')
            return false;
    }
    
    return true;
}


void stringToElement(string data, string pattern, vector<string>& Elements)
{

   int pos, count = 0;
	data += pattern;
	int size = data.size();
	string temp;
	for(int i = 0; i < size; i++)
    {
		pos=data.find(pattern,i);

    	if(pos<size)
        {
			count++;
			temp = data.substr(i,pos-i);
			if(temp.size() == 0)
         {
			    Elements.push_back("-");
		   }
			else
			    Elements.push_back(temp);
			i = pos + pattern.size() - 1;
        }
	}
}


float findExtremeValue(const vector<int>& v,const bool max = true){
   int idx;
	bool hadFound = 0;
	//find the first data include value
	for(int i = 0; i < v.size() ; i++){
		if( v[i] != INT_MAX){
			idx = i ;
			hadFound = 1;
			break;
		}
	}
   if(! hadFound)
        return NAN;
	//if max == true then find maximum, false, minimum)
   if( max == true ){
		for(size_t i = idx; i < v.size() - 1; i++){
			if(v[i + 1] != INT_MAX){
				if(v[idx] < v[i + 1]){
					idx = i + 1;
            }
			}
		}
   }
   else{
      for(size_t i = idx; i < v.size() - 1; i++){
         if(v[i + 1] != INT_MAX){
            if(v[idx] > v[i + 1]){
               idx = i + 1;
            }
         }
      }
   }
	return v[idx];
}

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   if (!(r.empty()))
   {
       if(r[0] == INT_MAX)
         os << ".";
       else
         os << r[0];
   }

   for (size_t i = 1; i < r.size(); i++)
   { 
      if(r[i] == INT_MAX)
         os << " .";
      else
         os << ' ' << r[i];
   }
   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells should be left blank (printed as ' ').
   size_t nC = t.nCols(), nR = t.nRows();
   for (size_t i = 0; i < nR; i++){
      for(size_t j = 0; j < nC; j++){
         if(t[i][j] != INT_MAX)
            os << setw(6) << right << t[i][j] ;
         else
            os << "     .";
      }
      os << endl;
   }
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume all the data of the table are in a single line.
    vector<string> sArray, vecTemp;
    vector< vector<string> > sTable; 
    vector<int> iArray;
    string s;
    int intTemp;

    getline(ifs,s);

    s += "\r\r";
    s = s.substr(0, s.find("\r\r", 0));
    stringToElement(s, "\r", sArray);
   //each element in sArray is a row
   //sTable is a string version of dbTable
    for (int i = 0; i < sArray.size(); i++)
    {
         vecTemp.clear();
         stringToElement( sArray[i], ",", vecTemp );
         sTable.push_back( vecTemp );
    }
   
    for (int i = 0; i < sArray.size(); i++)
    {
        iArray.clear();
        for(int j = 0; j < sTable[i].size(); j++)
        {
            if ( myStr2Int( sTable[i][j], intTemp ) )
                iArray.push_back( intTemp );    
            else
                iArray.push_back( INT_MAX );
        }
        t.addRow( DBRow(iArray) );
    }
   
   return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
   // TODO
   if ( c < this->size() )
      _data.erase(_data.begin() + c);

}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
   for (int i = 0; i < _sortOrder.size(); i++)
   {
      if ( r1[ _sortOrder[i] ] < r2[ _sortOrder[i] ] )
         return true;
      else if ( r1[ _sortOrder[i] ] > r2[ _sortOrder[i] ] )
         return false;
   }
   return false;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   // TODO
   _table.clear();
}

void
DBTable::addCol(const vector<int>& d)
{
   // TODO: add a column to the right of the table. Data are in 'd'.
   size_t nR = nRows();
   size_t ds = d.size();
   for(int i = 0; i < nR; i++)
   {
      if( i < ds)
         _table[i].addData(d[i]);
      else
         _table[i].addData(INT_MAX);
   }
}

void
DBTable::delRow(int c)
{
   // TODO: delete row #c. Note #0 is the first row.
   if(c < nRows())
      _table.erase(_table.begin() + c);
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
   // TODO: get the max data in column #c
   if(c < nCols()){
      vector<int> column;
      size_t nR = nRows();
      for(size_t i = 0; i < nR; i++){
         column.push_back(_table[i][c]);
      }
      return findExtremeValue(column);
   }
   return 0.0;
}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
   if(c < nCols()){
      vector<int> column;
      size_t nR = nRows();
      for(size_t i = 0; i < nR; i++){
         column.push_back(_table[i][c]);
      }
      return findExtremeValue(column, false);
   }
   return 0.0;
}

float 
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
   if(c < nCols()){
      float sum = 0.0;
      size_t nR = nRows();
      for(size_t i = 0; i < nR; i++){
         if(_table[i][c] != INT_MAX)
            sum += _table[i][c];
      }
      return sum;
   }
   return 0.0;
}

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
   if(c < nCols()){
      size_t nR = nRows();
      size_t ndistinct = 0;
      for(size_t i = 0; i < nR; i++){
         if(_table[i][c] != INT_MAX){
            bool distinct = true;
            for(size_t j = 0; j < i; j++){
               if(_table[i][c] == _table[j][c]){
                  distinct = false;
                  break;
               }
            }
            if(distinct)
               ndistinct++;
         }
      }
      return ndistinct;
   }
   return 0;
}

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
   if(c < nCols()){
      size_t nElem = 0;
      size_t nR = nRows();
      for(size_t i = 0; i < nR; i++){
         if(_table[i][c] != INT_MAX)
            nElem++;
      }
      return (getSum(c) / nElem);
   }
   return 0.0;
}

void
DBTable::sort(const struct DBSort& s)
{
   // TODO: sort the data according to the order of columns in 's'
   std::sort( _table.begin(), _table.end(), s);
}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   if(c < nCols()){
      if(_table[0][c] != INT_MAX)
         cout << _table[0][c];
      else
         cout << '.';
   }
   size_t nR = nRows();
   for(size_t i = 1; i < nR; i++){
      if(_table[i][c] != INT_MAX)
         cout << ' ' << _table[i][c];
      else
         cout << " .";
   }
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

