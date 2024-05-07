/* Implementation of Recursive-Descent Parser
 * for the SFort95 Language
 * parser(SP24).cpp
 * Programming Assignment 2
 * Spring 2024
*/
/* Implementation of Interpreter for the SFort95 Language
 * parserInterp.cpp
 * Programming Assignment 3
 * Spring 2024
*/

#include "parserInterp.h"
#include "val.h"
#include <cstdlib>
#include <cctype>
#include <string>
using namespace std;  
map<string, bool> defVar;
map<string, Token> SymTable;

map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool IdentList(istream& in, int& line);


//Program is: Prog = PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
bool Prog(istream& in, int& line)
{
	bool dl = false, sl = false;
	LexItem tok = Parser::GetNextToken(in, line);
		
	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
			dl = Decl(in, line);
			if( !dl  )
			{
				ParseError(line, "Incorrect Declaration in Program");
				return false;
			}
			sl = Stmt(in, line);
			if( !sl  )
			{
				ParseError(line, "Incorrect Statement in program");
				 
			}	
			tok = Parser::GetNextToken(in, line);
			
			if (tok.GetToken() == END) {
				tok = Parser::GetNextToken(in, line);
				
				if (tok.GetToken() == PROGRAM) {
					tok = Parser::GetNextToken(in, line);
					
					if (tok.GetToken() == IDENT) { //HERE 
						cout << "(DONE)" << endl;
						/*
						cout << "Contents of defVar map:" << endl;
						for (const auto& entry : defVar) {
    						cout << "Key: " << entry.first << "- Value: " << (entry.second ? "true" : "false") << endl;
						}
						cout << "Contents of SymTable map:" << endl;
						for (const auto& entry : SymTable) {
    					cout << "Key: " << entry.first << "- Value: " << entry.second<< endl;
						}
						cout << "Contents of TempsResults map:" << endl;
						for (const auto& entry : TempsResults) {
    					cout << "Key: " << entry.first << "- Value: " << entry.second <<" type :: "<< entry.second.GetType()<< endl;
						}*/
						
						return true;
					}
					else
					{
						ParseError(line, "Missing Program Name");
						return false;
					}	
				}
				else
				{
					ParseError(line, "Missing PROGRAM at the End");
					return false;
				}	
			}
			else
			{
				ParseError(line, "Missing END of Program");
				return false;
			}	
		}
		else
		{
			ParseError(line, "Missing Program name");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	
	else
	{
		ParseError(line, "Missing Program keyword");
		return false;
	}
}

//Decl ::= Type :: VarList 
//Type ::= INTEGER | REAL | CHARARACTER [(LEN = ICONST)] 
bool Decl(istream& in, int& line) {
	bool status = false;
	LexItem tok;
	string strLen;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	if(t == INTEGER || t == REAL || t == CHARACTER ) {
		
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == DCOLON) {
			status = VarList(in, line, t);
			
			if (status)
			{
				status = Decl(in, line);
				if(!status)
				{
					ParseError(line, "Declaration Syntactic Error.");
					return false;
				}
				return status;
			}
			else
			{
				ParseError(line, "Missing Variables List.");
				return false;
			}
		}
		else if(t == CHARACTER && tok.GetToken() == LPAREN)
		{
			tok = Parser::GetNextToken(in, line);
			
			if(tok.GetToken() == LEN)
			{
				tok = Parser::GetNextToken(in, line);
				
				if(tok.GetToken() == ASSOP)
				{
					tok = Parser::GetNextToken(in, line);
					
					if(tok.GetToken() == ICONST)
					{ 
						strLen = tok.GetLexeme();  

						tok = Parser::GetNextToken(in, line);
						if(tok.GetToken() == RPAREN)
						{
							tok = Parser::GetNextToken(in, line);
							if(tok.GetToken() == DCOLON)
							{
								status = VarList(in, line, t, stoi(strLen));
								
								if (status)
								{
									//cout << "Definition of Strings with length of " << strLen << " in declaration statement." << endl;
									status = Decl(in, line);
									if(!status)
									{
										ParseError(line, "Declaration Syntactic Error.");
										return false;
									}
									return status;
								}
								else
								{
									ParseError(line, "Missing Variables List.");
									return false;
								}
							}
						}
						else
						{
							ParseError(line, "Missing Right Parenthesis for String Length definition.");
							return false;
						}
					
					}
					else
					{
						ParseError(line, "Incorrect Initialization of a String Length");
						return false;
					}
				}
			}
		}
		else
		{
			ParseError(line, "Missing Double Colons");
			return false;
		}
			
	}
		
	Parser::PushBackToken(t);
	return true;
}//End of Decl

