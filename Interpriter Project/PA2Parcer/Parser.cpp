/*Parser 
Marco Scherillo
NJIT 2024
*/

#include "parser.h"
#include <vector>

map<string, bool> defVar;
map<string, Token> SymTable;
string* ptr = nullptr;
int nesting = 0;
vector<string> declaredChar;
bool indeclaration = false;

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

//Prog ::= PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
 bool Prog(istream& in, int& line){
		LexItem t = Parser::GetNextToken(in,line);
		
		if(t==PROGRAM){
			t = Parser::GetNextToken(in,line);
			if(t!=IDENT){
				ParseError(line,"keyword must be followed by ident");
				return false;
			}else{
				while(true){
					t = Parser::GetNextToken(in,line);
					if(t==END){
						t = Parser::GetNextToken(in,line);
						if(t!=PROGRAM){
							ParseError(line,"END must be followed by PROGRAM key word");
							return false;
						}else{
							t = Parser::GetNextToken(in,line);
							if(t!=IDENT){
								ParseError(line,"Must end with ident");
								return false; 
							}else{
								continue;
							}
						}
					}

					if(t==DONE && ErrCount()>0){
						cout<<"(DONE)"<<endl;
						return false;}

					if(t==DONE&& ErrCount()==0){
						if(declaredChar.size()>0){
							for (int i = 0; i < declaredChar.size(); ++i) {
							cout<<"Definition of Strings with length of "<<declaredChar[i]<<" in declaration statement."<<endl;
						}
						}
						while(nesting>0){
							cout << "End of Block If statement at nesting level " << nesting << endl;
                    		nesting--;
						}
						cout<<"(DONE)"<<endl;
						return true;
					}
					if(t==CHARACTER || t==REAL || t==INTEGER){ //declaration
						Parser::PushBackToken(t);
						 Decl(in,line);
					}else{//Stament
						Parser::PushBackToken(t);
						 Stmt(in,line);
					}
				}

			}

			
		}else{ParseError(line, "Must start with program key word");}
	
	ParseError(line,"prog function error");
	return false;  
 }//end of Prog

//Stmt ::= AssigStmt | BlockIfStmt | PrintStmt | SimpleIfStmt
 bool Stmt(istream& in, int& line){
	LexItem t = Parser::GetNextToken(in,line);

	if(t == ELSE){
		 return Stmt(in,line);
	}
	if(t == IDENT){
		Parser::PushBackToken(t);
		return AssignStmt(in,line);
	}else if(t == IF){
		return SimpleIfStmt(in,line);
	}else if(t == PRINT){
		return PrintStmt(in,line);
	}else if(t == THEN){
		return BlockIfStmt(in,line);
		}else{
		ParseError(line,"Statment must be a keyword or IDENT");
		return false;}
 }

 //AssignStmt ::= Var = Expr 
bool AssignStmt(istream& in, int& line){
	bool var = Var(in,line);
	if(var){
		LexItem t = Parser::GetNextToken(in,line);
		if(t!=ASSOP){
			ParseError(line,"Must have assigment operator after variable");
			return false;
		}else{
			return Expr(in,line);
		}
	}
	ParseError(line,"Assignment error");
	return false; 
}

 //Decl ::= Type :: VarList 
 bool Decl(istream& in, int& line){
	indeclaration = true;
	bool type = Type(in,line);
	LexItem t = Parser::GetNextToken(in, line);
	if(t!=DCOLON){
		ParseError(line, "Declaration must be followed by Double collon.");
		return false; 
	}
	bool var = VarList(in,line);
	indeclaration = false; 
	return var && type;
 }
//Type ::= INTEGER | REAL | CHARARACTER [(LEN = ICONST)]
 bool Type(istream& in, int& line){
	LexItem t = Parser::GetNextToken(in, line);
	if(t==INTEGER || t==REAL || t==CHARACTER){
		t = Parser::GetNextToken(in, line);
		if(t==LPAREN){
			t = Parser::GetNextToken(in, line);
			if(t==LEN){
				t = Parser::GetNextToken(in, line);
				if(t==ASSOP){
					t = Parser::GetNextToken(in, line);
					if(t==ICONST){
						declaredChar.push_back(t.GetLexeme());
						t = Parser::GetNextToken(in, line);
						if(t==RPAREN){
							return true;
						}else{
							ParseError(line,"Expected close parethesis");
							return false;
						}
					}else{
						ParseError(line,"Must be an intiger constant");
						return false;
					}
				}else{
					ParseError(line,"LEN must be followed by assigment operator");
					return false;
				}
			}else{
				ParseError(line, "paren must be fallowed by key word LEN");
				return false;
			}
		}else{
			Parser::PushBackToken(t);
			return true;
		}
		
	}else{
		ParseError(line,"Error Unknown type");
		return false; 
	}
 }
