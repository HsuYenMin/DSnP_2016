/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include <map>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
   vector<const CirGate*> GL;
   dfsTraversal(GL);
   for (size_t i = 0; i < _gateArr.size(); ++i){
      if(_gateArr[i] != 0 && _gateArr[i]->notGlobalRef()){
         switch(_gateArr[i]->_gateType){
            case UNDEF_GATE:
               _gateArr[i]->sweep();
               cout<<"Sweeping: UNDEF(" << _gateArr[i]->_ID << ") removed...\n";
               delete _gateArr[i]; 
               _gateArr[i] = 0;
               break;
            case AIG_GATE:
               _gateArr[i]->sweep();
               cout<<"Sweeping: AIG(" << _gateArr[i]->_ID << ") removed...\n";
               --_numOfAIGs;
               delete _gateArr[i]; 
               _gateArr[i] = 0;
               break;
            default:
               break;//no default really required here
         }
      }
   }
}

void CirGate::sweep()
{
   for(size_t i = 0; i < _faninList.size(); ++i){
      vector<CirGateV>& output = (_faninList[i].gate()
            ->_fanoutList) ;
      for(size_t j = 0; j < output.size(); ){
         if(output[j].gate() == this) output.erase(output.begin() + j);
         else ++j;
      }
   }
   for(size_t i = 0; i < _fanoutList.size(); ++i)
   {
      vector<CirGateV>& input = (_fanoutList[i].gate()
            ->_faninList) ;
      for(size_t j = 0; j < input.size(); ){
         if(input[j].gate() == this) input.erase(input.begin() + j);
         else ++j;
      }
   }
}
// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
   vector<const CirGate*> GL;
   dfsTraversal(GL);
   CirGate* CZ = getGate(0);
   vector<CirGateV>& outputs = CZ->_fanoutList;
   for(size_t i = 0; i < outputs.size(); ++i){
      if(outputs[i].gate()->isGlobalRef() &&
         outputs[i].gate()->getTypeStr() != "PO" ){
         //cerr << "opt from const zero started\n";
         opt_Traversal( outputs[i].gate());
         //cerr << "opt from const zero ended\n";
      }
   }
   for(size_t i = 0; i < _PIs.size(); ++i ){
      // cerr << "opt from PO " << _PIs[i]->getID() << " started\n";
      if(_PIs[i]->isGlobalRef()){
         vector<CirGateV>& outputs = _PIs[i]->_fanoutList;
         for(int j = 0; j < outputs.size(); ++j){
            if(outputs[j].gate()->isGlobalRef() &&
               outputs[j].gate()->getTypeStr() != "PO"){
               opt_Traversal( outputs[j].gate());
            }
         }
      }
   }
}

void
CirMgr::opt_Traversal(CirGate* CGptr)
{
   bool hasCZ = false;
   bool hasC1 = false;
   bool hasIden = false;
   bool hasInve = false;
   unsigned whichConst = 0;

   if( CGptr->getTypeStr() != "PO"){
      if(CGptr->_faninList.size() == 2){
         for(unsigned i = 0; i < CGptr->_faninList.size() ; ++i){
            if(CGptr->_faninList[i].gate()
               ->getTypeStr() == "CONST"){
               whichConst = i;
               if(CGptr->_faninList[i].isInvert())
                  hasC1 = true;
               else
                  hasCZ = true;
            }
         }
         vector<CirGateV>& inputs = CGptr->_faninList;
         if(inputs[0].gate() == inputs[1].gate()){
            if(inputs[0].isInvert() == inputs[1].isInvert())
               hasIden = true;
            else
               hasInve = true;
         }
      }

      if(hasCZ)
         opt_Const_Zero(CGptr);
      else if(hasC1)
         opt_Const_One(CGptr,whichConst);
      else if(hasIden)
         opt_Const_One(CGptr);
      else if(hasInve)
         opt_Const_Zero(CGptr);

      vector<CirGateV>& outputs = CGptr->_fanoutList;
      for(size_t i = 0; i < outputs.size(); ++i){
         if(outputs[i].gate()->isGlobalRef() &&
            outputs[i].gate()->getTypeStr() != "PO" )
            opt_Traversal(outputs[i].gate());
      }
      // If CGptr had been optimized, delete it at the end.
      if(hasCZ || hasC1 || hasIden || hasInve){
         if(CGptr->getTypeStr() == "AIG") --_numOfAIGs;
         _gateArr[CGptr->_ID] = 0;
         delete CGptr;
      }
   }
}

