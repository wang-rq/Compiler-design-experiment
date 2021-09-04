#include "global.h"

TokenType token;

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

Node* NewMethodNode(MethodKind kind) {
	Node* t = (Node*)malloc(sizeof(Node));
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
		}
	}
	return t;
}

Node* NewTypeNode(TypeKind kind) {
	Node* t = (Node*)malloc(sizeof(Node));
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
		}
	}
	return t;
}

Node* NewParamNode(ParamKind kind) {
	Node* t = (Node*)malloc(sizeof(Node));
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
		}
	}
	return t;
}

Node* newStmtNode(StmtKind kind) {
	Node* t = (Node*)malloc(sizeof(Node));
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
		}
	}
	return t;
}

Node* newExpNode(ExpKind kind) {
	Node* t = (Node*)malloc(sizeof(Node));
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
		}
	} 
	return t; 
}

void parError(char* str)
{
	printf("\n[ERROR] Parse error in line %d, column %d: %s!\n", Row, Col, str);
	getchar();
	exit(1);
}

static void match(TokenType expected) {
	if (token == expected) token = to_Token();
	else {
		parError("Unexpected token");
	}
}


static Node* MethodDecl(void) {
	Node* t = NULL;
	Node* p = ReturnType();

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

Node* MethodDecl_Sequence(void) {
	Node* t = MethodDecl();
	Node* p = t;
	while (token != ENDFILE) {
		Node* q;
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

static Node* ReturnType(void) {
	Node* t = NewTypeNode(ReturnTypeK);
	if (token != INT && token != REAL) {
		parError("Invalid return type!");
	}
	t->attr.name = my_strcpy(stringSave);
	match(token);
	return t;
}

static Node* IntType(void) {
	Node* t = NewTypeNode(IntTypeK);
	match(INT);
	return t;
}

static Node* RealType(void) {
	Node* t = NewTypeNode(RealTypeK);
	match(REAL);
	return t;
}

static Node* Type_Sequence(void) {
	Node* t = NULL;
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
	Node* p = t;
	while (token == COMMA) {
		match(COMMA);
		Node* q;
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

static Node* FormalParams(void) {
	Node* t = NewParamNode(FormalK);
	t->child[0] = Type_Sequence();
	return t;
}

static Node* Block(void) {
	match(BEGIN);
	Node* t = Statement_Sequence();
	match(END);
	return t;
}

static Node* Statement(void) {
	Node* t = NULL;
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

static Node* Statement_Sequence(void) {
	Node* t = Statement();
	Node* p = t;
	while (token != ENDFILE && token != END) {
		Node* q;
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


static Node* WhileStmt(void) {
	Node* t = newStmtNode(WhileK);
	match(WHILE);
	match(LEFTPAREN);
	if (t != NULL) t->child[0] = BoolExpression();
	match(RIGHTPAREN);
	if (t != NULL) t->child[1] = Statement();
	return t;
}

static Node* DoWhileStmt(void) {
	Node* t = newStmtNode(DoWhileK);
	match(DO);
	if (t != NULL) t->child[0] = Statement();
	match(WHILE);
	match(LEFTPAREN);
	if (t != NULL) t->child[1] = BoolExpression();
	match(RIGHTPAREN);
	return t;
}

static Node* ForStmt(void) {
	Node* t = newStmtNode(ForK);
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

static Node* IntLocalVarDeclStmt(void) {
	Node* t = newStmtNode(IntDeclareK);
	match(INT);
	if (t != NULL) {
		t->attr.name = my_strcpy(stringSave);
		match(ID);
	}
	match(SEMI);
	return t;
}

static Node* RealLocalVarDeclStmt(void) {
	Node* t = newStmtNode(RealDeclareK);
	match(REAL);
	if (t != NULL) {
		t->attr.name = my_strcpy(stringSave);
		match(ID);
	}
	match(SEMI);
	return t;
}

static Node* AssignStmt(void) {
	Node* t = newStmtNode(AssignK);
	if (t != NULL && token == ID) {
		t->attr.name = my_strcpy(stringSave);
	}
	match(ID);
	match(ASSIGN);
	if (t != NULL) t->child[0] = Expression();
	match(SEMI);
	return t;
}

static Node* ReturnStmt(void) {
	Node* t = newStmtNode(ReturnK);
	match(RETURN);
	if (t != NULL) t->child[0] = Expression();
	match(SEMI);
	return t;
}

static Node* IfStmt(void) {
	Node* t = newStmtNode(IfK);
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


static Node* WriteStmt(void) {
	Node* t = newStmtNode(WriteK);
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

static Node* ReadStmt(void) {
	Node* t = newStmtNode(ReadK);
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
static Node* Expression(void) {
	Node* t = MultiplicativeExpr();
	while (token == ADD || token == SUB) {
		Node* p = newExpNode(OpK);
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

static Node* MultiplicativeExpr(void) {
	Node* t = PrimaryExpr();
	while (token == MUL || token == DIV || token == MOD) {
		Node* p = newExpNode(OpK);
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

static Node* PrimaryExpr(void) {
	char* temp;
	Node* t = NULL;
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

static Node* BoolExpression(void) {
	Node* t = Expression();
	if (token == EQUAL || token == UNEQUAL || token == GT || token == LT) {
		Node* p = newExpNode(OpK);
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

static Node* ActualParams(void) {
	Node* t = NewParamNode(ActualK);
	Node* e = Expression();
	t->child[0] = e;
	Node* p = e;
	while (token == COMMA) {
		match(COMMA);
		Node* q;
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