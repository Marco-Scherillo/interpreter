// Marco Scherillo Section CS 280 Section 008
//tester file 
// Lexical Anylizer PA1 2024 see README.txt for more info. 
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <string>
#include <algorithm>
#include <cstring>
#include "lex.h"
#include <string.h>
using namespace std;

void RemoveDuplicates(vector<LexItem> &tokens){
    for (int i = 0; i < tokens.size() - 1; i++){
        for (int j = i + 1; j < tokens.size(); j++){
            if (tokens.at(i).GetLexeme() == tokens.at(j).GetLexeme()){
                tokens.erase(tokens.begin() + j);
                i--;
                break;
            }
        }
    }
}

void OrganizeTokens(vector<LexItem> &tokens){
    if(tokens.empty()){
        return; 
    }
    RemoveDuplicates(tokens);
    for (int i = 0; i < tokens.size(); i++) {
        for (int j = i; j < tokens.size(); j++) {
            LexItem remove = tokens.at(j);
            bool erase = false;
            if (tokens.at(i).GetToken() == ICONST) {
                int alpha = stoi(tokens.at(j).GetLexeme());
                int beta = stoi(tokens.at(i).GetLexeme());

                if (alpha < beta)
                    erase = true;
            }
            else if (tokens.at(i).GetToken() == RCONST) {
                int alpha = stof(tokens.at(j).GetLexeme());
                int beta = stof(tokens.at(i).GetLexeme());

                if (alpha < beta)
                    erase = true;
            }
            else if (tokens.at(j).GetLexeme() < tokens.at(i).GetLexeme()) 
                erase = true;
            
            if (erase) {
                tokens.erase(tokens.begin() + j);
                tokens.insert(tokens.begin() + i, remove);
            }
        }
    }
}
int main(int argc, char* argv[]){
    // Check if a file name is provided as a command line argument
    if (argc < 2) {
        cout << "NO SPECIFIED INPUT FILE." <<endl;
        return EXIT_FAILURE;
    }

    // Extract the file name from the command line argument
    const char* fileName = argv[1];

     

    // Open the file
    ifstream file(fileName);

    // Check if the file can be opened
    if (!file.is_open()) {
        cout << "CANNOT OPEN THE FILE " << fileName <<endl;
        return EXIT_FAILURE;
    }

    //check if file is empty 
    if(file.peek() == EOF){
        cout<<"Empty File."<<endl;
        return EXIT_FAILURE; 
    }
    

    bool all = false, intigers = false, real = false, str = false, id = false, kw = false;
    //flags 
    for(int i=2;i<argc;i++){
        char* command = argv[i]; 
        if(command[0]!='-'){
            cout<<"ONLY ONE FILE NAME IS ALLOWED."<<endl;
            return EXIT_FAILURE; 
        }
        if(strcmp(command, "-all") == 0){
            all = true; 
        }else  if(strcmp(command, "-int") == 0){
            intigers = true; 
        }else  if(strcmp(command, "-real") == 0){
            real = true; 
        }else if(strcmp(command, "-str") == 0){
            str = true;
        }else if(strcmp(command, "-id") == 0){
            id = true; 
        }else if(strcmp(command, "-kw") == 0){
            kw = true; 
        }else{
            cout<<"UNRECOGNIZED FLAG {" <<argv[i]<<"}"<<endl;
            exit(1);
        }

    }
    //declare eveything needed 
    int lineNumber = 0;
    LexItem token;
    vector<LexItem> tokens;
    vector<LexItem> intigerTokens;
    vector<LexItem> identifiers;
    vector<LexItem> reals;
    vector<LexItem> strings;
    vector<LexItem> kwords; 
    map<string, int> freq;
    map<string, int> kwfreq; 


         
        while(true){ // loop collecting tokens 
            token = getNextToken(file,lineNumber);
            if(token==DONE){break;}
            if(token == ERR){
                cout<<"Error in line "<<lineNumber+1<<": Unrecognized Lexeme {"<<token.GetLexeme()<<"}"<<endl;
                return EXIT_FAILURE;
            }
            tokens.push_back(token); // add all LexItem objects to a vector for later use 
            Token tType = token.GetToken();

            //kw flag used for the kw vector 
            bool keyword = (tType == PRINT) || (tType == PROGRAM) ||
                    (tType == REAL) || (tType == THEN) ||
                    (tType == CHARACTER) || (tType == INTEGER) || 
                    (tType == IF) || (tType == END) || (tType == THEN);

            
            if(all){cout<<token<<endl;} // if flag -all is requested print all tokens 
            // depending on its token tipe add each object to its desegnated vector 
            if(token == ICONST){
                intigerTokens.push_back(token);
            }else if(token == RCONST){
                reals.push_back(token);
            }else if(token == IDENT){
                identifiers.push_back(token);
            }else if(token == SCONST){
                strings.push_back(token); 
            }else if(keyword){
                kwords.push_back(token); 
            }
        }
        // use maps to count with out repetition 
        for (const auto& identifier : identifiers) {
            freq[identifier.GetLexeme()]++;}
        for (const auto& kword : kwords) {
            kwfreq[kword.GetLexeme()]++;}

        // organize everyting 
        OrganizeTokens(identifiers);
        OrganizeTokens(kwords);
        OrganizeTokens(intigerTokens);
        OrganizeTokens(reals);
        OrganizeTokens(strings);
        

        if(token!=ERR){ //print all the data colected 
            cout<<"\nLines: "<<lineNumber<<endl;
            cout<<"Total Tokens: "<<tokens.size()<<endl;
            cout<<"Identifiers: "<<identifiers.size()<<endl;
            cout<<"Integers: "<< intigerTokens.size()<<endl;
            cout<<"Reals: "<<reals.size()<<endl;
            cout<<"Strings: "<<strings.size();
        }
        //Flag inplimentations 
        if(id){
            cout<<"\nIDENTIFIERS:"<<endl;
            for (int i = 0; i < identifiers.size(); i++) {
                if (i > 0) {cout << ", ";}
                cout << identifiers[i].GetLexeme() << " (" << freq[identifiers[i].GetLexeme()] << ")";
            }
            
        }
        if(kw){
            cout<<"\nKEYWORDS:"<<endl;
            for (int i = 0; i < kwords.size(); i++) {
                if (i > 0) {cout << ", ";}
                cout << kwords[i].GetLexeme() << " (" << kwfreq[kwords[i].GetLexeme()] << ")";
            }
        }
        if(intigers){
            cout<<"\nINTEGERS:"<<endl;
            for (int i = 0; i < intigerTokens.size(); i++){ // print with commas exept for last one 
                if (i > 0){cout << ", ";}
                cout << intigerTokens.at(i).GetLexeme();
            }
            

        }
        if(real){
            cout<<"\nREALS:"<<endl;
            for (int i = 0; i < reals.size(); i++){ // print with commas exept for last one 
                if (i > 0){cout << ", ";}
                 string lexeme = reals.at(i).GetLexeme();
                    if (lexeme[0] == '.') {
                            cout << "0" << lexeme;
                    } else {cout << lexeme;}
            }
            
        }
        
        if(str){
             cout<<"\nSTRINGS:"<<endl;
            for (int i = 0; i < strings.size(); i++){ // print with commas exept for last one 
                if (i > 0){cout << ", ";}
                cout <<"\""<<strings.at(i).GetLexeme()<<"\"";
            }
           
        }
       cout<<endl; 

        
    

        
        
        
    
     
    

    

    file.close(); // Close the file after 
    return 0;
}




