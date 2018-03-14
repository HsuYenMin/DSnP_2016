/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>

using namespace std;

// TODO: (Optionally) Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.


template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap(size_t b=0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashMap<HashKey, HashData>;

   public:
      iterator(){}
      iterator(size_t a, size_t b, size_t nb, vector<HashNode>* bs):
      _indexA(a), _indexB(b), _numBuckets(nb), _buckets(bs){}
      iterator(const iterator& it):_indexA(it._indexA), _indexB(it._indexB),
                        _numBuckets(it._numBuckets), _buckets(it._buckets){}
      ~iterator(){}

      const HashNode& operator *() const {return _buckets[_indexA][_indexB];}
            HashNode& operator *() {return _buckets[_indexA][_indexB];}

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
    vector<HashNode>*  _buckets;

 };

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

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

   // check if k is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const HashKey& k) const
                            {  size_t bn = bucketNum(k);
                               for (size_t i = 0; i < _buckets[bn].size(); ++i)
                               {
                                  if(_buckets[bn][i].first == k)
                                     return true;
                               }
                               return false;
                            }
   // query if k is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(const HashKey& k, HashData& d) const
                            {  size_t bn = bucketNum(k);
                               for (size_t i = 0; i < _buckets[bn].size(); ++i)
                               {
                                  if(_buckets[bn][i].first == k)
                                  {
                                     d = _buckets[bn][i].second;
                                     return true;
                                  }
                               }
                               return false; }

   // update the entry in hash that is equal to k (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const HashKey& k, HashData& d)
                            {  size_t bn = bucketNum(k);
                               for (size_t i = 0; i < _buckets[bn].size(); ++i)
                               {
                                  if(_buckets[bn][i].first == k)
                                  {
                                     _buckets[bn][i].second = d;
                                     return true;
                                  }
                               }
                               insert(k,d);
                               return false; }

   // return true if inserted d successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> will not insert
   bool insert(const HashKey& k, const HashData& d)
                            {  size_t bn = bucketNum(k);
                               if(check(k))
                                  return false;
                               else
                               {
                                  _buckets[bn].push_back(HashNode(k,d));
                                  return true;
                               }
                            }

   // return true if removed successfully (i.e. k is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const HashKey& k)
                              {  size_t bn = bucketNum(k);
                                 for (size_t i = 0; i < _buckets[bn].size(); ++i)
                                 {
                                    if(_buckets[bn][i].first == k)
                                    {
                                       _buckets[bn].erase( _buckets[bn].begin() + i);
                                       return true;
                                    }
                                 }     return false;}
private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }
};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//
//    size_t operator() () const { return 0; }
//
//    bool operator == (const CacheKey&) const { return true; }
//
// private:
// };
//
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
   void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const {
      size_t i = k() % _size;
      if (k == _cache[i].first) {
         d = _cache[i].second;
         return true;
      }
      return false;
   }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) {
      size_t i = k() % _size;
      _cache[i].first = k;
      _cache[i].second = d;
   }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H
