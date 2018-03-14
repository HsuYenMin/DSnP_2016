/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;
class CirGateV;
class CirHashKey;
//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGateV
{
public:

   friend class CirGate;
   friend class CirGatePO;
   friend class CirGatePI;
   friend class CirGateCZ;
   #define NEG 0x1
   CirGateV();
   CirGateV(CirGate* g, size_t phase): _gateV( size_t(g) + phase ) {}

   CirGate* gate()       const { return (CirGate*)(_gateV & ~size_t(NEG)); }
   bool     isInvert()   const { return (_gateV & NEG); }
   size_t   operator()() const { return _gateV; }
   bool     operator== (const CirGateV& cv)  const { return _gateV == cv._gateV; }
private:
//data members
   size_t _gateV;
};

class CirHashKey
{
   public:
      CirHashKey(CirGateV i0, CirGateV i1): _in0(i0), _in1(i1){}
      size_t operator() () const { return _in0() + _in1(); }
      bool operator == (const CirHashKey& k) const 
        { return (( _in0 == k._in0 && _in1 == k._in1) ||
                     ( _in0 == k._in1 && _in1 == k._in0)) ; }

   private:
      CirGateV _in0, _in1;
};

class CirGate
{
public:
   friend class CirMgr;
   CirGate(unsigned ID, unsigned ln = 0, GateType g = UNDEF_GATE)
   : _gateType(g), _ID(ID), _lineNumber(ln){_symbol = ""; _ref = 0; }
   virtual ~CirGate() {}

   // Basic access methods
   void     setType(GateType g) { _gateType  = g ;}
   void     setLineNo(size_t ln){ _lineNumber = ln;}
   string   getTypeStr() const {  switch(_gateType)
                                {
                                    case UNDEF_GATE: return "UNDEF";
                                    case PI_GATE   : return "PI"   ;
                                    case PO_GATE   : return "PO"   ;
                                    case AIG_GATE  : return "AIG"  ;
                                    case CONST_GATE: return "CONST";
                                    case TOT_GATE  : return "TOT"  ;
                                }
                                return "TOT"; }
   unsigned getLineNo() const { return _lineNumber; }
   unsigned getID()      const { return _ID; }
   virtual bool isAig() const { return false; }

   // Printing functions
   virtual void printGate()             const;
   void         reportGate()            const;
   void         reportFanin (int level, int totalLevel)  const;
   void         reportFanin (int level) const;
   void         reportFanout(int level, int totalLevel)  const;
   void         reportFanout(int level) const;

   void         dfsTraversal(vector<const CirGate*>& dfsList) const;
   void         dfsTraversal(vector<CirGate*>& dfsList);
   void         dfsTOut     (vector<const CirGate*>& dfsList) const;
   bool         isGlobalRef()    const { return ( _ref == _globalRef_s); }
   bool         notGlobalRef()   const { return ( _ref != _globalRef_s); }
   static void  setGlobalRef()         { ++_globalRef_s; }
   void         setToGlobalRef() const { _ref = _globalRef_s; }

   void         fanout_push_back(CirGateV V) { _fanoutList.push_back(V); }
   void         fanin_push_back(CirGateV V) { _faninList.push_back(V); }
   // Member functions about circuit optimization
   void         sweep(); 
private:

protected:
  GateType         _gateType   ;
  unsigned         _ID         ;
  unsigned         _lineNumber ;
  string           _symbol     ;
  vector<CirGateV> _fanoutList ;
  vector<CirGateV> _faninList  ;
  static size_t _globalRef_s   ;
  mutable  size_t  _ref        ;

};

class CirGatePO: public CirGate
{
public:
         CirGatePO(unsigned ID, unsigned ln);
    void printGate() const;
};

class CirGatePI: public CirGate
{
public:
         CirGatePI(unsigned ID, unsigned ln);
    void printGate() const;
};

//const zero
class CirGateCZ: public CirGate
{
public:
         CirGateCZ();
    void printGate() const;
};

#endif // CIR_GATE_H
