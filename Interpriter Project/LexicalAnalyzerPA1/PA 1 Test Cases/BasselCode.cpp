#include <cctype>
#include <map>

using std::map;
using namespace std;

#include "lex.h"

LexItem id_or_kw(const string& lexeme, int linenum)
{
    map<string,Token> kwmap = {
            { "AND", AND},
            { "BEGIN", BEGIN},
            { "BOOLEAN", BOOLEAN },
            { "DIV", IDIV },
            { "END", END },
            { "ELSE", ELSE },
            { "FALSE", FALSE },
            { "IF", IF },
            { "INTEGER", INTEGER },
            { "MOD", MOD },
            { "NOT", NOT},
            { "OR", OR },
            { "PROGRAM", PROGRAM },
            { "REAL", REAL },
            { "STRING", STRING },
            { "WRITE", WRITE },
            { "WRITELN", WRITELN },
            { "VAR", VAR },
    };
    Token tt = IDENT;

    auto kIt = kwmap.find(lexeme);
    if( kIt != kwmap.end() )
        tt = kIt->second;

    return LexItem(tt, lexeme, linenum);
}

ostream& operator<<(ostream& out, const LexItem& tok) {
    map<Token, string> tokenPrint;
    tokenPrint[PROGRAM] = "PROGRAM";
    tokenPrint[WRITELN] = "WRITELN";
    tokenPrint[WRITE] = "WRITE";
    tokenPrint[IF] = "IF";
    tokenPrint[ELSE] = "ELSE";
    tokenPrint[IDENT] = "IDENT";
    tokenPrint[VAR] = "VAR";
    tokenPrint[BEGIN] = "BEGIN";
    tokenPrint[END] = "END";
    tokenPrint[INTEGER] = "INTEGER";
    tokenPrint[REAL] = "REAL";
    tokenPrint[STRING] = "STRING";
    tokenPrint[BOOLEAN] = "BOOLEAN";
    tokenPrint[TRUE] = "TRUE";
    tokenPrint[FALSE] = "FALSE";
    tokenPrint[ICONST] = "ICONST";
    tokenPrint[RCONST] = "RCONST";
    tokenPrint[SCONST] = "SCONST";
    tokenPrint[BCONST] = "BCONST";
    tokenPrint[PLUS] = "PLUS";
    tokenPrint[MINUS] = "MINUS";
    tokenPrint[MULT] = "MULT";
    tokenPrint[DIV] = "DIV";
    tokenPrint[IDIV] = "IDIV";

    // map<Token,string> tokenPrint = {
    // 	{"PROGRAM", PROGRAM},
    // 	{"WRITELN", WRITELN},
    // 	{"WRITE", WRITE },
    // 	{ "IF", IF },
    // 	{ "ELSE", ELSE },
    // 	{ "IDENT", IDENT },
    // 	{"VAR", VAR},
    // 	{"BEGIN", BEGIN},
    // 	{"END", END},
    // 	{"INTEGER", INTEGER},
    // 	{"REAL", REAL},
    // 	{"STRING", STRING},
    // 	{"BOOLEAN", BOOLEAN},
    // 	{"TRUE", TRUE},
    // 	{"FALSE", FALSE},

    // 	{ "ICONST", ICONST },
    // 	{ "RCONST", RCONST },
    // 	{ "SCONST", SCONST  },
    // 	{ "BCONST", BCONST },

    // 	{ "PLUS", PLUS },
    // 	{ "MINUS" , MINUS },
    // 	{ "MULT" , MULT  },
    // 	{ "DIV" , DIV },
    // 	{ "IDIV" , IDIV },
    // 	{ "ASSOP", ASSOP },
    // 	{ "EQ", EQ  },
    // 	{ "GTHAN" , GTHAN  },
    // 	{ "LTHAN", LTHAN },
    // 	{ "MOD", MOD},
    // 	{ "AND", AND},
    // 	{ "OR", OR},
    // 	{ "NOT", NOT},

    // 	{ "COMMA", COMMA  },
    // 	{ "LPAREN", LPAREN },
    // 	{ "RPAREN", RPAREN },

    // 	{ "SEMICOL", SEMICOL  },
    // 	{ "DOT", DOT },

    // 	{ "ERR",ERR  },

    // 	{ "DONE", DONE },
    // };
    tokenPrint[ASSOP] = "ASSOP";
    tokenPrint[EQ] = "EQ";
    tokenPrint[GTHAN] = "GTHAN";
    tokenPrint[LTHAN] = "LTHAN";
    tokenPrint[MOD] = "MOD";
    tokenPrint[AND] = "AND";
    tokenPrint[OR] = "OR";
    tokenPrint[NOT] = "NOT";
    tokenPrint[COMMA] = "COMMA";
    tokenPrint[LPAREN] = "LPAREN";
    tokenPrint[RPAREN] = "RPAREN";
    tokenPrint[SEMICOL] = "SEMICOL";
    tokenPrint[DOT] = "DOT";
    tokenPrint[ERR] = "ERR";
    tokenPrint[DONE] = "DONE";

    Token tt = tok.GetToken();
    out << tokenPrint[ tt ];
    if( tt == IDENT || tt == ICONST || tt == SCONST || tt == RCONST || tt == ERR ) {
        out << "(" << tok.GetLexeme() << ")";
    }
    return out;
}

