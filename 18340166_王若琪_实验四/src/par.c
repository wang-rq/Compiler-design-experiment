#include "global.h"

TokenType token;

// 拷贝字符串的简单函数
char* my_strcpy(char* str) {
	if (str == NULL) return NULL;
	int n;
	char* res;
	n = strlen(str) + 1;
	res = (char*)malloc(n);
	if (res == NULL) {
		printf("Out of memory!");
	}
	else
	{
		strcpy(res, str);
	}
	return res;
} 

// 新的 method 节点
struct Node* NewMethodNode(MethodKind kind) {
	struct Node* t = (struct Node*)malloc(sizeof(struct Node));
	if (t == NULL) {
		printf("Out of memory!");
	} 
	else {
		int i;
		for (i = 0; i < MAXNUMCHILD; i++) {
			t->nodekind = MethodK;
			t->kind.stmt = kind;
			t->child[i] = NULL;
			t->sibling = NULL;
			t->Row = Row;
		}
	}
	return t;
}

// 新的 Type 节点
struct Node* NewTypeNode(TypeKind kind) {
	struct Node* t = (struct Node*)malloc(sizeof(struct Node));
	if (t == NULL) {
		printf("Out of memory!");
	}
	else {
		int i;
		for (i = 0; i < MAXNUMCHILD; i++) {
			t->nodekind = TypeK;
			t->kind.stmt = kind;
			t->child[i] = NULL;
			t->sibling = NULL;
			t->Row = Row;
		}
	}
	return t;
}

// 新的 Param 节点
struct Node* NewParamNode(ParamKind kind) {
	struct Node* t = (struct Node*)malloc(sizeof(struct Node));
	if (t == NULL) {
		printf("Out of memory!");
	}
	else {
		int i;
		for (i = 0; i < MAXNUMCHILD; i++) {
			t->nodekind = ParamK;
			t->kind.stmt = kind;
			t->child[i] = NULL;
			t->sibling = NULL;
			t->Row = Row;
		}
	}
	return t;
}

// 新的 Stmt 节点
struct Node* newStmtNode(StmtKind kind) {
	struct Node* t = (struct Node*)malloc(sizeof(struct Node));
	if (t == NULL) {
		printf("Out of memory!");
	}
	else {
		int i;
		for (i = 0; i < MAXNUMCHILD; i++) {
			t->nodekind = StmtK;
			t->kind.stmt = kind;
			t->child[i] = NULL;
			t->sibling = NULL;
			t->Row = Row;
		}
	}
	return t;
}

// 新的 Exp 节点
struct Node* newExpNode(ExpKind kind) {
	struct Node* t = (struct Node*)malloc(sizeof(struct Node));
	if (t == NULL) {
		printf("Out of memory!");
	}
	else {
		int i;
		for (i = 0; i < MAXNUMCHILD; i++) {
			t->nodekind = ExpK;
			t->kind.stmt = kind;
			t->child[i] = NULL;
			t->sibling = NULL;
			t->Row = Row;
		}
	} 
	return t; 
}

// 语法报错
void parError(char* str)
{
	printf("\n[ERROR] Parse error in line %d, column %d: %s!\n", Row, Col, str);
	getchar();
	exit(1);
}

// 检查是否与期望的匹配
static void match(TokenType expected) {
	if (token == expected) token = to_Token();
	else {
		parError("Unexpected token");
	}
}

// 声明 Method 节点
static struct Node* MethodDecl(void) {
	struct Node* t = NULL;
	struct Node* p = ReturnType();

	if (token == MAIN) {
		t = NewMethodNode(MainK);
		match(MAIN);
	}
	else {
		t = NewMethodNode(NormalK);
	}
	t->child[0] = p;
	//func name
	if (t != NULL && token == ID) {
		t->attr.name = my_strcpy(stringSave);
	}
	match(ID);
	match(LEFTPAREN);
	if (token == RIGHTPAREN) {
		t->child[1] = NULL;
		match(RIGHTPAREN);
		t->child[2] = Block();
	}
	else {
		t->child[1] = FormalParams();
		match(RIGHTPAREN);
		t->child[2] = Block();
	}
	return t;
}

// 串行 Method 节点
struct Node* MethodDecl_Sequence(void) {
	struct Node* t = MethodDecl();
	struct Node* p = t;
	while (token != ENDFILE) {
		struct Node* q;
		q = MethodDecl();
		if (q != NULL) {
			if (t == NULL) t = p = q;
			else {
				p->sibling = q;
				p = q;
			} 
		}
	}
	return t;
}

