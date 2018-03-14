/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;
   BSTreeNode(const T& d,BSTreeNode<T>* p = 0, BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0):
      _data(d), _parent(p), _left(l), _right(r) {}
   T                _data  ;
   BSTreeNode<T>*   _parent;
   BSTreeNode<T>*   _left  ;
   BSTreeNode<T>*   _right ;
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
public:
   BSTree(){
     _root = _dummy = new BSTreeNode<T>(T());
     _size = 0;
   }
   ~BSTree() { clear(); delete _dummy; }

   class iterator {
      friend class BSTree;

   public:
      iterator(BSTreeNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () {  if((_node->_right) == 0){
                                     iterator it(_node);
                                     while(1){
                                        if( ((it._node)->_parent) == 0){
                                           return *(this);
                                        }
                                        if(it._node ==  (((it._node)->_parent)->_left) )
                                        { it._node = (it._node)->_parent;  break; }
                                        it._node = (it._node)->_parent;
                                     }
                                     _node = it._node;
                                     return *(this);
                                  }
                                  else{
                                     _node = (_node->_right);
                                     while( (_node->_left) != 0 ){
                                        _node = (_node->_left);
                                     }
                                     return *(this);
                                  } }
      iterator operator ++ (int) { iterator ans(*this);
                                   ++(*this);
                                   return ans; }
      iterator& operator -- () { if((_node->_left) == 0){
                                     iterator it(_node);
                                     while(1){
                                        if( ((it._node)->_parent) == 0){
                                           return *(this);
                                        }
                                        if( it._node ==  (((it._node)->_parent)->_right) )
                                        { it._node = (it._node)->_parent;  break; }
                                        it._node = (it._node)->_parent;
                                     }
                                     _node = it._node;
                                     return *(this);
                                 }
                                 else{
                                     _node = (_node->_left);
                                     while( (_node->_right) != 0 ){
                                        _node = (_node->_right);
                                     }
                                     return *(this);
                               } }
      iterator operator -- (int) { iterator ans(*this);
                                   --(*this);
                                   return ans; }

      iterator& operator = (const iterator& i) { _node = i._node;
                                                 return *(this); }

      bool operator != (const iterator& i) const { return ( _node != i._node); }
      bool operator == (const iterator& i) const { return ( _node == i._node);}
   private:
      BSTreeNode<T>* _node;
   };

   iterator begin() const { iterator it(_root);
                            while(((it._node)->_left) != 0){
                              it._node = ((it._node)->_left) ;}
                            return it; }
   iterator end() const { iterator it(_dummy);
                          return it; }
   bool empty() const   { return (_root == _dummy);}
   size_t size() const  { return _size; }

   void insert(const T& x){   iterator it(_root);
                              if(_root == _dummy){
                                 BSTreeNode<T>* a = new BSTreeNode<T>(x,_dummy->_parent,0,_dummy);
                                 _root = a;
                                 _dummy->_parent = a;
                              }
                              else
                                 while(1){
                                   if(it._node == _dummy)
                                   {  (_dummy->_parent)->_right = new BSTreeNode<T>(x,_dummy->_parent,0,_dummy); 
                                      _dummy->_parent = (_dummy->_parent)->_right; break;}
                                   if(x < *it)
                                   {  if((it._node)->_left == 0)
                                      {  (it._node)->_left = new BSTreeNode<T>(x, it._node); break;}
                                      else
                                         it._node = (it._node)->_left;
                                   }
                                   else
                                   {
                                      if((it._node)->_right == 0)
                                      {  (it._node)->_right = new BSTreeNode<T>(x, it._node); break;}
                                      else
                                         it._node = (it._node)->_right;
                                   }
                                 }
                              _size++;
                          }
   void pop_front() { if(! empty() ){
                         iterator beg = begin();
                         if((beg._node)->_right == 0){
                            ((beg._node)->_parent)->_left = 0;
                            delete beg._node; beg._node = 0;
                         }
                         else{
                            if((beg._node)->_parent){
                               ((beg._node)->_right)->_parent = (beg._node)->_parent;
                               ((beg._node)->_parent)->_left = (beg._node)->_right;
                               delete beg._node; beg._node = 0;
                            }
                            else{
                               _root = (beg._node)->_right;
                               ((beg._node)->_right)->_parent = 0;
                               delete beg._node; beg._node = 0;
                            }
                         }
                         _size--;
                      }
                    }
   void pop_back() { if(size() == 1)
                        {pop_front(); _size--;}
                     else if(! empty() ){
                        iterator it(_dummy->_parent);
                        if((it._node)->_left){
                           iterator successor(it);
                           --successor;
                           *it = *successor;
                           BSTreeNode<T>* child = 0;
                           if((successor._node)->_left){
                              child = (successor._node)->_left;
                              child->_parent = (successor._node)->_parent;
                           }
                           // if the successor has child(must be left child), the
                           // successor's parent's corresponding child should be
                           // the successor's child.
                           if(((successor._node)->_parent)->_right == successor._node)
                              ((successor._node)->_parent)->_right = child;
                           else
                              ((successor._node)->_parent)->_left = child;
                           delete successor._node; successor._node = 0;
                        }
                        else{// if the last element do not have child
                           if((it._node)->_parent){//handle dummy node
                              ((it._node)->_right)->_parent = (it._node)->_parent;
                              ((it._node)->_parent)->_right = (it._node)->_right;
                              delete it._node; it._node = 0;
                           }
                           else{
                              _root = (it._node)->_right;
                              ((it._node)->_right)->_parent = 0;
                              delete it._node; it._node = 0;
                           }
                        }
                        _size--;
                     } }

   // return false if nothing to erase
   bool erase(iterator pos) {   if(! empty() ){
                                   BSTreeNode<T>* parent = (pos._node)->_parent;
                                   bool Right = 0; // 1 for right, 0 for left
                                   bool RC = 0, LC = 0;// right child and left child
                                   if((pos._node)->_right)
                                      RC = 1;
                                   if((pos._node)->_left)
                                      LC = 1;
                                   if(parent){
                                      if(pos._node == parent->_right)
                                         Right = 1 ;
                                      if(!RC){
                                        if(!LC){
                                          if(Right)
                                             parent->_right = 0;
                                          else
                                             parent->_left = 0;
                                          delete pos._node; pos._node = 0;
                                        }
                                        else{
                                          if(Right)
                                             parent->_right = (pos._node)->_left;
                                          else
                                             parent->_left = (pos._node)->_left;
                                          (pos._node)->_left->_parent = parent;
                                          delete pos._node; pos._node = 0;
                                        }
                                      }
                                      else{ // RC == true
                                        if(!LC){
                                           if(Right)
                                              parent->_right = (pos._node)->_right;
                                           else
                                              parent->_left = (pos._node)->_right;
                                           (pos._node)->_right->_parent = parent;
                                           delete pos._node; pos._node = 0;
                                        }
                                        else{// RC == 1, LC == 1 _parent!=0
                                           iterator successor(pos);
                                           --successor;
                                           *pos = *successor;
                                           BSTreeNode<T>* child = 0;
                                           if((successor._node)->_left){
                                              child = (successor._node)->_left;
                                              child->_parent = (successor._node)->_parent;
                                           }
                                           // if the successor has child(must be left child), the
                                           // successor's parent's corresponding child should be
                                           // the successor's child.
                                           if(((successor._node)->_parent)->_right == successor._node)
                                              ((successor._node)->_parent)->_right = child;
                                           else
                                              ((successor._node)->_parent)->_left = child;
                                           delete successor._node; successor._node = 0;
                                        }
                                     }
                                  }
                                  else{//no parent
                                     if(!LC){
                                        (pos._node)->_right->_parent = 0;
                                        _root = (pos._node)->_right;
                                        delete pos._node; pos._node = 0;
                                     }
                                     else{// have two children
                                       iterator successor(pos);
                                       --successor;
                                       *pos = *successor;
                                       BSTreeNode<T>* child = 0;
                                       if((successor._node)->_left){
                                          child = (successor._node)->_left;
                                          child->_parent = (successor._node)->_parent;
                                       }
                                       // if the successor has child(must be left child), the
                                       // successor's parent's corresponding child should be
                                       // the successor's child.
                                       if(((successor._node)->_parent)->_right == successor._node)
                                          ((successor._node)->_parent)->_right = child;
                                       else
                                          ((successor._node)->_parent)->_left = child;
                                       delete successor._node; successor._node = 0;
                                     }
                                  }
                                   _size--;
                                   return true;
                                }
                                else
                                   return false; }
   bool erase(const T& x) { iterator it;
                            if( find(x, it) ){
                                erase(it);
                                return true;// bool erase(iterator pos) will do _size--
                            } return false; }

   void clear() { if(! empty() ){
                     clear(_root);
                     _root = _dummy;
                     _dummy->_parent =0;
                     _size = 0;
                  }
                }
   void clear(BSTreeNode<T>* node){
                                     if(node->_right && node->_right != _dummy)
                                        clear(node->_right);
                                     if(node->_left)
                                        clear(node->_left);
                                     delete node; node = 0;
                                  }
   void sort() const { }//no action will be taken
   void print(){ postorder(_root);}
   void postorder(BSTreeNode<T>*p, int indent=0){
                                                   if(p != NULL && p != _dummy) {
                                                      if(p->_left) postorder(p->_left, indent+4);
                                                      if(p->_right) postorder(p->_right, indent+4);
                                                      if (indent) std::cout << std::setw(indent) << ' ';               
                                                      cout<< p->_data << "\n ";
                                                   }
                                                }
private:
   BSTreeNode<T>* _root;
   BSTreeNode<T>* _dummy;
   size_t         _size;
   bool find(const T& x, iterator& it){
                                        it._node = _root;
                                        while(1){
                                           if(it._node == _dummy)
                                           {  return false;}
                                           if(x < *it)
                                           {  if((it._node)->_left == 0)
                                              {  return false;}
                                              else
                                                 it._node = (it._node)->_left;
                                           }
                                           else if(*it < x)
                                           {
                                              if((it._node)->_right == 0)
                                              {  return false;}
                                              else
                                              it._node = (it._node)->_right;
                                           }
                                           else{
                                              return true;
                                           }
                                        }
                                     }
   //void min(iterator it){}
   //bool successor (BSTreeNode<T>* N,iterator it){}//downNext function
};

#endif // BST_H
