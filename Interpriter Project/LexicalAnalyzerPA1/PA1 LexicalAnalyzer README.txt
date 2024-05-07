In this programming assignment, you will be building a lexical analyzer for small programming language, called Simple Fortran95-Like (SFort95), and a program to test it. This assignment will be followed by two other assignments to build a parser and an interpreter to the language. Although, we are not concerned about the syntax definitions of the language in this assignment, we intend to introduce it ahead of Programming Assignment 2 in order to determine the language terminals: reserved words, constants, identifier, and operators. The syntax definitions of the SFort95 language are given below using EBNF notations. However, the details of the meanings (i.e. semantics) of the language constructs will be given later on.

 

Prog ::= PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
Decl ::= Type :: VarList
Type ::= INTEGER | REAL | CHARARACTER [
]
VarList ::= Var [= Expr] {, Var [= Expr]}
Stmt ::= AssigStmt | BlockIfStmt | PrintStmt | SimpleIfStmt
PrintStmt ::= PRINT *, ExprList
BlockIfStmt ::= IF (RelExpr) THEN {Stmt} [ELSE {Stmt}] END IF
SimpleIfStmt ::= IF (RelExpr) SimpleStmt
SimpleStmt ::= AssigStmt | PrintStmt
AssignStmt ::= Var = Expr
ExprList ::= Expr {, Expr}
RelExpr ::= Expr [ ( == | < | > ) Expr ]
Expr ::= MultExpr { ( + | - | // ) MultExpr }
MultExpr ::= TermExpr { ( * | / ) TermExpr }
TermExpr ::= SFactor { ** SFactor }
SFactor ::= [+ | -] Factor
Var = IDENT
Factor ::= IDENT | ICONST | RCONST | SCONST | (Expr)
 

Based on the language definitions, the lexical rules of the language and the assigned tokens to the terminals are specified in the PA 1 statement.

This tool needs to be loaded in a new browser window