// Return 节点
static struct Node* ReturnType(void) {
	struct Node* t = NewTypeNode(ReturnTypeK);
	if (token != INT && token != REAL) {
		parError("Invalid return type!");
	}
	t->attr.name = my_strcpy(stringSave);
	match(token);
	return t;
}

static struct Node* IntType(void) {
	struct Node* t = NewTypeNode(IntTypeK);
	match(INT);
	t->attr.name = INT;
	return t;
}

static struct Node* RealType(void) {
	struct Node* t = NewTypeNode(RealTypeK);
	match(REAL);
	return t;
}

static struct Node* Type_Sequence(void) {
	struct Node* t = NULL;
	switch (token)
	{
	case INT:
		t = NewTypeNode(IntTypeK);
		match(INT);
		break;
	case REAL:
		t = NewTypeNode(RealTypeK);
		match(REAL);
		break;
	default:
		parError("Invalid parameter type");
		break;
	}
	t->attr.name = my_strcpy(stringSave);
	match(ID);
	struct Node* p = t;
	while (token == COMMA) {
		match(COMMA);
		struct Node* q;
		q = Type_Sequence();
		if (q != NULL) {
			if (t == NULL) t = p = q;
			else {
				p->sibling = q;
				p = q;
			}
		}
	}
	return t;
}

static struct Node* FormalParams(void) {
	struct Node* t = NewParamNode(FormalK);
	t->child[0] = Type_Sequence();
	return t;
}

static struct Node* Block(void) {
	match(BEGIN);
	struct Node* t = Statement_Sequence();
	match(END);
	return t;
}

static struct Node* Statement(void) {
	struct Node* t = NULL;
	switch (token) {
	case WHILE: t = WhileStmt(); break;
	case DO: t = DoWhileStmt(); break;
	case FOR: t = ForStmt(); break;
	case BEGIN: t = Block(); break;
	case INT: t = IntLocalVarDeclStmt(); break;
	case REAL: t = RealLocalVarDeclStmt(); break;
	case ID: t = AssignStmt(); break;
	case RETURN: t = ReturnStmt(); break;
	case IF: t = IfStmt(); break;
	case WRITE: t = WriteStmt(); break;
	case READ: t = ReadStmt(); break;
	default:
		parError("Unexpected token");
		break;
	}
	return t;
}

static struct Node* Statement_Sequence(void) {
	struct Node* t = Statement();
	struct Node* p = t;
	while (token != ENDFILE && token != END) {
		struct Node* q;
		q = Statement();
		if (q != NULL) {
			if (t == NULL) t = p = q;
			else {
				p->sibling = q;
				p = q;
			}
		}
	}
	return t;
}


static struct Node* WhileStmt(void) {
	struct Node* t = newStmtNode(WhileK);
	match(WHILE);
	match(LEFTPAREN);
	if (t != NULL) t->child[0] = BoolExpression();
	match(RIGHTPAREN);
	if (t != NULL) t->child[1] = Statement();
	return t;
}

static struct Node* DoWhileStmt(void) {
	struct Node* t = newStmtNode(DoWhileK);
	match(DO);
	if (t != NULL) t->child[0] = Statement();
	match(WHILE);
	match(LEFTPAREN);
	if (t != NULL) t->child[1] = BoolExpression();
	match(RIGHTPAREN);
	return t;
}

static struct Node* ForStmt(void) {
	struct Node* t = newStmtNode(ForK);
	match(FOR);
	if (t != NULL) t->child[0] = AssignStmt();
	if (token == UPTO || token == DOWNTO) {
		match(token);
		if (t != NULL) t->child[1] = Expression();
		match(DO);
		if (t != NULL) t->child[2] = Statement();
	}
	else {
		parError("Invalid writePath");
	}
	return t;
}

static struct Node* IntLocalVarDeclStmt(void) {
	struct Node* t = newStmtNode(IntDeclareK);
	match(INT);
	if (t != NULL) {
		t->attr.name = my_strcpy(stringSave);
		match(ID);
	}
	match(SEMI);
	return t;
}

static struct Node* RealLocalVarDeclStmt(void) {
	struct Node* t = newStmtNode(RealDeclareK);
	match(REAL);
	if (t != NULL) {
		t->attr.name = my_strcpy(stringSave);
		match(ID);
	}
	match(SEMI);
	return t;
}

