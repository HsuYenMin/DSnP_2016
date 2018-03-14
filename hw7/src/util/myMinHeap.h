/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d){_data.push_back(d);
                              Data temp;
                              int t = _data.size();
                              while (t > 1)
                              {  int p = t / 2;
                                 if ( (_data[p - 1]) < (_data[t - 1]))
                                    break;
                                 temp = _data[t - 1];
                                 _data[t - 1] = _data[p - 1];
                                 _data[p - 1] = temp;
                                 t = p;
                              } }

   void delMin() { delData(0); }

   void delData(size_t i) { if( i < _data.size() )
                            {  ++i;
                               _data[i - 1] = _data[_data.size() - 1];
                               _data.pop_back();
                               Data temp;
                               if(i / 2 > 0 && (_data[i - 1]) < (_data[i / 2 - 1]) )
                               {
                                  while(i / 2 > 0 && (_data[i - 1]) < (_data[i / 2 - 1]) )
                                  {
                                     temp = _data[i - 1];
                                     _data[i - 1] = _data[i / 2 - 1];
                                     _data[i / 2 - 1] = temp;
                                     i /= 2;
                                  }
                               }
                               else
                               {
                                  size_t t = 0;
                                  while(i * 2 + 1 <= _data.size())
                                  {
                                     if(   (_data[i - 1]) < (_data[i * 2 - 1])
                                        && (_data[i - 1]) < (_data[i * 2]) )
                                        break;
                                     t = ( (_data[i * 2 - 1]) < (_data[i * 2]) ?
                                                         i * 2 : i * 2 + 1);
                                     temp = _data[t - 1];
                                     _data[t - 1] = _data[i - 1];
                                     _data[i - 1] = temp;
                                     i = t;
                                  }

                                  if (i * 2 == _data.size())
                                  {
                                     if( (_data[i * 2 - 1]) < (_data[i - 1]) )
                                     {  temp = _data[i * 2 - 1];
                                        _data[i * 2 - 1] = _data[i - 1];
                                        _data[i - 1] = temp;
                                        i *= 2;
                                     }
                                  }
                               }
                            } }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
