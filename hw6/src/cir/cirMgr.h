/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(){}
   ~CirMgr() {resetGateArr();}

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const { if(gid < _gateArr.size())
                                             return _gateArr[gid];
                                          else return 0; }

   // Member functions about circuit construction
   bool readCircuit(const string&);
   bool readAIG(const string&);
   bool readSYMBOL(const string&);
   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;
   void dfsTraversal(vector<const CirGate*>& dfsList) const;
   //
   bool resetGateArr(){ 
                        for(size_t i = 0; i < _gateArr.size(); ++i)
                           if(!_gateArr[i]) delete (_gateArr[i]);
                        _POs.clear(); _PIs.clear(); _gateArr.clear(); 
                        return false;
                      }

private:
   vector<CirGate*> _POs       ;
   vector<CirGate*> _PIs       ;
   unsigned         _L         ;
   unsigned         _numOfAIGs ;
   vector<CirGate*> _gateArr   ;
};

#endif // CIR_MGR_H
