/****************************************************************************
  FileName     [ dbCmd.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include <cassert>
#include "util.h"
#include "dbCmd.h"
#include "dbTable.h"

// Global variable
DBTable dbtbl;
// Static functions
static bool checkColIdx(const string& token, int& c)
{
   if (!dbtbl) {
      cerr << "Error: Table is not yet created!!" << endl;
      return false;
   }

   if (!myStr2Int(token, c)) {
      cerr << "Error: " << token << " is not a number!!\n";
      return false;
   }
   if (c < 0 || c >= int(dbtbl.nCols())) {
      cerr << "Error: Column index " << c << " is out of range!!\n";
      return false;
   }
   return true;
}

static bool checkRowIdx(const string& token, int& c)
{
   if (!dbtbl) {
      cerr << "Error: Table is not yet created!!" << endl;
      return false;
   }

   if (!myStr2Int(token, c)) {
      cerr << "Error: " << token << " is not a number!!\n";
      return false;
   }
   if (c < 0 || c >= int(dbtbl.nRows())) {
      cerr << "Error: Row index " << c << " is out of range!!\n";
      return false;
   }
   return true;
}

bool
initDbCmd()
{
   // TODO...
   if(!(cmdMgr->regCmd("DBAPpend", 4, new DBAppendCmd) &&
        cmdMgr->regCmd("DBAVerage", 4, new DBAveCmd) &&
        cmdMgr->regCmd("DBCount", 3,new DBCountCmd) &&
        cmdMgr->regCmd("DBDelete", 3,new DBDelCmd) &&
        cmdMgr->regCmd("DBMAx", 4,new DBMaxCmd) &&
        cmdMgr->regCmd("DBMIn", 4,new DBMinCmd) &&
        cmdMgr->regCmd("DBPrint", 3,new DBPrintCmd) &&
        cmdMgr->regCmd("DBRead", 3,new DBReadCmd) &&
        cmdMgr->regCmd("DBSOrt", 4,new DBSortCmd) &&
        cmdMgr->regCmd("DBSUm", 4,new DBSumCmd) 
     )) {
     cerr << "Registering \" init\" commands fails... esiting" << endl;
     return false;
   }
   return true;
}

//----------------------------------------------------------------------
//    DBAPpend <-Row | -Column> <(int data)...>
//----------------------------------------------------------------------
CmdExecStatus
DBAppendCmd::exec(const string& option)
{
   // TODO...
   // check option
   if (option.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;

   bool doRow = false;
   if (myStrNCmp("-Row", options[0], 2) == 0) doRow = true;
   else if (myStrNCmp("-Column", options[0], 2) != 0)
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
   if(options.size() == 1)
      return CmdExec::errorOption(CMD_OPT_MISSING, options[0]);
   int intTemp;
   if (doRow) {//add row
      DBRow dbr;
      size_t nC = dbtbl.nCols();
      for(size_t i = 1; i < nC + 1; i++){
         if(i < options.size()){
            if(options[i] == "-")
               dbr.addData(INT_MAX);
            else if(myStr2Int(options[i], intTemp))
               dbr.addData(intTemp);
            else 
               return CMD_EXEC_ERROR;
         }
         else
            break;
      }
      while(dbr.size() < nC){
         dbr.addData(INT_MAX);
      }
      dbtbl.addRow(dbr);
   }
   else {//add column
      vector<int> intVec;
      size_t nR = dbtbl.nRows();
      for(size_t i = 1; i < nR + 1; i++){
         if(i < options.size()){
            if(options[i] == "-")
               intVec.push_back(INT_MAX);
            else if(myStr2Int(options[i], intTemp))
               intVec.push_back(intTemp);
            else
               return CMD_EXEC_ERROR;
         }
         else
            break;
      }
      dbtbl.addCol(intVec);
   }

   return CMD_EXEC_DONE;
}

void
DBAppendCmd::usage(ostream& os) const
{
   os << "Usage: DBAPpend <-Row | -Column> <(int data)...>" << endl;
}

void
DBAppendCmd::help() const
{
   cout << setw(15) << left << "DBAPpend: "
        << "append a row or column of data to the table" << endl;
}


//----------------------------------------------------------------------
//    DBAVerage <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBAveCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(option, c)) return CMD_EXEC_ERROR;

   float a = dbtbl.getAve(c);
   ios_base::fmtflags origFlags = cout.flags();
   cout << "The average of column " << c << " is " << fixed
        << setprecision(2) << a << ".\n";
   cout.flags(origFlags);

   return CMD_EXEC_DONE;
}

void
DBAveCmd::usage(ostream& os) const
{     
   os << "Usage: DBAVerage <(int colIdx)>" << endl;
}

void
DBAveCmd::help() const
{
   cout << setw(15) << left << "DBAverage: "
        << "compute the average of a column" << endl;
}


//----------------------------------------------------------------------
//    DBCount <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBCountCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(option, c)) return CMD_EXEC_ERROR;

   int n = dbtbl.getCount(c);
   cout << "The distinct count of column " << c << " is " << n << "." << endl;

   return CMD_EXEC_DONE;
}

void
DBCountCmd::usage(ostream& os) const
{     
   os << "Usage: DBCount <(int colIdx)>" << endl;
}

void
DBCountCmd::help() const
{
   cout << setw(15) << left << "DBCount: "
        << "report the distinct count of data in a column" << endl;
}


//----------------------------------------------------------------------
//    DBDelete <-Row | -Column> <(int index)>
//----------------------------------------------------------------------
CmdExecStatus
DBDelCmd::exec(const string& option)
{
   // check option
   vector<string> options;
   if (!CmdExec::lexOptions(option, options, 2))
      return CMD_EXEC_ERROR;
   
   bool doRow = false;
   if (myStrNCmp("-Row", options[0], 2) == 0) doRow = true;
   else if (myStrNCmp("-Column", options[0], 2) != 0)
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);

   int c;
   if (doRow) {
      if (!checkRowIdx(options[1], c)) return CMD_EXEC_ERROR;
      dbtbl.delRow(c);
   }
   else {
      if (!checkColIdx(options[1], c)) return CMD_EXEC_ERROR;
      dbtbl.delCol(c);
   }

   return CMD_EXEC_DONE;
}

void
DBDelCmd::usage(ostream& os) const
{
   os << "Usage: DBDelete <-Row | -Column> <(int index)>" << endl;
}

void
DBDelCmd::help() const
{
   cout << setw(15) << left << "DBDelete: "
        << "delete a row or column from the table" << endl;
}


//----------------------------------------------------------------------
//    DBMAx <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBMaxCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(option, c)) return CMD_EXEC_ERROR;

   float n = dbtbl.getMax(c);
   cout << "The max data of column " << c << " is " << n << "." << endl;

   return CMD_EXEC_DONE;
}

void
DBMaxCmd::usage(ostream& os) const
{     
   os << "Usage: DBMAx <(int colIdx)>" << endl;
}

void
DBMaxCmd::help() const
{
   cout << setw(15) << left << "DBMAx: "
        << "report the maximum number of a column" << endl;
}


//----------------------------------------------------------------------
//    DBMIn <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBMinCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(option, c)) return CMD_EXEC_ERROR;

   float n = dbtbl.getMin(c);
   cout << "The min data of column " << c << " is " << n << "." << endl;

   return CMD_EXEC_DONE;
}

void
DBMinCmd::usage(ostream& os) const
{     
   os << "Usage: DBMIn <(int colIdx)>" << endl;
}

void
DBMinCmd::help() const
{
   cout << setw(15) << left << "DBMIn: "
        << "report the minimum number of a column" << endl;
}


//----------------------------------------------------------------------
//    DBPrint < (int rowIdx) (int colIdx)
//            | -Row (int rowIdx) | -Column (colIdx) | -Table | -Summary>
//----------------------------------------------------------------------
CmdExecStatus
DBPrintCmd::exec(const string& option)
{  
   // TODO.../
   if (option.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;
   int colIdx, rowIdx;

   if(myStr2Int(options[0], rowIdx)){
      if(!checkRowIdx(options[0], rowIdx))
         return CMD_EXEC_ERROR;
      if(options.size() == 1)
         return CmdExec::errorOption(CMD_OPT_MISSING, options[0]);
      options.clear();
      if(!CmdExec::lexOptions(option, options, 2))//for extra option
         return CMD_EXEC_ERROR;
      if(!myStr2Int(options[1], colIdx))
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
      if(!checkColIdx(options[1], colIdx))
         return CMD_EXEC_ERROR;
      if(dbtbl[rowIdx][colIdx] != INT_MAX)
         cout << dbtbl[rowIdx][colIdx] << endl;
      else
         cout << "null" << endl;
   }
   else{
      bool doRow_Table[] ={1, 1};// {0, 0} for -row, {1, 0} for -column
                                 // {0, 1} for -table, {1, 1} for -summary
      if(myStrNCmp("-Row", options[0], 2) == 0){ 
         doRow_Table[0] = 0;
         doRow_Table[1] = 0;
      }
      else if(myStrNCmp("-Column", options[0], 2) == 0) doRow_Table[1] = 0;
      else if(myStrNCmp("-Table", options[0], 2) == 0) doRow_Table[0] = 0;
      else if(myStrNCmp("-Summary", options[0], 2) != 0){
         checkRowIdx(options[0], rowIdx);
         return CMD_EXEC_ERROR;
      }
      if(doRow_Table[0] == 0 && doRow_Table[1] == 0){// -Row
         if(options.size() == 1)//for missing option
            return CmdExec::errorOption(CMD_OPT_MISSING, options[0]);
         if(!myStr2Int(options[1], rowIdx))//for illegal option
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
         if(!checkRowIdx(options[1], rowIdx))//for out of range
            return CMD_EXEC_ERROR;
         options.clear();
         if(!CmdExec::lexOptions(option, options, 2))//for extra option
            return CMD_EXEC_ERROR;
         cout << dbtbl[rowIdx] << endl;
      }
      else if(doRow_Table[0] == 1 && doRow_Table[1] == 0){// -Column
         if(options.size() == 1)//for missing option
            return CmdExec::errorOption(CMD_OPT_MISSING, options[0]);
         if(!myStr2Int(options[1], colIdx))//for illegal option
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
         if(!checkColIdx(options[1], colIdx))//for out of range
            return CMD_EXEC_ERROR;
         options.clear();
         if(!CmdExec::lexOptions(option, options, 2))//for extra option
            return CMD_EXEC_ERROR;
            dbtbl.printCol(colIdx);
            cout << endl;
      }
      else {// -Table and -Summary
         options.clear();
         if(!CmdExec::lexOptions(option, options, 1))//for extra option
            return CMD_EXEC_ERROR;
         if(!doRow_Table[0])
            cout << dbtbl << endl;
         else
            dbtbl.printSummary();
      }
   }  
   return CMD_EXEC_DONE;
}

void
DBPrintCmd::usage(ostream& os) const
{
   os << "DBPrint < (int rowIdx) (int colIdx)\n"
      << "        | -Row (int rowIdx) | -Column (colIdx) | -Table>" << endl;
}

void
DBPrintCmd::help() const
{
   cout << setw(15) << left << "DBPrint: "
        << "print the data in the table" << endl;
}


//----------------------------------------------------------------------
//    DBRead <(string csvFile)> [-Replace]
//----------------------------------------------------------------------
CmdExecStatus
DBReadCmd::exec(const string& option)
{
   // check option
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;

   if (options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");

   bool doReplace = false;
   string fileName;
   for (size_t i = 0, n = options.size(); i < n; ++i) {
      if (myStrNCmp("-Replace", options[i], 2) == 0) {
         if (doReplace) return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
         doReplace = true;
      }
      else {
         if (fileName.size())
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
         fileName = options[i];
      }
   }

   ifstream ifs(fileName.c_str());
   if (!ifs) {
      cerr << "Error: \"" << fileName << "\" does not exist!!" << endl;
      return CMD_EXEC_ERROR;
   }

   if (dbtbl) {
      if (!doReplace) {
         cerr << "Error: Table exists. Use \"-Replace\" option for "
              << "replacement.\n";
         return CMD_EXEC_ERROR;
      }
      cout << "Table is replaced..." << endl;
      dbtbl.reset();
   }
   if (!(ifs >> dbtbl)) return CMD_EXEC_ERROR;
   cout << "\"" << fileName << "\" was read in successfully." << endl;

   return CMD_EXEC_DONE;
}

void
DBReadCmd::usage(ostream& os) const
{
   os << "Usage: DBRead <(string csvFile)> [-Replace]" << endl;
}

void
DBReadCmd::help() const
{
   cout << setw(15) << left << "DBRead: "
        << "read data from .csv file" << endl;
}
//----------------------------------------------------------------------
//    DBSOrt <(int colIdx)>...
//----------------------------------------------------------------------
CmdExecStatus
DBSortCmd::exec(const string& option)
{
   // check option
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;
   
   if (options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");

   DBSort sortOrders;
   for (int i = 0, n = options.size(); i < n; ++i) {
      int s;
      if (!checkColIdx(options[i], s)) return CMD_EXEC_ERROR;
      sortOrders.pushOrder(s);
   }
   dbtbl.sort(sortOrders);

   return CMD_EXEC_DONE;
}

void
DBSortCmd::usage(ostream& os) const
{
   os << "Usage: DBSOrt <(int colIdx)>..." << endl;
}

void
DBSortCmd::help() const
{
   cout << setw(15) << left << "DBSort: "
        << "sort the data in the table" << endl;
}
//----------------------------------------------------------------------
//    DBSUm <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBSumCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(option, c)) return CMD_EXEC_ERROR;
   float n = dbtbl.getSum(c);
   cout << "The sum of column " << c << " is " << n << "." << endl;

   return CMD_EXEC_DONE;
}
void
DBSumCmd::usage(ostream& os) const
{     
   os << "Usage: DBSUm <(int colIdx)>" << endl;
}
void
DBSumCmd::help() const
{
   cout << setw(15) << left << "DBSUm: "
        << "compute the summation of a column" << endl;
}