//Stmt ::= AssigStmt | BlockIfStmt | PrintStmt | SimpleIfStmt
bool Stmt(istream& in, int& line) {
	bool status;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case PRINT:
		status = PrintStmt(in, line);
		
		if(status)
			status = Stmt(in, line);
		break;

	case IF:
		status = BlockIfStmt(in, line);
		if(status)
			status = Stmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
        status = AssignStmt(in, line);
		if(status)
			status = Stmt(in, line);
		break;
		
	
	default:
		Parser::PushBackToken(t);
		return true;
	}

	return status;
}//End of Stmt

bool SimpleStmt(istream& in, int& line) {
	bool status;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case PRINT:
		status = PrintStmt(in, line);
		
		if(!status)
		{
			ParseError(line, "Incorrect Print Statement");
			return false;
		}	
		//cout << "Print statement in a Simple If statement." << endl;
		break;

	case IDENT:
		Parser::PushBackToken(t);
        status = AssignStmt(in, line);
		if(!status)
		{
			ParseError(line, "Incorrect Assignent Statement");
			return false;
		}
		//cout << "Assignment statement in a Simple If statement." << endl;
			
		break;
		
	
	default:
		Parser::PushBackToken(t);
		return true;
	}

	return status;
}//End of SimpleStmt

//VarList ::= Var [= Expr] {, Var [= Expr]}
bool VarList(istream& in, int& line, LexItem & idtok, int strlen) {
	bool status = false, exprstatus = false;
	string identstr;
	 
	LexItem tok = Parser::GetNextToken(in, line);
	 Value val;
	if(tok == IDENT)
	{
		
		identstr = tok.GetLexeme();
		auto it = defVar.find(identstr);
		if (it == defVar.end())
		{
			if(idtok.GetToken() == REAL || idtok.GetToken() == ICONST){
				defVar[identstr] = false;
			}else{
				defVar[identstr] = true;
			}
			SymTable[identstr] = idtok.GetToken();
			//cout<<identstr+" :: "+idtok.GetLexeme()<<endl;
		}	
		else
		{
			ParseError(line, "Variable Redefinition");
			return false;
		}
		
	}
	else
	{
		
		ParseError(line, "Missing Variable Name");
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok == ASSOP)
	{	
		 
		exprstatus = Expr(in, line, val);
		if(!exprstatus)
		{
			ParseError(line, "Incorrect initialization for a variable.");
			return false;
		}
		
		//cout<< "Initialization of the variable " << identstr <<" in the declaration statement." << endl;
		if(idtok.GetToken() == REAL || idtok.GetToken() == ICONST){
			defVar[identstr] = true;
		}
		val.SetstrLen(strlen); 
		TempsResults[identstr] = val;
		tok = Parser::GetNextToken(in, line);
		
		if (tok == COMMA) {
			
			status = VarList(in, line, idtok, strlen);
			
		}
		else
		{	
			for(auto it = TempsResults.begin(); it != TempsResults.end(); ++it){
				if(SymTable[it->first] == REAL && it->second.IsInt()){
					it->second = Value(static_cast<double>(it->second.GetInt()));
					}
				if(it->second.GetType() == VSTRING){
					string outstring;
					Value outvalue;
					int slen = it->second.GetString().length();
					string instring = it->second.GetString();
					int spaces = strlen - slen;
					if(spaces<0){spaces = spaces*(-1);}
					if(strlen>slen){
						outstring = instring.append(spaces, ' ');
						outvalue = Value(outstring);
						outvalue.SetstrLen(strlen);
						it->second = outvalue;  
				}else if(strlen<slen){
					outstring = instring.substr(0, strlen);
					outvalue = Value(outstring);
					outvalue.SetstrLen(strlen);
					it->second = outvalue; 
				}
			}
			}
			Parser::PushBackToken(tok);
			return true;
		}
	}
	else if (tok == COMMA) {
		
		status = VarList(in, line, idtok, strlen);
	}
	else if(tok == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		
		return false;
	}
	else { 
		for(auto it = TempsResults.begin(); it != TempsResults.end(); ++it){
			if(SymTable[it->first] == REAL && it->second.IsInt()){
				it->second = Value(static_cast<double>(it->second.GetInt()));
				
			}
		}
		for(auto it = SymTable.begin(); it != SymTable.end(); ++it){
			if(it->second == CHARACTER){
				if(TempsResults.count(it->first)<1){ 
					string outstring = "";
					Value outvalue = Value(outstring);
					outvalue.SetstrLen(strlen);
					TempsResults[it->first] = outvalue; 
				}
			}
		}
		Parser::PushBackToken(tok);
		return true;
	}
		
	for(auto it = SymTable.begin(); it != SymTable.end(); ++it){
			if(it->second == CHARACTER){
				if(TempsResults.count(it->first)<1){ 
					string outstring = "";
					Value outvalue = Value(outstring);
					outvalue.SetstrLen(strlen);
					TempsResults[it->first] = outvalue; 
				}
			}
		}
	return status;
	
}//End of VarList
	


