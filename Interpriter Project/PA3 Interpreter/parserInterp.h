/* 
 * parserInterp.h
 * Programming Assignment 3
 * Spring 2024
*/

#ifndef PARSER_H_
#define PARSER_H_

#include <iostream>

using namespace std;

#include "lex.h"
#include "val.h"

extern bool Prog(istream& in, int& line);
extern bool Decl(istream& in, int& line);
extern bool Type(istream& in, int& line);
extern bool VarList(istream& in, int& line, LexItem & idtok, int strlen = 1 ); //check 
extern bool Stmt(istream& in, int& line);
extern bool SimpleStmt(istream& in, int& line);
extern bool PrintStmt(istream& in, int& line);
extern bool BlockIfStmt(istream& in, int& line);
extern bool SimpleIfStmt(istream& in, int& line);
extern bool AssignStmt(istream& in, int& line);
extern bool Var(istream& in, int& line, LexItem & idtok); // check 
extern bool ExprList(istream& in, int& line);
extern bool RelExpr(istream& in, int& line, Value & retVal);// check
extern bool Expr(istream& in, int& line, Value & retVal);// check 
extern bool MultExpr(istream& in, int& line, Value & retVal);// check 
extern bool TermExpr(istream& in, int& line, Value & retVal);//check 
extern bool SFactor(istream& in, int& line, Value & retVal);// check 
extern bool Factor(istream& in, int& line, int sign, Value & retVal);//check 

extern int ErrCount();

#endif /* PARSER_H_ */
