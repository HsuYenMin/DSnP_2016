/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
   //methods
       iterator(){}
       iterator(size_t a, size_t b, size_t nb, vector<Data>* bs):_indexA(a),      _indexB(b),
                                                                 _numBuckets(nb), _buckets(bs)
                                                                 {}
       iterator(const iterator& it):_indexA(it._indexA),         _indexB(it._indexB),
                                    _numBuckets(it._numBuckets), _buckets(it._buckets){}
      ~iterator(){}

      const Data& operator *() const {return _buckets[_indexA][_indexB];}
            Data& operator *() {return _buckets[_indexA][_indexB];}

      iterator& operator --(){bool begin = true;
                              size_t i;
                              size_t max_size = (size_t)-1;
                              if(_indexA == max_size && _indexB == max_size)
                              {
                                 if(_numBuckets == 0)
                                    return (*this);
                                 i = _numBuckets - 1;
                                 while(true)
                                 {
                                    if(_buckets[i].size() != 0)
                                    {
                                       begin = false;
                                       break;
                                    }
                                    if(i == 0)
                                       break;
                                    else
                                       --i;
                                 }

                                 if(begin == true)
                                    return (*this);
                                 else
                                 {
                                    _indexA = i;
                                    _indexB = _buckets[i].size() - 1;
                                 }
                              }

                              if(_indexA == 0 && _indexB == 0)
                                 return (*this);
                              else if(_indexB != 0)
                                 --_indexB;
                              else
                              {
                                 for(i = _indexA - 1; i < max_size; --i)
                                 {
                                    if(_buckets[i].size() != 0)
                                    {
                                       begin = false;
                                       break;
                                    }
                                 }

                                 if(begin == true)
                                    return (*this);
                                 else
                                 {
                                    _indexA = i;
                                    _indexB = _buckets[i].size() - 1;
                                 }

                              }
                              return (*this); }

      iterator& operator ++(){bool end = true;
                              size_t i = 0;
                              size_t max_size = (size_t)-1;

                              if(_indexA == max_size && _indexB == max_size)
                                 return (*this);
                              else if(_indexB < (_buckets[_indexA].size() - 1))
                                 ++_indexB;
                              else
                              {  for(i = _indexA + 1; i < _numBuckets; ++i)
                                 {
                                    if(_buckets[i].size() != 0)
                                    {
                                       end = false;
                                       break;
                                    }
                                 }

                                 if(end == true)
                                 {
                                    _indexA = _indexB = max_size;
                                    return (*this);
                                 }
                                 else
                                 {
                                    _indexA = i;
                                    _indexB = 0;
                                 }

                              }
                              return (*this); }
      iterator  operator --(int){ iterator ans(*this);
                                  --(*this);
                                  return ans;}
      iterator  operator ++(int){ iterator ans(*this);
                                  ++(*this);
                                  return ans;}
      iterator& operator = (const iterator& it){_indexA = it._indexA;
                                                _indexB = it._indexB;
                                                _numBuckets = it._numBuckets;
                                                _buckets = it._buckets;}
      bool operator == (const iterator& it) const{if(   it._indexA == _indexA
                                                     && it._indexB == _indexB
                                                     && it._numBuckets == _numBuckets
                                                     && it._buckets == _buckets)
                                                     return true;
                                                  else
                                                     return false; }
      bool operator != (const iterator& it) const{return !(it == (*this)); }
   //data member
   private:
      size_t _indexA, _indexB, _numBuckets;
      vector<Data>*  _buckets;

   };


   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { size_t max_size = (size_t)-1;
                            if(empty())
                            {
                               iterator it(max_size, max_size, _numBuckets, _buckets);
                               return it ;
                            }
                            size_t i = 0;
                            while( _buckets[i].size() == 0 )
                            {
                               ++i;
                            }
                            iterator b(i, 0, _numBuckets, _buckets);
                            return b; }
   // Pass the end
   iterator end() const { size_t max_size = (size_t)-1;
                          return iterator(max_size, max_size, _numBuckets, _buckets); }
   // return true if no valid data
   bool empty() const { for(size_t i = 0; i < _numBuckets; ++i)
                        {
                           if(_buckets[i].size() != 0)
                              return false;
                        }
                        return true; }
   // number of valid data
   size_t size() const { size_t s = 0;
                         for (size_t i = 0; i < _numBuckets; ++i)
                         {
                            s += _buckets[i].size();
                         } return s; }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const { size_t bn = bucketNum(d);
                                     for (size_t i = 0; i < _buckets[bn].size(); ++i)
                                     {
                                        if(_buckets[bn][i] == d)
                                           return true;
                                     } return false; }
   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {  size_t bn = bucketNum(d);
                                for (size_t i = 0; i < _buckets[bn].size(); ++i)
                                {
                                   if(_buckets[bn][i] == d)
                                   {
                                      d = _buckets[bn][i];
                                      return true;
                                   }
                                }
                                return false; }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) { size_t bn = bucketNum(d);
                                for (size_t i = 0; i < _buckets[bn].size(); ++i)
                                {
                                   if(_buckets[bn][i] == d)
                                   {
                                      _buckets[bn][i] = d;
                                      return true;
                                   }
                                }
                                insert(d);
                                return false; }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) { size_t bn = bucketNum(d);
                                if(check(d))
                                   return false;
                                else
                                {
                                   _buckets[bn].push_back(d);
                                   return true;
                                }
                              }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) { size_t bn = bucketNum(d);
                                for (size_t i = 0; i < _buckets[bn].size(); ++i)
                                {
                                   if(_buckets[bn][i] == d)
                                   {
                                      _buckets[bn].erase( _buckets[bn].begin() + i);
                                      return true;
                                   }
                                }     return false; }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
