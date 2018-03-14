/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr
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

static bool
myStr2Size_t(const string& str, size_t& num)
{
   num = 0;
   size_t i = 0;
   bool valid = false;
   for (; i < str.size(); ++i) {
      if (isdigit(str[i])) {
         num *= 10;
         num += int(str[i] - '0');
         valid = true;
      }
      else return false;
   }
   return valid;
}

static bool
lexSingleOption
(const string& option, string& token, bool optional = false)
{
   size_t n = myStrGetTok(option, token);
   if (!optional) {
      if (token.size() == 0) {
         return false;
      }
   }
   if (n != string::npos) {
      return false;
   }
   return true;
}

// if nOpts is specified (!= 0), the number of tokens must be exactly = nOpts
// Otherwise, return false.
//
static bool
lexOptions
(const string& option, vector<string>& tokens, size_t nOpts=0)
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
   if (nOpts != 0) {
      if (tokens.size() < nOpts) {
         return false;
      }
      if (tokens.size() > nOpts) {
         return false;
      }
   }
   return true;
}
/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};
// self define enum
enum CirParseState {
   INIT,
   HEADER,
   PI,
   PO,
   AIG,
   SYMBOL,
   COMMENT,
   DONE
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
//static char buf[1024]
static string buf = "";
static string errMsg;
static int errInt;
static CirGate *errGate;
static CirParseState state = INIT;
static size_t M=0,I=0,L=0,O=0,A=0;
static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
static bool
readHEADER(string& header, vector<string>& tokens){   
   if(!lexOptions(header, tokens, 6)){
      return false;
   }
   else{
      size_t countDel = 0;
      //TODO use state machine to count col 
      for(size_t i =0; i < header.size(); ++i){
         if (header[i] == ' '){++countDel; ++colNo;}
         else if (header[i] == '\t') { errInt = 9; return parseError(ILLEGAL_WSPACE);}
      }
      if(countDel > 5) return parseError(EXTRA_SPACE);
      if(tokens[0] != "aag"){ errMsg = tokens[0];
                              return parseError(ILLEGAL_IDENTIFIER);}
      //check tokens[1]~token[5] is size_t 
      if(!myStr2Size_t(tokens[1],M)){ errMsg = tokens[1];
                                      return parseError(ILLEGAL_NUM);}
      if(!myStr2Size_t(tokens[2],I)){ errMsg = tokens[2];
                                      return parseError(ILLEGAL_NUM);}
      if(!myStr2Size_t(tokens[3],L)){ errMsg = tokens[3];
                                      return parseError(ILLEGAL_NUM);}
      if(!myStr2Size_t(tokens[4],O)){ errMsg = tokens[4];
                                      return parseError(ILLEGAL_NUM);}
      if(!myStr2Size_t(tokens[5],A)){ errMsg = tokens[4];
                                      return parseError(ILLEGAL_NUM);}
      if(M < I + L + A) return false;
      return true;
   }
}
bool
CirMgr::readAIG(const string& line){
   //case 1: the first char is digit
   //case 2: the first char is [ilo]
   //case 3: the first char is c
   if(isdigit(line[0])){ 
      vector<string> tokens;
      size_t LHS=0, RHS1 = 0, RHS2 = 0;
      if(!lexOptions(line, tokens, 3))  return false;
      if(!myStr2Size_t(tokens[0],LHS))  return false;
      if(!myStr2Size_t(tokens[1],RHS1)) return false;
      if(!myStr2Size_t(tokens[2],RHS2)) return false;

      if(LHS  > M*2)  { errInt = LHS;   return parseError(MAX_LIT_ID);}
      if(RHS1 > M*2+1){ errInt = RHS1;  return parseError(MAX_LIT_ID);}
      if(RHS2 > M*2+1){ errInt = RHS2;  return parseError(MAX_LIT_ID);}
      if(LHS % 2){ errMsg="AIG"; errInt = LHS; return parseError(CANNOT_INVERTED);}

      unsigned AIG = LHS/2;
      if( _gateArr[AIG] == 0)
         _gateArr[AIG] = new CirGate(AIG, lineNo+1, AIG_GATE);
      else if( _gateArr[AIG] != 0 && _gateArr[AIG]->getTypeStr() != "UNDEF") 
      {  errInt = LHS; return parseError(REDEF_GATE);}
      else if( _gateArr[AIG] != 0){
         _gateArr[AIG]->setType(AIG_GATE);
         _gateArr[AIG]->setLineNo(lineNo+1);
      }
      for(unsigned j = 0; j < 2; ++j)
      {
         size_t temp = (j==0)?RHS1:RHS2;

         if(temp % 2)
         {
            if(_gateArr[(temp - 1) / 2] == 0)
               _gateArr[(temp - 1) / 2] = new CirGate((temp - 1) / 2);
            _gateArr[AIG]->fanin_push_back(CirGateV(_gateArr[(temp-1)/2],1));
            _gateArr[(temp-1)/2]->fanout_push_back(CirGateV(_gateArr[AIG],1));

         }
         else
         {
            if(_gateArr[temp / 2] == 0)
               _gateArr[temp / 2] = new CirGate(temp / 2);
            _gateArr[AIG]->fanin_push_back(CirGateV(_gateArr[temp/2],0));
            _gateArr[temp/2]->fanout_push_back(CirGateV(_gateArr[AIG],0));
         }
      }
      return true;
   }
   else if(line[0] == 'i' || line[0] == 'o'){state = SYMBOL;  return readSYMBOL(line);}
   else if(line[0] == 'c')                  {state = COMMENT; return true;}
   else return false;
}
bool
CirMgr::readSYMBOL(const string& line){
   string token, symbol;
   size_t order = 0;
   //case 1: the first char is [ilo]
   //case 2: the first char is c
   if(line[0] == 'i'){ 
      token = line.substr(1,line.find_first_of(' ') - 1);
      if(!myStr2Size_t(token,order)) {return false;}
      if(order >= _PIs.size())       {return false;}
      if(_PIs[order]->_symbol != "") {errMsg = "PI"; errInt = order; 
                                      return parseError(REDEF_SYMBOLIC_NAME);}
      symbol = line.substr(line.find_first_of(' ') + 1);
      _PIs[order]->_symbol = symbol;
      return true;
   }
   else if(line[0] == 'o'){
      token = line.substr(1,line.find_first_of(' ') - 1);
      if(!myStr2Size_t(token,order)) {return false;}
      if(order >= _POs.size())       {return false;}
      if(_POs[order]->_symbol != "") {errMsg = "PO"; errInt = order; 
                                      return parseError(REDEF_SYMBOLIC_NAME);}
      symbol = line.substr(line.find_first_of(' ')+1);
      _POs[order]->_symbol = symbol;
      return true;
   }
   else if(line[0] == 'c'){state = COMMENT; return true;}
   else return false;
}
bool
CirMgr::readCircuit(const string& fileName)
{
   fstream fin;
   vector<string> tokens;
   string         token="";
   size_t         id = 0;
   // set static variables 
   state = INIT;
   lineNo = colNo = M = I = L = O = A = 0;
   errMsg = "";
   errInt = 0;
   //
   _numOfAIGs = 0;
   //
   CirGate* constZero = 0;
   constZero = new CirGateCZ;
   _gateArr.push_back(constZero);
   fin.open(fileName, ios::in);
   if(!fin.is_open()){
      cout << "Cannot open design \"" << fileName << "\"!!" << endl;
      return false;
   }
   if(!getline(fin, buf)) return parseError(MISSING_NEWLINE);
   tokens.clear();
   state = HEADER; 
   if(!readHEADER(buf, tokens)) return false;
   // successfully read the header
   for (unsigned i = 0; i < M + O; ++i)
   {
      CirGate* cirGate4PushBack = 0;
      _gateArr.push_back( cirGate4PushBack );
   }
   state = PI;
   for(size_t i = 0; i < I; ++i){
      ++lineNo; colNo = 0;
      if(!getline(fin, buf)) return parseError(MISSING_NEWLINE);
      if(!lexSingleOption(buf,token)) return false;
      if(!myStr2Size_t(token,id)){ 
         errMsg = token;
         return parseError(ILLEGAL_NUM);
      }
      if(id % 2){ errMsg="PI"; errInt = id; return parseError(CANNOT_INVERTED);}
      if(id > M*2){ errInt = id; return parseError(MAX_LIT_ID);}
      if(_gateArr[id/2] !=0){ errInt = id; errGate = _gateArr[id/2]; return parseError(REDEF_GATE);} 
      id /= 2;
      _gateArr[id] = new CirGatePI(id, lineNo+1);
      _PIs.push_back(_gateArr[id]);
   }
   state = PO;
   for(size_t i = 0; i < O; ++i){
      ++lineNo; colNo = 0;
      if(!getline(fin, buf)) return parseError(MISSING_NEWLINE);
      if(!lexSingleOption(buf,token)) return false;
      if(!myStr2Size_t(token,id)){ 
         errMsg = token;
         return parseError(ILLEGAL_NUM);
      }
      if(id > M*2 + 1){ errInt = id; return parseError(MAX_LIT_ID);}
      _gateArr[M + 1 + i] = new CirGatePO( M + 1 + i, lineNo + 1);
      _POs.push_back(_gateArr[M + 1 + i]);
      if(id % 2)
      {
         if(_gateArr[(id - 1) / 2] == 0)
            _gateArr[(id - 1) / 2] = new CirGate((id - 1) / 2);
         _gateArr[M+1+i]->fanin_push_back(CirGateV(_gateArr[(id-1)/2],1));
         _gateArr[(id-1)/2]->fanout_push_back(CirGateV(_gateArr[M+1+i],1));

      }
      else
      {
         if(_gateArr[id / 2] == 0)
            _gateArr[id / 2] = new CirGate(id / 2);
         _gateArr[M+1+i]->fanin_push_back(CirGateV(_gateArr[id/2],0));
         _gateArr[id/2]->fanout_push_back(CirGateV(_gateArr[M+1+i],0));
      }
   }
   state = AIG;
   ++lineNo; colNo = 0;
   while(getline(fin, buf)){
      switch(state){
         case AIG:
            if(!readAIG(buf)) return resetGateArr();
            if(state == AIG) ++_numOfAIGs;
            break;
         case SYMBOL:
            if(!readSYMBOL(buf)) return resetGateArr();
            break;
         default:
            return false;
            break;
      }
      if(state == COMMENT) break;
      ++lineNo; colNo = 0;
   }
   if(state == COMMENT){
      ++lineNo; colNo = 0;
      if(!getline(fin, buf)) return parseError(MISSING_NEWLINE);
      /*
      do{
      }
      while(getline(fin,buf));
      */
   }
   _L = L;    
   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << endl << "Circuit Statistics" << endl << "==================" << endl
        << "  PI    " << setw(8) << right << _PIs.size() << endl
        << "  PO    " << setw(8) << right << _POs.size() << endl
        << "  AIG   " << setw(8) << right << _numOfAIGs  << endl
        << "------------------"  << endl
        << "  Total " << setw(8) << (_PIs.size() + _POs.size() + _numOfAIGs)
        << endl;

}


void
CirMgr::printNetlist() const
{
   vector<const CirGate*> dfsList;
   dfsTraversal(dfsList);
   cout << endl;
   size_t j = 0;
   for(int i = 0; i < dfsList.size(); ++i)
   {
      if(dfsList[i]->getTypeStr() != "UNDEF"){
         cout << "[" << j << "]";
         (dfsList[i]->printGate());
         cout << endl;
         ++j;
      }
   }
}
void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(int i = 0; i < _PIs.size(); ++i)
   {
      cout << " " << (_PIs[i]->getID());
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(int i = 0; i < _POs.size(); ++i)
   {
      cout << " " << (_POs[i]->getID());
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   string s1 = "Gates with floating fanin(s):";
   string s2 = "Gates defined but not used  :";
   size_t size = _gateArr.size();
   for(size_t i = 1; i < size; ++i){// except CONST
      if (_gateArr[i] && _gateArr[i]->getTypeStr() != "PI" 
          && _gateArr[i]->getTypeStr() != "UNDEF"){
         if(_gateArr[i]->getTypeStr()=="PO" &&(_gateArr[i]->_faninList)[0].gate()->getTypeStr() == "UNDEF" )
         {  s1 += " "; s1 += unsigned2str(i);}
         else if(_gateArr[i]->getTypeStr()=="AIG" && 
               ((_gateArr[i]->_faninList)[0].gate()->getTypeStr() == "UNDEF" 
                ||(_gateArr[i]->_faninList)[1].gate()->getTypeStr() == "UNDEF"))
         {  s1 += " "; s1 += unsigned2str(i);}
      }
   }
   for(size_t i = 1; i < size; ++i){// except CONST
      if (_gateArr[i] && _gateArr[i]->getTypeStr() != "PO"
          && _gateArr[i]->getTypeStr() != "UNDEF"){
         if( ((_gateArr[i]->_faninList).size()  == 2
          && (_gateArr[i]->_fanoutList).size() == 0) || (_gateArr[i]->getTypeStr() == "PI" &&(_gateArr[i]->_fanoutList).size() == 0))
         { s2 += " "; s2 += unsigned2str(i);}
      }
   }
   if(s1.size() > 29) cout << s1 << endl;
   if(s2.size() > 29) cout << s2 << endl;
}

void
CirMgr::writeAag(ostream& outfile) const
{
   outfile << "aag " << (_gateArr.size() - _POs.size() - 1) << " "
           << _PIs.size() << " " << _L << " " << _POs.size()
           << " ";
   vector<const CirGate*> dfsList;
   dfsTraversal(dfsList);
   size_t c = 0;
   for (size_t i = 0; i < dfsList.size() ; ++i){
     if(   dfsList[i]->getTypeStr() != "PO"
        && dfsList[i]->getTypeStr() != "PI"
        && dfsList[i]->getTypeStr() != "CONST"
        && dfsList[i]->getTypeStr() != "UNDEF")
        ++c;
   }
   outfile << c << endl;
   for(size_t i = 0; i < _PIs.size(); ++i)
   {  outfile << ((_PIs[i]->_ID) * 2) << endl;}
   for(size_t i = 0; i < _POs.size(); ++i){
      if((_POs[i]->_faninList[0]).isInvert())
         outfile << (((_POs[i]->_faninList[0]).gate())->_ID * 2 + 1) << endl;
      else
         outfile << (((_POs[i]->_faninList[0]).gate())->_ID *2) << endl;
   }
   for(size_t i = 0; i < dfsList.size(); ++i){
      if(dfsList[i]->getTypeStr() == "AIG"){
         outfile << (dfsList[i]->_ID * 2);
         for(size_t j = 0; j < 2; ++j){
            if((dfsList[i]->_faninList[j]).isInvert())
               outfile << " "
               << (((dfsList[i]->_faninList[j]).gate())->_ID * 2 + 1) ;
            else
               outfile << " "
               << (((dfsList[i]->_faninList[j]).gate())->_ID *2);
         }
         cout << endl;
      }
   }
   for(size_t i = 0; i < _PIs.size(); ++i){
      if(_PIs[i]->_symbol != ""){
         outfile << "i" << i << " " << _PIs[i]->_symbol << endl;
      }
   }
   for(size_t i = 0; i < _POs.size(); ++i){
      if(_POs[i]->_symbol != ""){
         outfile << "o" << i << " " << _POs[i]->_symbol << endl;
      }
   }
   cout << "c" << endl;
}

void
CirMgr::dfsTraversal(vector<const CirGate*>& dfsList) const
{
   CirGate::setGlobalRef();
   for(int i = 0; i < _POs.size(); ++i)
   {  _POs[i]->dfsTraversal(dfsList);}
}