//PrintStmt:= PRINT *, ExpreList 
bool PrintStmt(istream& in, int& line) {
	LexItem t;
	ValQue = new queue<Value>;

	t = Parser::GetNextToken(in, line);
 	if( t != DEF ) {
		
		ParseError(line, "Print statement syntax error.");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	
	if( t != COMMA ) {
		
		ParseError(line, "Missing Comma.");
		return false;
	}
	bool ex = ExprList(in, line);
	
	if( !ex ) { //  delete que
		while (!(*ValQue).empty())
		{
		ValQue->pop();
		}
		ParseError(line, "Missing expression after Print Statement");
		return false;
	}
	
	while(!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		bool inMap = false; 
		switch(nextVal.GetType()){
			case VSTRING:
			inMap = (TempsResults.find(nextVal.GetString()) != TempsResults.end()) ? true : false;
			if(inMap){
				cout<<TempsResults[nextVal.GetString()];
			}else{
				 cout<<nextVal.GetString(); 
			}
			break;
			case VINT:
			cout<<nextVal.GetInt();
			break;
			case VREAL:
			cout<<nextVal.GetReal(); 
			break;
			default:
			cout<<nextVal; 
			break; 
		}
		ValQue->pop();
	}
	cout << endl;
	return ex;
}//End of PrintStmt

//BlockIfStmt:= if (Expr) then {Stmt} [Else Stmt]
//SimpleIfStatement := if (Expr) Stmt
bool BlockIfStmt(istream& in, int& line) {
	bool ex=false, status ; 
	static int nestlevel = 0;
	//int level;
	LexItem t;
	
	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	Value retVal;

	ex = RelExpr(in, line, retVal);
	if(!ex){
		return ex; 
	}
	if(retVal.GetType() == VREAL){
		ParseError(line, "Run-Time Error-Illegal Type for If statement condition");
		return false; 
	}
	//check if stamnt to be true or false.
	if( retVal.GetType() != VBOOL){
		ParseError(line, "Run-Time Error-Illegal Type for If statement condition");
		return false; 
	}
	if( retVal.GetBool() == false) {
		while(t != ELSE){
			t = Parser::GetNextToken(in, line);
		}
	}else{
		t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if(t != THEN)
	{
		Parser::PushBackToken(t);
		
		status = SimpleStmt(in, line);
		if(status)
		{
			return true;
		}
		else
		{
			ParseError(line, "If-Stmt Syntax Error");
			return false;
		}
		
	}
	}
	
	
	
	nestlevel++;
	//level = nestlevel;
	status = Stmt(in, line);
	if(!status)
	{
		ParseError(line, "Missing Statement for If-Stmt Then-Part");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if( t == ELSE ) {
		status = Stmt(in, line);
		if(!status)
		{
			ParseError(line, "Missing Statement for If-Stmt Else-Part");
			return false;
		}
		else
		  t = Parser::GetNextToken(in, line);
		
	}
	
	
	if(t != END ) {
		
		ParseError(line, "Missing END of IF");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if(t == IF ) {
		//cout << "End of Block If statement at nesting level " << level << endl;
		return true;
	}	
	
	Parser::PushBackToken(t);
	ParseError(line, "Missing IF at End of IF statement");
	return false;
}//End of IfStmt function

//Var:= ident
bool Var(istream& in, int& line, LexItem & idtok)
{
	string identstr;
	 idtok = Parser::GetNextToken(in, line);
	if (idtok == IDENT){
		identstr = idtok.GetLexeme(); 
		defVar[identstr] = true; 
		//cout<<identstr+" This is in Var"<<endl;
		return true;
	}
	else if(idtok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << idtok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}//End of Var

//AssignStmt:= Var = Expr
bool AssignStmt(istream& in, int& line) {
	
	bool varstatus = false, status = false;
	LexItem t;
	varstatus = Var( in, line, t);
	Value retVal; 
	string ident = t.GetLexeme();
	//cout<<ident+" this is in ASSstmt"<<endl;
	if (varstatus){
		t = Parser::GetNextToken(in, line);
		if (t == ASSOP){
			status = Expr(in, line, retVal);
			if(!status) {
				ParseError(line, "Missing Expression in Assignment Statment");
				return status;
			}
			if(SymTable[ident] == CHARACTER && SymTable[retVal.GetString()] == REAL){
					ParseError(--(--line), "Illegal mixed-mode assignment operation");
					return false; 
				}
			if(status){
				if(retVal.GetType() == VSTRING){
					for(auto it = TempsResults.begin(); it != TempsResults.end(); ++it){
					if(it->first == ident){
						int strlen = it->second.GetstrLen();
						retVal.SetstrLen(strlen);
						TempsResults[ident] = retVal; 
					}
				}
				}else{
					TempsResults[ident] = retVal;
				}	
			
				
			}
		}
		else if(t.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else {
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
	}
	else {
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	for(auto it = TempsResults.begin(); it != TempsResults.end(); ++it){
			if(SymTable[it->first] == REAL && it->second.IsInt()){
				it->second = Value(static_cast<double>(it->second.GetInt()));
			}
			if(it->second.GetType() == VSTRING){
				string outstring;
					Value outvalue;
					int slen = it->second.GetString().length();
					string instring = it->second.GetString();
					int strlen = it->second.GetstrLen(); 
					int spaces = strlen - slen;
					if(spaces<0){spaces = spaces*(-1);}
					if(strlen>slen){
						outstring = instring.append(spaces, ' ');
						outvalue = Value(outstring);
						outvalue.SetstrLen(strlen);
						it->second = outvalue;  
				}else if(strlen<slen){
					outstring = instring.substr(0, strlen);
					outvalue = Value(outstring);
					outvalue.SetstrLen(strlen);
					it->second = outvalue; 
				}
			}
			
		}
		
	
	
	return status;	
}//End of AssignStmt

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	Value retVal;

	status = Expr(in, line, retVal);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	
	
		ValQue->push(retVal);
	LexItem tok = Parser::GetNextToken(in, line);
	if (tok == COMMA) {
		
		status = ExprList(in, line);
		
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//End of ExprList

//RelExpr ::= Expr  [ ( == | < | > ) Expr ]
bool RelExpr(istream& in, int& line, Value & retVal) {
	
	bool t1 = Expr(in, line, retVal);
	LexItem tok;
	Value value1 = TempsResults[retVal.GetString()];
	
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok == EQ || tok == LTHAN || tok == GTHAN) 
	{
		t1 = Expr(in, line, retVal);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		try{
			Value value2 = Value(static_cast<double>(retVal.GetInt()));   
		if(tok == EQ){
			if(value1.GetReal() == value2.GetReal()){
				retVal = Value(true);
			}else{
				retVal = Value(false);
			}
		} 
		}catch(char const* e){
		cerr<<e<<endl;
		cout<<"in real expr"<<endl; 
		}
	}
	
	return true;
}//End of RelExpr

//Expr ::= MultExpr { ( + | - | // ) MultExpr }
bool Expr(istream& in, int& line, Value & retVal) {
	bool t1 = MultExpr(in, line, retVal);
	LexItem tok;
	 
	
	if( !t1 ) {
		return false;
	}
	
	Value value1 = retVal;
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == PLUS || tok == MINUS || tok == CAT) 
	{
		t1 = MultExpr(in, line, retVal);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		Value value2 = retVal;
		//check if val 1 or val 2 are in map
		bool val1 = false, val2 = false; 
		if(value1.GetType() == VSTRING){
			 val1 = (TempsResults.find(value1.GetString()) != TempsResults.end()) ? true : false;
		}
		if(value2.GetType() == VSTRING){
			 val2 = (TempsResults.find(value2.GetString()) != TempsResults.end()) ? true : false;
		}
		// do operations accordinglly
			switch(tok.GetToken()){
			case PLUS:
				if(val1){
					value1 = TempsResults[value1.GetString()] + value2;
				}else if(val2){
					value1 = TempsResults[value2.GetString()] + value1;
				}else{
					value1 = value1 + value2; 
				}
				if(value2.GetType() == VSTRING || value1.GetType() == VSTRING){
				line = line - 2;
				ParseError(line, "Illegal operand type for the operation.");
				return false;
				}
			break;
			case CAT:
			if(val1 == true && val2 == false){
					value1 = TempsResults[value1.GetString()].Catenate(value2); 
				}else if(val2 == true && val1 == false){
					value1 = value1.Catenate(TempsResults[value2.GetString()]); 
				}else if(val1 == true && val2 == true){
					value1 = TempsResults[value1.GetString()].Catenate(TempsResults[value2.GetString()]); 
				}
				else{
					value1 = value1.Catenate(value2); 
				}
			break;
			case MINUS:
				if(value2.GetType() == VSTRING || value1.GetType() == VSTRING){
				line = line - 2;
				ParseError(line, "Illegal operand type for the operation.");
				return false;
				}
			default:
			break; 
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
		
	}
	retVal = value1; //assigment done return value 
	Parser::PushBackToken(tok);
	return true;
}//End of Expr

//MultExpr ::= TermExpr { ( * | / ) TermExpr }
bool MultExpr(istream& in, int& line, Value & retVal) {
	bool t1 = TermExpr(in, line, retVal); 
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}
	Value value1 = retVal;
	
	tok	= Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
	}
	while ( tok == MULT || tok == DIV  )
	{
		t1 = TermExpr(in, line, retVal);
		
		if( !t1 ) {
			ParseError(line, "Missing operand after operator");
			return false;
		}
		Value value2 = retVal;
		//check map for values
		bool val1 = false, val2 = false;  
		
		if(value1.GetType() == VSTRING){
			 val1 = (TempsResults.find(value1.GetString()) != TempsResults.end()) ? true : false;
		}else if(value2.GetType() == VSTRING){
			 val2 = (TempsResults.find(value2.GetString()) != TempsResults.end()) ? true : false;
		}
		/*
		if(!val1 || !val2){
			if(defVar[value1.GetString()] == true || defVar[value2.GetString()] == true){
				ParseError(line, "Using uninitialized Variable");
				return false; 
			}
		}*/
		// do operations accordinglly
		try{
			Value check; 
		switch(tok.GetToken()){ //keep ooperating on val one till end of loop 
			case MULT:
				if(val1 && !val2){ 
					value1 = TempsResults[value1.GetString()] * value2;
				}else if(val2 && !val1){
					value1 = TempsResults[value2.GetString()] * value1; 
				}else if(val1 && val2){
					value1 = TempsResults[value2.GetString()] * TempsResults[value1.GetString()];
				}else{
					value1 = value1 * value2;
				}
			break;
			case DIV:
				 check = TempsResults[value2.GetString()];
				if(check.IsReal()){
					if(check.GetReal() < 1){
					ParseError(--line, "Run-Time Error-Illegal division by Zero");
					return false; 
				}
				}
				value1 = TempsResults[value1.GetString()] / TempsResults[value2.GetString()]; 
			default:
			break; 
		}
		}catch(const char* e){
			cerr<<"What? "<< e <<" "; 
			if(retVal.GetType() == VINT){
				cout<<retVal.GetInt()<<endl;
			}else if(retVal.GetType() == VREAL){
				cout<<retVal.GetReal()<<endl;
			}else if(retVal.GetType() == VSTRING){
				cout<<retVal.GetString()<<endl;
			}
		}
		tok	= Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
		
	}
	retVal = value1; //assign finished loop to refrance variable  
	Parser::PushBackToken(tok);
	return true;
}//End of MultExpr

//TermExpr ::= SFactor { ** SFactor }
bool TermExpr(istream& in, int& line, Value & retVal) {

	Value value1;
	Value value2;
	
	bool t1 = SFactor(in, line, value1);
	LexItem tok; 
	
	if( !t1 ) {
		return false;
	}
	retVal = value1;
	tok	= Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
	}
	while ( tok == POW  )
	{
		t1 = SFactor(in, line, value2);
		
		
		if( !t1 ) {
			ParseError(line, "Missing exponent operand");
			return false;
		}
		tok	= Parser::GetNextToken(in, line);
		if(tok == POW){
			TermExpr(in, line, value2);
		}else{
			Parser::PushBackToken(tok);
			 value2 = retVal;
		}
		 
		//check if map and preform power operand
		bool val1 = false, val2 = false;  
		
		if(value1.GetType() == VSTRING){
			 val1 = (TempsResults.find(value1.GetString()) != TempsResults.end()) ? true : false;
		}
		if(value2.GetType() == VSTRING){
			 val2 = (TempsResults.find(value2.GetString()) != TempsResults.end()) ? true : false;
		}
		switch(tok.GetToken()){ //keep ooperating on val one till end of loop 
			case POW:
				if(val1 && !val2){
					value1 = TempsResults[value1.GetString()].Power(value2);
				}else if(val2 && !val1){
					value1 = value1.Power(TempsResults[value2.GetString()]); 
				}else if(val1 && val2){
					value1 = TempsResults[value1.GetString()].Power(TempsResults[value2.GetString()]);
				}
				else{
					value1 = value1.Power(value2); 
				}
			break;
			default:
			break; 
		}
		tok	= Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
		
	}
	retVal = value1;
	Parser::PushBackToken(tok);
	return true;
}//End of TermExpr

//SFactor = Sign Factor | Factor
bool SFactor(istream& in, int& line, Value & retVal)
{
	LexItem t = Parser::GetNextToken(in, line);
	
	bool status;
	int sign = 0;
	if(t == MINUS )
	{
		sign = -1;
	}
	else if(t == PLUS)
	{
		sign = 1;
	}
	else
		Parser::PushBackToken(t);
		
	status = Factor(in, line, sign, retVal);
	if (sign == -1){
		try{
		if(TempsResults[retVal.GetString()].GetType() == VSTRING){
			ParseError(line, "Illegal operand type for the operation.");
			return false;
		}else{
			retVal = TempsResults[retVal.GetString()].GetInt()*-1;
		}}catch(char const* e){cerr<<e<<endl;}
	}

	return status;
}//End of SFactor

//Factor := ident | iconst | rconst | sconst | (Expr)
bool Factor(istream& in, int& line, int sign, Value & retVal) {
	
	LexItem tok = Parser::GetNextToken(in, line);
	if( tok == IDENT ) {
		string lexeme = tok.GetLexeme();
		if (!(defVar.find(lexeme)->second))
		{
			ParseError(line, "Using Undefined Variable");
			return false;	
		}
		retVal = Value(lexeme); 
		return true;
	}
	else if( tok == ICONST ) {
		retVal = Value(stoi(tok.GetLexeme()));
		return true;
	}
	else if( tok == SCONST ) { 
		retVal = Value(tok.GetLexeme());
		return true;
	}
	else if( tok == RCONST ) {
		retVal = Value(stod(tok.GetLexeme())); 
		return true;
	}
	else if( tok == LPAREN ) {
		bool ex = Expr(in, line, retVal);
		if( !ex ) {
			ParseError(line, "Missing expression after (");
			return false;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;
		else 
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing ) after expression");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	
	return false;
}



