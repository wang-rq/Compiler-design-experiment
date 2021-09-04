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
typedef enum { ReturnTypeK, IntTypeK, RealTypeK, Boolean } TypeKind;
typedef enum { WhileK, DoWhileK, ForK, IfK, ReturnK, AssignK, ReadK, WriteK, IntDeclareK, RealDeclareK } StmtKind;
typedef enum { OpK, ConstK, IdK, MethodCallK } ExpKind;

// parse tree
struct Node
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
	int Row;
};

extern TokenType token; //´¢´æµ±Ç°µÄtoken
extern char stringSave[MAXTOKENLEN + 1];
extern FILE* inputFile;
extern FILE* output;
extern int Row;
extern int Col;

TokenType to_Token(void);
void printToken(TokenType token, const char* str);
struct Node* NewMethodNode(MethodKind);
struct Node* NewTypeNode(TypeKind);
struct Node* NewParamNode(TypeKind);
struct Node* newStmtNode(StmtKind);
struct Node* newExpNode(ExpKind);
void printTree(struct Node*);
char* copyString(char*);
void printSpaces(void);
void menu(void);
void parError(char* errMessage); 
static struct Node* MethodDecl(void); 
struct Node* MethodDecl_Sequence(void);
static struct Node* WhileStmt(void);
static struct Node* DoWhileStmt(void);
static struct Node* ForStmt(void);
static struct Node* ReturnType(void);
static struct Node* IntType(void);
static struct Node* RealType(void);
static struct Node* Type_Sequence(void);
static struct Node* FormalParams(void);
static struct Node* Block(void);
static struct Node* Statement(void);
static struct Node* Statement_Sequence(void);
static struct Node* IntLocalVarDeclStmt(void);
static struct Node* RealLocalVarDeclStmt(void);
static struct Node* AssignStmt(void);
static struct Node* ReturnStmt(void);
static struct Node* IfStmt(void);
static struct Node* WriteStmt(void);
static struct Node* ReadStmt(void);
static struct Node* Expression(void);
static struct Node* MultiplicativeExpr(void);
static struct Node* PrimaryExpr(void);
static struct Node* BoolExpression(void);
static struct Node* ActualParams(void);

void codeGen(struct Node* syntaxTree);
void recursive_gen(struct Node* tree);