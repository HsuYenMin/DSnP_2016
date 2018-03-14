#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>
#include <iostream>
using namespace std;

class Row
{
public:
   Row(size_t i):_size(i){
      _data = new int[_size];
   }
   Row(const Row &row):_size(row._size){
      _data = new int[_size];
      for (size_t i = 0; i < _size; ++i)
      {
         _data[i] = row._data[i];
      }
   }
   ~Row(){
      delete[] _data;
   }
   const int operator[] (size_t i) const {return _data[i];} // TODO
   int& operator[] (size_t i) {return _data[i];} // TODO
   const size_t size(){return _size;}   
private:
   int  *_data;
   size_t _size;
};

class Table
{
public:
   const Row& operator[] (size_t i) const{return _rows[i];}
   Row& operator[] (size_t i){return _rows[i];}
   bool   read(const string&);
   void   print();
   int    sum(size_t colIdx);
   bool   extremeIdx(size_t colIdx, int multi, size_t &eIdx);
   float  ave(size_t colIdx);
   int    max(size_t colIdx);
   int    min(size_t colIdx);
   size_t count(size_t colIdx);
   void   add();
   size_t size(){return _rows.size();}
private:
   vector<Row>  _rows;
};

#endif // P2_TABLE_H