void
CirMgr::opt_Const_One(CirGate* CGptr, unsigned whichConst)
{
   unsigned input4connect = (whichConst == 0) ? 1 : 0;
   CirGate* plug = CGptr->_faninList[input4connect].gate();
   for(size_t i = 0; i < plug->_fanoutList.size(); ++i){
      if(plug->_fanoutList[i].gate() == CGptr)
         plug->_fanoutList.erase(plug->_fanoutList.begin() + i);
   }
   for(size_t i = 0; i < CGptr->_fanoutList.size(); ++i){
      vector<CirGateV>& inputs = CGptr->_fanoutList[i].gate()
                                 ->_faninList;
      for(size_t j = 0; j < inputs.size(); ){
         if(inputs[j].gate() == CGptr){
            size_t invert = CGptr->_faninList[input4connect].isInvert() ^ inputs[j].isInvert();
            inputs.erase(inputs.begin() + j);
            inputs.push_back(CirGateV(plug,invert));
            plug->_fanoutList.push_back
               (CirGateV(CGptr->_fanoutList[i].gate(),invert));
         }
         else
            ++j;
      }
   }
   CirGate* CZ = _gateArr[0];
   for (size_t i = 0; i < CZ->_fanoutList.size();){
      if(CZ->_fanoutList[i].gate() == CGptr)
         CZ->_fanoutList.erase(CZ->_fanoutList.begin() + i);
      else
         ++i;
   }
   cout << "Simplifying: " << plug->_ID << " merging ";
   if(CGptr->_faninList[input4connect].isInvert()) cout << "!";
   cout << CGptr->_ID << "..." << endl;
}

void
CirMgr::opt_Const_Zero(CirGate* CGptr)
{
   for(size_t i = 0; i < CGptr->_fanoutList.size(); ++i){
      vector<CirGateV>& inputs = CGptr->_fanoutList[i].gate()
                                 ->_faninList;
      for(size_t j = 0; j < inputs.size();){
         if(inputs[j].gate() == CGptr){
            size_t invert = inputs[j].isInvert();
            inputs.erase(inputs.begin() + j);
            inputs.push_back(CirGateV(getGate(0),invert));
         }
         else
            ++j;
      }
   }
   for(size_t i = 0; i < CGptr->_faninList.size(); ++i){
      vector<CirGateV>& outputs = CGptr->_faninList[i].gate()
                                  ->_fanoutList;
      for(size_t j = 0; j < outputs.size();){
         if(outputs[j].gate() == CGptr)
            outputs.erase(outputs.begin() + j);
         else
            ++j;
      }
      if(outputs.size() == 0 && (CGptr->_faninList[i].gate()->getTypeStr()!="PI" &&
                                 CGptr->_faninList[i].gate()->getTypeStr()!="CONST")){
         cout << "Simplifying: 0 merging " <<  CGptr->_faninList[i].gate()->_ID << "..." << endl;
         erase(CGptr->_faninList[i].gate());
      }
   }
   cout << "Simplifying: 0 merging " << CGptr->_ID << "..." << endl;
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
void
CirMgr::erase(CirGate* CGptr)
{
   for(size_t i = 0; i < CGptr->_faninList.size(); ++i){
      vector<CirGateV>& outputs = CGptr->_faninList[i].gate()
                                  ->_fanoutList;
      for(size_t j = 0; j < outputs.size() ;){
         if(outputs[j].gate() == CGptr)
            outputs.erase(outputs.begin() + j);
         else
            ++j;
      }
   }
   for(size_t i = 0; i < CGptr->_fanoutList.size(); ++i){
      vector<CirGateV>& inputs = CGptr->_fanoutList[i].gate()
                                  ->_faninList;
      for(size_t j = 0; j < inputs.size() ;){
         if(inputs[j].gate() == CGptr)
            inputs.erase(inputs.begin() + j);
         else
            ++j;
      }
   }
   if(CGptr->getTypeStr() == "AIG") --_numOfAIGs;
   _gateArr[CGptr->_ID] = 0;
   delete CGptr;
}
