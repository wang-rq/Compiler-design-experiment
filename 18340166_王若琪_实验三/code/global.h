#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define NUM_KEY 15
#define MAXNUMCHILD 3
#define MAXTOKENLEN 256


//token type
typedef enum {
	IF, ELSE, WRITE, READ, RETURN, BEGIN, END, MAIN, INT, REAL, WHILE, DO, FOR, UPTO, DOWNTO,
	SEMI, COMMA, LEFTPAREN, RIGHTPAREN,
	ADD, SUB, MUL, DIV, MOD, ASSIGN, EQUAL, UNEQUAL, GT, LT,
	ID, NUM, QSTR,
	ERROR, ENDFILE
}TokenType;


typedef enum { MethodK, TypeK, ParamK, StmtK, ExpK } NodeKind;
typedef enum { MainK, NormalK } MethodKind;
typedef enum { FormalK, ActualK, NoneK } ParamKind;
typedef enum { ReturnTypeK, IntTypeK, RealTypeK } TypeKind;
typedef enum { WhileK, DoWhileK, ForK, IfK, ReturnK, AssignK, ReadK, WriteK, IntDeclareK, RealDeclareK } StmtKind;
typedef enum { OpK, ConstK, IdK, MethodCallK } ExpKind;

// parse tree
typedef struct 
{
	struct Node* child[MAXNUMCHILD];
	struct Node* sibling;
	NodeKind nodekind; 
	union { MethodKind method; TypeKind type; StmtKind stmt; ExpKind exp; } kind; 
	union {
		TokenType token;
		float val;
		char* name;
	} attr;
}Node;

extern TokenType token; //´¢´æµ±Ç°µÄtoken
extern char stringSave[MAXTOKENLEN + 1];
extern FILE* inputFile;
extern FILE* output;
extern int Row;
extern int Col;

TokenType to_Token(void);
void printToken(TokenType token, const char* str);
Node* NewMethodNode(MethodKind);
Node* NewTypeNode(TypeKind);
Node* NewParamNode(TypeKind);
Node* newStmtNode(StmtKind);
Node* newExpNode(ExpKind);
void printTree(Node*);
char* copyString(char*);
void printSpaces(void);
void menu(void);
void parError(char* errMessage); 
static Node* MethodDecl(void); 
Node* MethodDecl_Sequence(void);
static Node* WhileStmt(void);
static Node* DoWhileStmt(void);
static Node* ForStmt(void);
static Node* ReturnType(void);
static Node* IntType(void);
static Node* RealType(void);
static Node* Type_Sequence(void);
static Node* FormalParams(void);
static Node* Block(void);
static Node* Statement(void);
static Node* Statement_Sequence(void);
static Node* IntLocalVarDeclStmt(void);
static Node* RealLocalVarDeclStmt(void);
static Node* AssignStmt(void);
static Node* ReturnStmt(void);
static Node* IfStmt(void);
static Node* WriteStmt(void);
static Node* ReadStmt(void);
static Node* Expression(void);
static Node* MultiplicativeExpr(void);
static Node* PrimaryExpr(void);
static Node* BoolExpression(void);
static Node* ActualParams(void);