LexItem getNextToken(istream& in, int& linenum)
{
    enum TokState { START, INID, INSTRING, ININT, INFLOAT, INCOMMENT } lexstate = START;
    string lexeme;
    char ch, nextch, nextchar;
    Token tt;
    bool decimal = false;

    //cout << "in getNestToken" << endl;
    while(in.get(ch)) {
        //cout << "in while " << ch << endl;
        switch( lexstate ) {
            case START:
                if( ch == '\n' )
                    linenum++;

                if( isspace(ch) )
                    continue;

                lexeme = ch;

                if( isalpha(ch) || ch == '_') {
                    if(isalpha(ch)) lexeme = toupper(ch);
                    lexstate = INID;
                }

                else if( ch == '\'' ) {
                    lexstate = INSTRING;

                }

                else if( isdigit(ch) ) {
                    lexstate = ININT;
                }
                else if( ch == '{' ) {
                    lexeme += ch;
                    lexstate = INCOMMENT;
                    in.get(ch);
                }

                else {
                    tt = ERR;
                    switch( ch ) {
                        case '+':
                            tt = PLUS;
                            break;

                        case '-':
                            tt = MINUS;
                            break;

                        case '*':
                            tt = MULT;
                            break;

                        case '/':
                            tt = DIV;
                            break;

                        case ':':
                            nextchar = in.peek();
                            if(nextchar == '='){
                                in.get(ch);
                                tt = ASSOP;
                                break;
                            }
                            tt = COLON;
                            break;

                        case '=':
                            tt = EQ;
                            break;

                        case '(':
                            tt = LPAREN;
                            break;
                        case ')':
                            tt = RPAREN;
                            break;

                        case ';':
                            tt = SEMICOL;
                            break;
                        case ',':
                            tt = COMMA;
                            break;
                        case '>':
                            tt = GTHAN;
                            break;

                        case '<':
                            tt = LTHAN;
                            break;

                        case '.':
                            nextch = in.peek();
                            if(isdigit(nextch)){
                                lexstate = INFLOAT;
                                decimal = true;
                                continue;
                            }
                            else {
                                lexeme += nextch;
                                return LexItem(ERR, lexeme, linenum);
                                //cout << "Here what?" << endl;
                            }

                    }

                    return LexItem(tt, lexeme, linenum);
                }
                break;

            case INID:
                if( isalpha(ch) || isdigit(ch) || ch == '_' ) {
                    if(isalpha(ch))
                        ch = toupper(ch);
                    //cout << "inid " << ch << endl;
                    lexeme += ch;
                }
                else {
                    in.putback(ch);
                    //cout<< lexeme<<endl;
                    return id_or_kw(lexeme, linenum);

                }

                break;

            case INSTRING:

                if( ch == '\n' ) {
                    return LexItem(ERR, lexeme, linenum);
                }
                lexeme += ch;
                if( ch == '\'' ) {
                    lexeme = lexeme.substr(1, lexeme.length()-2);
                    return LexItem(SCONST, lexeme, linenum);
                }
                break;

            case ININT:
                if( isdigit(ch) ) {
                    lexeme += ch;
                }
                else if(ch == '.') {
                    lexstate = INFLOAT;
                    in.putback(ch);
                }
                else {
                    in.putback(ch);
                    return LexItem(ICONST, lexeme, linenum);
                }
                break;

            case INFLOAT:

                if( ch == '.' && isdigit(in.peek()) && !decimal) {
                    lexeme += ch; decimal = true;

                }
                else if(ch == '.' && !isdigit(in.peek()) && !decimal){
                    lexeme += ch;
                    return LexItem(ERR, lexeme, linenum);
                }
                else if(isdigit(ch) && decimal){
                    lexeme += ch;
                }

                else if(ch == '.' && decimal){
                    lexeme += ch;
                    return LexItem(ERR, lexeme, linenum);
                }
                else {
                    in.putback(ch);
                    return LexItem(RCONST, lexeme, linenum);
                }

                break;


            case INCOMMENT:
                if(ch == '\n')
                    linenum++;
                else if( ch == '}' ) {

                    in.get(ch);
                    lexstate = START;

                }
                break;
        }

    }//end of while loop

    // if(lexstate == INCOMMENT)
    // {
    // 	cout << endl;
    // 	cout << "Missing a comment end delimiters '*)' at line " << linenum << endl;
    // 	//return LexItem(ERR, lexeme, linenum);
    // }
    if( in.eof() )
        return LexItem(DONE, "", linenum);
    return LexItem(ERR, "some strange I/O error", linenum);
}