// BlockIfStmt ::= IF (RelExpr) THEN {Stmt} [ELSE {Stmt}] END IF
bool BlockIfStmt(istream& in, int& line) {
    if (Stmt(in, line)) {
        while (true) {
           LexItem t = Parser::GetNextToken(in, line);
		   if(t==PRINT){
				Parser::PushBackToken(t);
				Stmt(in,line);
				continue;
		   }
            if (t == ELSE) {
                if (!Stmt(in, line)) {
                    ParseError(line, "Expecting statement after ELSE");
                    return false;
                }
            } else if (t == END) {
                t = Parser::GetNextToken(in, line);
                if (t == IF) {
                    return true;
                } else {
                    ParseError(line, "Block if statement must end in END IF");
                    return false;
                }
            } else {
                Parser::PushBackToken(t);
                return true;
            }
        }
    } else {
		cout<<"return false"<<endl;
        return false;
    }
}

// SimpleIfStmt ::= IF (RelExpr) SimpleStmt
bool SimpleIfStmt(istream& in, int& line) {
    LexItem t = Parser::GetNextToken(in, line);
    if (t != LPAREN) {
        ParseError(line, "IF statement must have a condition in parenthesis");
        return false;
    }
	t = Parser::GetNextToken(in, line);
    if (!RelExpr(in, line)) {
		ParseError(line,"If must be followe by a real exp");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t != RPAREN) {
        ParseError(line, "IF statement must end with a parenthesis");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t == THEN) {
        nesting++;
		Parser::PushBackToken(t);
        return Stmt(in, line);
    } else {
        Parser::PushBackToken(t);
        return SimpleStmt(in, line);
    }
}
// RelExpr ::= Expr [ ( == | < | > ) Expr ]
bool RelExpr(istream& in, int& line){
	LexItem t = Parser::GetNextToken(in,line);
	if(t==EQ || t==LTHAN || t==GTHAN){
		return Expr(in,line); 
	}else{
		ParseError(line,"Must have == , > or < after expr");
		return false; 
	}
	
}
//Expr ::= MultExpr { ( + | - | // ) MultExpr }
bool Expr(istream& in, int& line){
	if (!MultExpr(in, line)) {
        ParseError(line, "Expected a Multiplicative Expression");
        return false;
    }

    LexItem t = Parser::GetNextToken(in, line);
	if(t==RPAREN){
		Parser::PushBackToken(t);
		return true; 
	}

	if(t==COMMA){
		Parser::PushBackToken(t);
		return true; 
	}
	if(t==END || t==CHARACTER || t==PRINT || t==IF || t==ELSE){
		Parser::PushBackToken(t);
		return true;
	}
    if (t != PLUS && t != MINUS && t != CAT) {
        ParseError(line+1, "Expression must be followed by an operator");
        return false;
    }

    if (!MultExpr(in, line)) {
        ParseError(line, "Expected a Multiplicative Expression after the operator");
        return false;
    }

    return true;

}

// MultExpr ::= TermExpr { ( * | / ) TermExpr }
bool MultExpr(istream& in, int& line){
	if (!TermExpr(in, line)) {
        ParseError(line, "Expected a Term Expression");
        return false;
    }

    LexItem t = Parser::GetNextToken(in, line);

    while (t == MULT || t == DIV) {
        if (!TermExpr(in, line)) {
            ParseError(line, "Expected a Term Expression after the '*' or '/' operator");
            return false;
        }
        t = Parser::GetNextToken(in, line);
    }

    Parser::PushBackToken(t);
    return true;
}

// TermExpr ::= SFactor { ** SFactor }
bool TermExpr(istream& in, int& line){
	if (!SFactor(in, line)) {
        ParseError(line, "Expected a Simple Factor");
        return false;
    }

    LexItem t = Parser::GetNextToken(in, line);
    while (t == POW) {
        if (!SFactor(in, line)) {
            ParseError(line, "Expected a Simple Factor after the '**' operator");
            return false;
        }
        t = Parser::GetNextToken(in, line);
    }

    Parser::PushBackToken(t);
    return true;
}

