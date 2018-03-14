/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
      _isSorted = 1;
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node = _node->_next;
                                 return *(this);}
      iterator operator ++ (int) { iterator it(_node);
                                   _node = _node->_next;
                                   return it;}
      iterator& operator -- () { _node = _node->_prev;
                                 return *(this); }
      iterator operator -- (int) { iterator it(_node);
                                   _node = _node->_prev;
                                   return it; }

      iterator& operator = (const iterator& i) { _node = i._node;
                                                 return *(this); }

      bool operator != (const iterator& i) const { return (_node != i._node); }
      bool operator == (const iterator& i) const { return (_node == i._node); }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head); }
   iterator end() const { return iterator(_head->_prev); }
   bool empty() const { return (_head ==  _head->_prev); }
   size_t size() const { size_t size = 0;
                         iterator it(_head);
                         const iterator dummy = end();
                         while(it != dummy)
                         {
                            ++size;
                            ++it;
                         }
                         return size;
                       }

   void push_back(const T& x) { const iterator dummy = end();
                                const iterator secondLast((dummy._node)->_prev);
                                //which will become the second last after push_back
                                DListNode<T>* newNode = new DListNode<T>(x, secondLast._node, dummy._node);
                                secondLast._node->_next = dummy._node->_prev = newNode;
                                if(_head == dummy._node) _head = newNode;
                              }
   void pop_front() { if(!empty())
                      {
                        const iterator dummy = end();
                        const iterator second(_head->_next);
                        //which is the second before pop_front
                        delete _head;
                        _head = second._node;
                        (dummy._node)->_next = second._node;
                        (second._node)->_prev = dummy._node;}}
   void pop_back() { if(!empty())
                      {
                        bool moveHead = false;
                        const iterator dummy = end();
                        const iterator secondLast((dummy._node)->_prev->_prev);
                        if(size() == 1)
                           moveHead = true;
                        delete ((dummy._node)->_prev);
                        if(moveHead)
                           _head = dummy._node;
                        (dummy._node)->_prev = secondLast._node;
                        (secondLast._node)->_next = dummy._node;
                      }
                    }

   // return false if nothing to erase
   bool erase(iterator pos) { if (empty()) return false;
                              if(pos == end()) return false;
                              if(pos._node == _head){
                                 pop_front();
                                 return true;
                              }
                              const iterator next((pos._node)->_next);
                              const iterator prev((pos._node)->_prev);
                              delete pos._node;
                              (next._node)->_prev = prev._node;
                              (prev._node)->_next = next._node;
                              return true;
                           }
   bool erase(const T& x) {  for(iterator it(_head); it != end(); ++it){
                                 if (*it == x){
                                    erase(it);
                                    return true;
                                 }
                             }
                             return false;
                          }

   void clear() { iterator it(_head);
                  iterator dummy = end();
                  DListNode<T>* data = 0;
                  while(it != dummy){
                     data = it._node;
                     ++it;
                     delete data;
                  }
                  data = 0;
                  _head = dummy._node;
                  (dummy._node)->_prev = (dummy._node)->_next = dummy._node;
                }// delete all nodes except for the dummy node

   void sort() const {   size_t n = size();
                         const iterator dummy = end();
                         if(n > 2)
                         {
                            --n;
                            iterator a[2], b[2];
                            a[1] = begin();
                            for(size_t m = 1; m < n; m *= 2){
                               iterator before(_head);
                               iterator after(_head->_next);
                               for(size_t i = 0; i < n - m; i = i + 2 * m){
                                  a[0] = a[1] = after;
                                  for(size_t j = 0; j < m - 1; ++j)
                                     ++a[1];
                                  b[0] = b[1] = a[1];
                                  ++b[0];
                                  size_t j = 0;
                                  while(j < m && (b[1]._node)->_next != dummy._node){
                                     ++b[1];
                                     ++j;
                                  }
                                  after._node = b[1]._node->_next;
                                  merge(a,b);
                                  append(before,a[0]);
                                  before = a[1];
                                  append(a[1],after);
                               }
                            }
                         }
                         iterator it = begin();
                         iterator jt(it);
                         ++it;
                         while(it != dummy)
                         {
                            if((*jt) < (*it) || (*jt) == (*it))
                               break;
                            else
                               swap(jt,it);
                            ++jt;
                            ++it;
                         }
                      }

private:
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
   void swap (iterator& a, iterator& b) const { T temp;
                                                temp = *a;
                                                *a = *b;
                                                *b = temp; }

   void merge(iterator (&a)[2], iterator (&b)[2]) const { 
                                                   iterator begin, prev;
                                                   if(*b[0] < *a[0])
                                                      prev = b[0]++;
                                                   else
                                                      prev = a[0]++;
                                                   begin = prev;
                                                   while((b[0]._node != (b[1]._node)->_next) 
                                                          && (a[0]._node != (a[1]._node)->_next)){
                                                      if(*b[0] < *a[0]){
                                                         append(prev, b[0]);
                                                         ++b[0];
                                                      }
                                                      else{
                                                         append(prev, a[0]);
                                                         ++a[0];
                                                      }
                                                   }
                                                   if(a[0]._node == (a[1]._node)->_next){
                                                      append(prev,b[0]);
                                                      a[1] = b[1];
                                                   }
                                                   else
                                                      append(prev,a[0]);
                                                   a[0] = begin;
                                                 }
   void append(iterator& prev, iterator& next) const{ (prev._node)->_next = next._node;
                                                      (next._node)->_prev = prev._node;
                                                      prev = next; 
                                                    }
};

#endif // DLIST_H
