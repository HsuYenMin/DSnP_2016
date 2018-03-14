/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;
extern CirMgr *cirMgr;
size_t CirGate::_globalRef_s = 0;
// TODO: Implement memeber functions for class(es) in cirGate.h
/**************************************/
/*          helper functions          */
/**************************************/
static string
unsigned2str(unsigned i)
{
    stringstream ss;
    ss << i;
    return ss.str();
}
/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
    cout << "==================================================" << endl;

    string s = "= ";
    s += this->getTypeStr();
    s += "(";
    s += unsigned2str(_ID);
    s += ")";
    if(_symbol != "")
    {
       s += "\""    ;
       s += _symbol ;
       s += "\""    ;
    }
    s += ", line ";
    s += unsigned2str(_lineNumber);
    while( s.size() < 49 ){
        s += " ";
    }
    s += "=";
    cout << s << endl;

    cout << "==================================================" << endl;
}


void
CirGate::reportFanin(int level) const
{  assert (level >= 0);
   setGlobalRef();
   reportFanin(level, level);
}
void
CirGate::reportFanin(int level, int totalLevel)  const
{
   assert (level >= 0);
   cout << getTypeStr() << " " << _ID;

   bool hasFanin = false;
   bool hasPrint = isGlobalRef();
   bool levelZero = (level == 0);

   for(int i = 0; i < _faninList.size() ; ++i)
   {
      if( _faninList[i].gate() != 0)
         hasFanin = true;
   }

   if(!hasFanin)
      cout << endl;
   else
   {
      if(hasPrint)
         cout << "*" << endl;
      else if(levelZero)
         cout << endl;
      else
      {
         cout << endl;
         for(int i = 0; i < _faninList.size(); ++i)
         {
            if(_faninList[i].gate() == 0)
               continue;
            for(int j = 0; j <= totalLevel - level; ++j)
               cout << "  ";
            if( _faninList[i].isInvert() )
               cout << "!";
            setToGlobalRef();

            (_faninList[i].gate())
            ->reportFanin(level - 1, totalLevel);
         }
      }
   }
}

void
CirGate::reportFanout(int level) const
{  assert (level >= 0);
   setGlobalRef();
   reportFanout(level, level);
}

void
CirGate::reportFanout(int level, int totalLevel) const
{
   assert (level >= 0);

   cout << getTypeStr() << " " << _ID;
   bool hasFanout = false;
   bool hasPrint = isGlobalRef();
   bool levelZero = (level == 0);

   for(int i = 0; i < _fanoutList.size(); ++i)
   {
      if( _fanoutList[i].gate() != 0)
         hasFanout = true;
   }

   if(!hasFanout)
      cout << endl;
   else
   {
      if(hasPrint)
         cout << "*" << endl;
      else if(levelZero)
         cout << endl;
      else
      {
         cout << endl;
         for(int i = 0; i < _fanoutList.size(); ++i)
         {
            if(_fanoutList[i].gate() == 0)
               continue;
            for(int j = 0; j <= totalLevel - level ; ++j)
               cout << "  ";
            if( _fanoutList[i].isInvert() )
               cout << "!";
            setToGlobalRef();

            (_fanoutList[i].gate())
            ->reportFanout(level - 1, totalLevel);
         }
      }
   }
}

void
CirGate::dfsTraversal(vector<const CirGate*>& dfsList) const
{
   for(int i = 0; i < _faninList.size(); i++)
   {
       if( !((_faninList[i]).gate())->isGlobalRef() )
       {
          ((_faninList[i]).gate())->setToGlobalRef();
          ((_faninList[i]).gate())->dfsTraversal(dfsList);
       }
   }
   const CirGate* cirGate4PushBack = this;
   dfsList.push_back(cirGate4PushBack);
}

void
CirGate::dfsTOut(vector<const CirGate*>& dfsList) const
{
   for(int i = 0; i < _fanoutList.size(); i++)
   {
       if( ! ((_fanoutList[i]).gate())->isGlobalRef() )
       {
          ((_fanoutList[i]).gate())->setToGlobalRef();
          ((_fanoutList[i]).gate())->dfsTOut(dfsList);
       }
   }
   const CirGate* cirGate4PushBack = this;
   dfsList.push_back(cirGate4PushBack);
}

void CirGate::printGate() const
{
  cout << " "<< getTypeStr() << " " << _ID << " ";
  int idx = _faninList.size() - 1;
  if( _faninList.size() == 2)
  {
     for(int i = 0; i < _faninList.size() - 1; ++i)
     {
        if( _faninList[i].gate()->getTypeStr() == "UNDEF")
           cout << "*" ;
        if( _faninList[i].isInvert() )
           cout << "!" ;
        cout << (_faninList[i].gate()->getID()) << " ";
     }
        if( _faninList[idx].gate()->getTypeStr() == "UNDEF")
           cout << "*" ;
        if( _faninList[idx].isInvert() )
           cout << "!" ;
        cout << (_faninList[idx].gate()->getID()) << " ";
  }
  if(_symbol != "")
     cout << " (" << _symbol << ")" << endl;
}

/**************************************/
/*  class CirGatePO member functions  */
/**************************************/
CirGatePO::CirGatePO(unsigned ID, unsigned ln)
:CirGate(ID,ln,PO_GATE){}

void
CirGatePO::printGate() const
{
   cout << " PO  " << _ID ;
   if( _faninList.size() == 1)
   {
      if( _faninList[0].gate()->getTypeStr() == "UNDEF"){
         if( _faninList[0].isInvert() )
         cout << " *!" ;
         else
         cout << " *" ;
      }
      else if( _faninList[0].isInvert() )
         cout << " !" ;
      else
         cout << " ";
      cout << (_faninList[0].gate()->getID());
   }
   if(_symbol != "")
      cout << " (" << _symbol << ")";

}
/**************************************/
/*  class CirGatePI member functions  */
/**************************************/

CirGatePI::CirGatePI(unsigned ID, unsigned ln)
:CirGate(ID,ln,PI_GATE){}

void
CirGatePI::printGate() const
{
   cout << " PI  " << _ID;

      if(_symbol != "")
         cout << " (" << _symbol << ")";

}
/**************************************/
/*  class CirGateCZ member functions  */
/**************************************/

CirGateCZ::CirGateCZ()
:CirGate(0,0,CONST_GATE){}

void
CirGateCZ::printGate() const
{
   cout << " CONST0";
}
