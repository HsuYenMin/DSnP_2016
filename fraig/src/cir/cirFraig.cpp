/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
   vector<CirGate*> CG;
   dfsTraversal(CG);
   HashMap<CirHashKey, CirGate*> hashMap(getHashSize(_numOfAIGs));
   for(size_t i = 0; i < CG.size(); ++i)
   {
      string GStr = CG[i]->getTypeStr();
      if(GStr != "PO" && GStr != "CONST" &&
         GStr != "PI" && CG[i]->_faninList.size() == 2){
         CirHashKey key(CG[i]->_faninList[0], CG[i]->_faninList[1]);
         CirGate* g = 0;
         CirGate* f = CG[i];
         if(hashMap.query(key,g)) merge(f, g);
         else hashMap.insert(key,f);
      }
   }
}
void
CirMgr::merge(CirGate* a, CirGate* b) //merge a to b
{
   for(size_t i = 0; i < a->_fanoutList.size(); ++i){
      vector<CirGateV>& inputs = a->_fanoutList[i].gate()
                                 ->_faninList;
      for(size_t j = 0; j < inputs.size(); ){
         if(inputs[j].gate() == a){
            size_t invert = inputs[j].isInvert();
            inputs.erase(inputs.begin() + j);
            inputs.push_back(CirGateV(b,invert));
            b->_fanoutList.push_back
               (CirGateV(a->_fanoutList[i].gate(),invert));
         }
         else
            ++j;
      }
   }
   a->_fanoutList.clear();
   cout << "Strashing: " << b->_ID << " merging " << a->_ID << "...\n";
   erase(a);
}
void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
