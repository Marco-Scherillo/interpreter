/*
 * File: Analyzer.cpp
 * PA1
 * Author: Marco Scherillo 2024 NJIT
 *  Lexical Anylizer see README.txt for more info. 
 */
#include "lex.h"
#include <iostream>
#include <fstream> 
#include <string>
#include <map> 
using namespace std;

LexItem id_or_kw(const string& lexeme, int linenum){
     map<string,Token> kwmap = {
                    {"program",   PROGRAM},
                    {"PROGRAM",   PROGRAM},
                    {"end",       END},
                    {"END",       END},
                    {"THEN",        THEN},
                    {"then",        THEN},
                    {"else",      ELSE},
                    {"ELSE",      ELSE},
                    {"if",        IF},
                    {"IF",        IF},
                    {"integer",   INTEGER},
                    {"INTEGER",   INTEGER},
                    {"real",      REAL},
                    {"REAL",        REAL},
                    {"character", CHARACTER},
                    {"CHARACTER", CHARACTER},
                    {"print",     PRINT},
                    {"PRINT",     PRINT},
                    {"len",       LEN},
                    {"LEN",       LEN},
    };

    Token tt = IDENT;

    auto kIt = kwmap.find(lexeme);
    if( kIt != kwmap.end() )
        tt = kIt->second;

    return LexItem(tt, lexeme, linenum);

}

ostream& operator<<(ostream& out, const LexItem& tok){
     static map<Token, string> tokenPrint{ // map for converting tokens in to their respctive string. 
            {PROGRAM, "PROGRAM"}, {INTEGER, "INTEGER"},{REAL,"REAL"}, {END, "END"}, {IF, "IF"}, {ELSE, "ELSE"},
            {THEN, "THEN"}, {IDENT, "IDENT"}, {ICONST, "ICONST"},{RCONST, "RCONST"},{PRINT,"PRINT"},
            {SCONST, "SCONST"}, {PLUS, "PLUS"}, {MINUS, "MINUS"}, {MULT, "MULT"},{CHARACTER,"CHARACTER"},
            {DIV, "DIV"},{ASSOP, "ASSOP"}, {LPAREN, "LPAREN"}, {RPAREN, "RPAREN"},
            {COMMA, "COMMA"}, {EQ, "EQ"},{POW,"POW"},{GTHAN, "GTHAN"},
            {LTHAN, "LTHAN"},{CAT,"CAT"},{DCOLON,"DCOLON"},{DEF,"DEF"},
            {DOT, "DOT"}, {ERR, "ERR"}, {DONE, "DONE"}
    };

    Token tType = tok.GetToken();
    string *token = &tokenPrint[tType];

    bool keyword = (tType == PRINT) || (tType == PROGRAM) ||
                    (tType == REAL) || (tType == THEN) ||
                    (tType == CHARACTER) || (tType == INTEGER) || 
                    (tType == IF) || (tType == END) || (tType == THEN);

    

    if (tType == ICONST || tType == RCONST){
        cout << *token;
        cout << ": (" << tok.GetLexeme() << ")";
    }else if(tType == SCONST){
        cout << *token;
        cout << ": \"" << tok.GetLexeme()<<"\"";
    }else if(tType == IDENT){
        cout << *token;
        cout << ": '" << tok.GetLexeme()<<"'";
    }else if(keyword){
        cout << *token;
    }else{
        cout << *token;
    }
    return out;
}