//SFactor ::= [+ | -] Factor
bool SFactor(istream& in, int& line){
	LexItem t = Parser::GetNextToken(in, line);

    if (t == PLUS) {
        // Unary plus or minus found, continue parsing Factor
        if (!Factor(in, line, 1)) {
            ParseError(line, "Expected a Factor after unary operator");
            return false;
        }
    }else if(t==MINUS){
		if (!Factor(in, line, -1)) {
            ParseError(line, "Expected a Factor after unary operator");
            return false;
        }
	} else {
        // No unary operator, push back the token and parse Factor
        Parser::PushBackToken(t);
        if (!Factor(in, line, 0)) {
            ParseError(line, "Expected a Factor");
            return false;
        }
    }

    return true;
}
//Factor ::= IDENT | ICONST | RCONST | SCONST | (Expr)
bool Factor(istream& in, int& line, int sign){
	 LexItem t = Parser::GetNextToken(in, line);
    if ((sign == -1 && t == MINUS) || (sign == 1 && t == PLUS)) {
        // Unary minus or plus found, continue parsing Factor
        if (!Factor(in, line, sign)) {
            ParseError(line, "Expected a Factor after unary operator");
            return false;
        }
    } else if (t == ICONST || t == RCONST || t == SCONST) {
        // Identifiers, constants, or string constants
        return true;
    } else if(t == IDENT ){
		auto it = defVar.find(t.GetLexeme());
		if (it == defVar.end()){
			ParseError(line,"Undeclared Variable");
		}
		return true; 
	}else if (t == LPAREN) {
        // Opening parenthesis found, parse Expr
        if (!Expr(in, line)) {
            ParseError(line, "Expected an expression inside parentheses");
            return false;
        }

        // After the expression, expect a closing parenthesis
        t = Parser::GetNextToken(in, line);
        if (t != RPAREN) {
            ParseError(line, "Expected closing parenthesis");
            return false;
        }

        return true;
    } else {
        // Invalid token for Factor
        ParseError(line, "Invalid token for Factor");
        return false;
    }
	ParseError(line,"Factor error");
	return false;
}
//SimpleStmt ::= AssigStmt | PrintStmt 
bool SimpleStmt(istream& in, int& line){
	LexItem t = Parser::GetNextToken(in,line);
	if(t==PRINT){
		cout<<"Print statement in a Simple If statement."<<endl;
		return PrintStmt(in,line);
	}else{
		return AssignStmt(in,line); 
	}
}

//PrintStmt:= PRINT *, ExpreList 
bool PrintStmt(istream& in, int& line) {
	LexItem t;
	
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
	
	if( !ex ) {
		ParseError(line, "Missing expression after Print Statement");
		return false;
	}
	return ex;
}//End of PrintStmt


//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	
	status = Expr(in, line);
	
	LexItem tok = Parser::GetNextToken(in, line);

	if(!status && tok==COMMA){
		ParseError(line, "Missing Expression");
		return false;
	}
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

//VarList ::= Var [= Expr] {, Var [= Expr]}
bool VarList(istream& in, int& line){
	bool status = false;
	status = Var(in,line);
	string value = *ptr; //assign ptr  value to value variable 
	delete ptr;  // delete pointer so no memory leaks 
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	LexItem tok = Parser::GetNextToken(in, line);
	if (tok == COMMA) {
		status = VarList(in, line);
	}else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}else if(tok == ASSOP){
		cout<<"Initialization of the variable "<<value<<" in the declaration statement."<<endl;
		bool ex = Expr(in,line);
		if(ex){
			tok = Parser::GetNextToken(in, line);
			if (tok == COMMA) {
				status = VarList(in, line);
				}else{
					Parser::PushBackToken(tok);
					return false;
				}
		}else{
			return ex;
		}
	}
	else{
		Parser::PushBackToken(tok);
		return false;
	} 
	return status;
}

// Var ::= IDENT
bool Var(istream& in, int& line){
	LexItem t = Parser::GetNextToken(in,line);
	if(t==ICONST){
		ParseError(line,"Var can not start with integer");
		return Var(in,line); 
	}
	if(t!=IDENT){
		ParseError(line, "Variable Must be an Identifier");
		return false;
	}else{
		ptr = new string(t.GetLexeme());
		 // Check if the string is already in the map
    auto it = defVar.find(t.GetLexeme());

    if (it != defVar.end() && indeclaration) {
        // String is already in the map, set its value to false
        it->second = false;
		ParseError(line, "Error string re definition");
    } else {
        // String is not in the map, insert it with value true
        defVar[t.GetLexeme()] = true;
    }
		return true;}
}