static struct Node* AssignStmt(void) {
	struct Node* t = newStmtNode(AssignK);
	if (t != NULL && token == ID) {
		t->attr.name = my_strcpy(stringSave);
	}
	match(ID);
	match(ASSIGN);
	if (t != NULL) t->child[0] = Expression();
	match(SEMI);
	return t;
}

static struct Node* ReturnStmt(void) {
	struct Node* t = newStmtNode(ReturnK);
	match(RETURN);
	if (t != NULL) t->child[0] = Expression();
	match(SEMI);
	return t;
}

static struct Node* IfStmt(void) {
	struct Node* t = newStmtNode(IfK);
	match(IF);
	match(LEFTPAREN);
	if (t != NULL) t->child[0] = BoolExpression();
	match(RIGHTPAREN);
	if (t != NULL) t->child[1] = Statement();
	if (token == ELSE) {
		match(ELSE);
		if (t != NULL) t->child[2] = Statement();
	}
	return t;
}


static struct Node* WriteStmt(void) {
	struct Node* t = newStmtNode(WriteK);
	match(WRITE);
	match(LEFTPAREN);
	if (t != NULL) t->child[0] = Expression();
	match(COMMA);
	if (token != QSTR) {
		parError("Invalid writePath");
	}
	t->attr.name = my_strcpy(stringSave);
	match(QSTR);
	match(RIGHTPAREN);
	match(SEMI);
	return t;
}

static struct Node* ReadStmt(void) {
	struct Node* t = newStmtNode(ReadK);
	match(READ);
	match(LEFTPAREN);
	if (t != NULL && token == ID)
		t->child[0] = PrimaryExpr();
	match(COMMA);
	if (token != QSTR) {
		parError("Invalid writePath");
	}
	t->attr.name = my_strcpy(stringSave);
	match(QSTR);
	match(RIGHTPAREN);
	match(SEMI);
	return t;
}

// op char is root
static struct Node* Expression(void) {
	struct Node* t = MultiplicativeExpr();
	while (token == ADD || token == SUB) {
		struct Node* p = newExpNode(OpK);
		if (p != NULL) {
			p->child[0] = t;
			p->attr.token = token;
			t = p;
			match(token);
			t->child[1] = MultiplicativeExpr();
		}
	}
	return t;
}

static struct Node* MultiplicativeExpr(void) {
	struct Node* t = PrimaryExpr();
	while (token == MUL || token == DIV || token == MOD) {
		struct Node* p = newExpNode(OpK);
		if (p != NULL) {
			p->child[0] = t;
			p->attr.token = token;
			t = p;
			match(token);
			t->child[1] = PrimaryExpr();
		}
	}
	return t;
}

static struct Node* PrimaryExpr(void) {
	char* temp;
	struct Node* t = NULL;
	switch (token) {
	case NUM:
		t = newExpNode(ConstK);
		if ((t != NULL) && (token == NUM))
			t->attr.val = atof(stringSave);
		match(NUM);
		break;
	case ID:
		temp = my_strcpy(stringSave);
		match(ID);
		if (token == LEFTPAREN) {
			t = newExpNode(MethodCallK);
			t->attr.name = my_strcpy(temp);
			match(LEFTPAREN);
			t->child[0] = ActualParams();
			match(RIGHTPAREN);
		}
		else {
			t = newExpNode(IdK);
			t->attr.name = my_strcpy(temp);
		}
		break;
	case LEFTPAREN:
		match(LEFTPAREN);
		t = Expression();
		match(RIGHTPAREN);
		break;
	default:
		parError("Unexpected token");
		break;
	}
	return t;
}

static struct Node* BoolExpression(void) {
	struct Node* t = Expression();
	if (token == EQUAL || token == UNEQUAL || token == GT || token == LT) {
		struct Node* p = newExpNode(OpK);
		if (p != NULL) {
			p->child[0] = t;
			p->attr.token = token;
			t = p;
			match(token);
			t->child[1] = Expression();
		}
	}
	else {
		parError("Unexpected token");
	}
	return t;
}

static struct Node* ActualParams(void) {
	struct Node* t = NewParamNode(ActualK);
	struct Node* e = Expression();
	t->child[0] = e;
	struct Node* p = e;
	while (token == COMMA) {
		match(COMMA);
		struct Node* q;
		q = Expression();
		if (q != NULL) {
			if (e == NULL) e = p = q;
			else {
				p->sibling = q;
				p = q;
			}
		}
	}
	return t;
}