LexItem getNextToken(istream& in, int& linenum) {

     enum TokState { START, INID, INSTRING, ININT, INREAL, INCOMMENT, SIGN} lexState = START;
    string lexeme; 
    char ch, nextChar;
    while(in.get(ch)){
        switch(lexState){
            case START:
                if(ch == '\n'){ // count lines 
                    linenum++;
                }
                if(isspace(ch)){//ignore whitespaces
                    continue;
                }
                if(ch == '!'){
                   lexState = INCOMMENT;
                   continue;
                }

                lexeme+=ch;

                
                if(isdigit(ch)){ // check for digits 
                    lexState = ININT;
                }
                if(isalpha(ch)){ //check for Identifiers 
                    lexState = INID;
                }
                if(ch == '"'){
                    lexState = INSTRING;
                    continue; 
                }
                if( ch == '\''){
                    lexState = INSTRING;
                    continue;
                }
                if(!isalnum(ch)){
                    lexState = SIGN;
                }
            break;
            case ININT:  // ICONST := [0-9]+
                // Checks if an alpha ch is next to an integer number
                if(isdigit(ch)){
                     lexeme+=ch;

                }else if(ch == '.'){ // if there is a . in an intiger it is actually a real 
                    lexState = INREAL;
                    in.putback(ch); 
                }else{
                    
                    lexState = START;
                    in.putback(ch);
                    return LexItem(ICONST, lexeme, linenum);
                }
                
            break;
            case INID: // IDENT := Letter ( Letter | Digit | _ )*
                if(ch == '.'){
                    lexState = SIGN;
                }
                if(isalpha(ch) || isdigit(ch) || ch == '_'){ 
                    lexeme+=ch;
                }else{
                    in.putback(ch);
                     return id_or_kw(lexeme,linenum);
                }
            break;
            case SIGN: // everything that not alpha num gets sent here 
                in.putback(ch);
                if(lexeme == ","){return LexItem(COMMA,lexeme,linenum);} 
                if(lexeme == "("){ return LexItem(LPAREN,lexeme, linenum);}
                if(lexeme == ")"){return LexItem(RPAREN,lexeme,linenum);}
                if(lexeme == "-"){return LexItem(MINUS,lexeme,linenum);}
                if(lexeme == "+"){return LexItem(PLUS, lexeme,linenum);}
                if(lexeme == "<"){return LexItem(LTHAN,lexeme,linenum);}
                if(lexeme == ">"){return LexItem(GTHAN,lexeme,linenum);}
                if(lexeme == "*"){ // check for DEF after a *
                    nextChar = in.peek();
                    if(nextChar == ','){
                        return LexItem(DEF,lexeme,linenum);
                    }else if(nextChar == '*'){
                        lexeme+=nextChar;
                        in.get(ch);
                        return LexItem(POW, lexeme, linenum);
                    }else{return LexItem(MULT,lexeme,linenum);}
                }
                if(lexeme == "/"){ // check for double 
                    nextChar = in.peek();
                    if(nextChar == '/'){
                        lexeme+=nextChar;
                        in.get(ch);
                        return LexItem(CAT,lexeme,linenum);
                    }else{return LexItem(DIV,lexeme,linenum);}
                    
                    }
                if(lexeme == ""){}
                if(lexeme == "="){ // check for either = or == 
                    nextChar = in.peek();
                    if(nextChar == '='){
                        lexeme+=nextChar;
                        in.get(ch);
                        return LexItem(EQ,lexeme,linenum);
                    }else{return LexItem(ASSOP,lexeme,linenum);}
                }
                if(lexeme == "."){
                    nextChar = in.peek();
                    if(isdigit(nextChar)){
                        lexeme+=nextChar;
                        lexState = INREAL;
                        in.get(ch);
                        continue; 
                    }else{return LexItem(DOT, lexeme, linenum);}
                    
                }
                if(lexeme == ":"){
                   nextChar = in.peek();
                   if(nextChar == ':'){
                    lexeme+=nextChar;
                    in.get(ch);
                    return LexItem(DCOLON, lexeme,linenum); 
                   }else{return LexItem(ERR,"No single collon allowed", linenum);}
                }
                //if its not in these its invalid 
                else{return LexItem(ERR, lexeme,linenum);}
                
                
            break;
            case INCOMMENT:
               if (ch == '\n') {
                    linenum++;
                    lexState = START;
                }
            continue;
            case INREAL:
                lexeme+=ch;
                in.get(ch);
                while(isdigit(ch)){//pull in digits
                
                    lexeme += ch;
                    in.get(ch);
                }
                if(ch == '.'){//if you see . ==> ERR
            
                    lexeme += ch;

                    while(isdigit(ch)){ //pull in digits
                
                    lexeme += ch;
                    in.get(ch);
                    }

                    in.putback(ch);

                    return LexItem(ERR, lexeme, linenum);
                 }else{
                if(ch == '\n')
                    linenum++;
                else
                    in.putback(ch);

                return LexItem(RCONST, lexeme, linenum);}
        

                
            break;
            case INSTRING:
                if( ch == '\n' ) {
                    return LexItem(ERR, lexeme, linenum);
                }
                lexeme += ch;
                if( ch == '\'') {
                    lexeme = lexeme.substr(0, lexeme.length());
                    if(lexeme.front() == lexeme.back()){
                        return LexItem(SCONST, lexeme.substr(1, lexeme.length() - 2), linenum);
                    }else{
                        return LexItem(ERR, lexeme, linenum);
                    }
                    
                }
                if( ch == '"') {
                    lexeme = lexeme.substr(0, lexeme.length());
                    if(lexeme.front() == lexeme.back()){
                        return LexItem(SCONST, lexeme.substr(1, lexeme.length() - 2), linenum);
                    }else{
                        return LexItem(ERR, lexeme, linenum);
                    }
                    
                }


                    
                
            break;

                
            

                
                

        }//original switch end 
    }//while loop end 
    if(in.eof()){return LexItem(DONE," ", linenum);}

    //worst comes to worse just end it 
    return LexItem(ERR,"Extited loop without reaching end of file ?? ", linenum);
}