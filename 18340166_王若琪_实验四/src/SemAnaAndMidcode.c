#include "global.h"

#define HashSize 211  // 哈希表的大小

// 哈希函数
static int hash(char* key)
{
	int temp = 0;
	int i = 0;
	while (key[i] != '\0')
	{
		temp = ((temp << 4) + key[i]) % HashSize; // 相当于乘 2 的 4 次
		++i;
	}
	return temp;
}

// 记录每个变量出现的位置（行）
typedef struct PosListRec
{
	int Row;
	struct PosListRec* next;
} *PosList;

// 哈希表元素数据结构
typedef struct hashNode
{
	char* name; // 符号名
	int type; // 类型
	int memloc; // 变量的位置
	PosList rows; // 行号数据结构
	struct hashNode* next;
} *HT;

// 哈希表
static HT hashTable[HashSize];

int getHashIndex(char* name)
{
	return  hash(name);
}

// 符号表插入函数
void insert_table(char* name, int Row, int loc, int exp_type)
{
	int h = hash(name);
	HT li = hashTable[h];
	while ((li != NULL) && (strcmp(name, li->name) != 0))
		li = li->next;
	if (li == NULL) // 如果这个符号不在符号表内，就添加该符号
	{
		li = (HT)malloc(sizeof(struct hashNode));
		li->name = name;
		li->rows = (PosList)malloc(sizeof(struct PosListRec));
		li->rows->Row = Row;
		li->memloc = loc;
		li->type = exp_type;
		li->rows->next = NULL;
		li->next = hashTable[h];
		hashTable[h] = li;
	}
	else // 如果这个符号在符号表内，只加入行号即可
	{
		PosList t = li->rows;
		while (t->next != NULL) t = t->next;
		t->next = (PosList)malloc(sizeof(struct PosListRec));
		t->next->Row = Row;
		t->next->next = NULL;
	}
}

// 符号表查找类型，如果存在就返回位置，不存在返回 -1
int lookup_table_type(char* name)
{
	int h = hash(name);
	HT l = hashTable[h];
	while ((l != NULL) && (strcmp(name, l->name) != 0))
		l = l->next;
	if (l == NULL) return -1;
	else
		return l->type;
	return l->memloc;
}

// 通过获得要查找的符号名的hash值，从hash值得位置遍历对应的桶，返回相应的值
int lookup_table(char* name)
{
	int h = hash(name);
	HT l = hashTable[h];
	int i_temp = -1;
	while ((l != NULL) && (strcmp(name, l->name) != 0))
	{
		i_temp++;
		l = l->next;
	}
	if (l == NULL) return -1;
	else
		return 10 * h + i_temp;
}

// 输出符号表
void printSymTab()
{
	printf("\nSymbol table:\n");
	int i;
	printf("-------------  --------   ------------\n");
	printf("Variable Name  Location   Line Numbers\n");
	printf("-------------  --------   ------------\n");
	for (i = 0; i < HashSize; ++i)
	{
		if (hashTable[i] != NULL)
		{
			HT l = hashTable[i];
			while (l != NULL)
			{
				PosList t = l->rows;
				printf("%-14s ", l->name);
				printf("%-8d  ", l->memloc);

				while (t != NULL)
				{
					printf("%4d ", t->Row);
					t = t->next;
				}
				printf("\n");
				l = l->next;
			}
		}
	}
	printf("-------------  --------   ------------\n\n\n");
}


static int genLoc = 0;
static int highgenLoc = 0;


// 只有寄存器的指令 op是操作类型，*r 是目标寄存器， s是第一个源寄存器， t是第二个源寄存器
void genRO(char* op, int r, int s, int t)
{
	printf("%3d:  %5s  %d,%d,%d ", genLoc++, op, r, s, t);
	printf("\n");
	if (highgenLoc < genLoc) highgenLoc = genLoc;
}

// 从寄存器到内存的指令，r 是目标寄存器，*d 是 offset，* s 基准寄存器
void genRM(char* op, int r, int d, int s)
{
	printf("%3d:  %5s  %d,%d(%d) ", genLoc++, op, r, d, s);
	printf("\n");
	if (highgenLoc < genLoc)  highgenLoc = genLoc;
}

// 关于跳过多少代码
int genSkip(int howMany)
{
	int i = genLoc;
	genLoc += howMany;
	if (highgenLoc < genLoc)  highgenLoc = genLoc;
	return i;
} 

// 返回
void genBackup(int loc)
{
	genLoc = loc;
}

// 恢复
void genRestore(void)
{
	genLoc = highgenLoc;
}

// 程序计数器
#define  pc 12

// 内存指针
#define  mp 6

// global pointer
#define gp 5

// accumulator
#define  ac 0
#define  ac1 1

// 转化为相对
void genRM_Abs(char* op, int r, int a)
{
	printf("%3d:  %5s  %d,%d(%d) ",
		genLoc, op, r, a - (genLoc + 1), pc);
	++genLoc;
	printf("\n");
	if (highgenLoc < genLoc) highgenLoc = genLoc;
}

// 临时的 memory offset
static int tempOffset = 0;

// 生成 statement 节点的中间代码
static void genStmt(struct Node* tree)
{
	struct Node* p1, * p2, * p3;
	int savedLoc1, savedLoc2, currentLoc;
	int loc;
	switch (tree->kind.stmt) {

	case IfK:
		p1 = tree->child[0];
		p2 = tree->child[1];
		p3 = tree->child[2];
		recursive_gen(p1); // 生成子节点1部分的中间代码
		savedLoc1 = genSkip(1); // 跳过 1 行
		recursive_gen(p2); // 生成子节点2部分的中间代码
		savedLoc2 = genSkip(1); // 跳过 1 行
		currentLoc = genSkip(0); // 跳过 0 行
		genBackup(savedLoc1); // 返回到 savedLoc1
		genRM_Abs("JEQ", ac, currentLoc); // 生成中间代码
		genRestore(); // 恢复
		recursive_gen(p3); // 生成子节点3部分的中间代码
		currentLoc = genSkip(0);  // 跳过 0 行
		genBackup(savedLoc2); // 返回到 savedLoc2
		genRM_Abs("LDA", pc, currentLoc); // 生成中间代码
		genRestore(); // 恢复
		break;

	case DoWhileK:
		p1 = tree->child[0];
		p2 = tree->child[1];
		savedLoc1 = genSkip(0);
		recursive_gen(p1);
		recursive_gen(p2);
		genRM_Abs("JEQ", ac, savedLoc1);
		break; 

	case WhileK:
		p1 = tree->child[0];
		p2 = tree->child[1];
		savedLoc1 = genSkip(0);
		recursive_gen(p1);

		savedLoc2 = genSkip(1);
		recursive_gen(p2);

		genRM_Abs("LDA", pc, savedLoc1);

		currentLoc = genSkip(0);

		genBackup(savedLoc2);
		genRM_Abs("JEQ", ac, currentLoc);
		genRestore();

		break;

	case AssignK:
		recursive_gen(tree->child[0]);
		loc = lookup_table(tree->attr.name); //找到地址
		genRM("ST", ac, loc, gp);
		break;

	case ReadK:
		genRO("IN", ac, 0, 0);
		loc = lookup_table(tree->attr.name);
		genRM("ST", ac, loc, gp);
		break;
	case WriteK:
		recursive_gen(tree->child[0]);
		genRO("OUT", ac, 0, 0);
		break;

	default:
		break;
	}
}

// 生成 expression 节点的中间代码
static void genExp(struct Node* tree)
{
	int loc;
	struct Node* p1, * p2;
	switch (tree->kind.exp) {

	case ConstK:
		genRM("LDC", ac, tree->attr.val, 0);
		break;

	case IdK:
		loc = lookup_table(tree->attr.name);
		genRM("LD", ac, loc, gp);
		break; 

	case OpK:
		p1 = tree->child[0];
		p2 = tree->child[1];
		recursive_gen(p1);
		genRM("ST", ac, tempOffset--, mp);
		recursive_gen(p2);
		genRM("LD", ac1, ++tempOffset, mp);
		switch (tree->attr.token) {
		case ADD:
			genRO("ADD", ac, ac1, ac);
			break;
		case SUB:
			genRO("SUB", ac, ac1, ac);
			break;
		case MUL:
			genRO("MUL", ac, ac1, ac);
			break;
		case DIV:
			genRO("DIV", ac, ac1, ac);
			break;
		case MOD:
			genRO("MOD", ac, ac1, ac);
			break;
		case LT:
			genRO("SUB", ac, ac1, ac);
			genRM("JLT", ac, 2, pc);
			genRM("LDC", ac, 0, ac);
			genRM("LDA", pc, 1, pc);
			genRM("LDC", ac, 1, ac);
			break;
		case GT:
			genRO("SUB", ac, ac1, ac);
			genRM("JGT", ac, 2, pc);
			genRM("LDC", ac, 0, ac);
			genRM("LDA", pc, 1, pc);
			genRM("LDC", ac, 1, ac);
			break;
		case EQUAL:
			genRO("SUB", ac, ac1, ac);
			genRM("JEQ", ac, 2, pc);
			genRM("LDC", ac, 0, ac);
			genRM("LDA", pc, 1, pc);
			genRM("LDC", ac, 1, ac);
			break;
		case UNEQUAL:
			genRO("SUB", ac, ac1, ac);
			genRM("JEQ", ac, 2, pc);
			genRM("LDC", ac, 0, ac);
			genRM("LDA", pc, 1, pc);
			genRM("LDC", ac, 1, ac);
			break;

		default:
			break;
		} 
		break; 

	default:
		break;
	}
}

// 递归生成中间代码
void recursive_gen(struct Node* tree) 
{
	if (tree != NULL)
	{
		switch (tree->nodekind) {
		case StmtK:
			genStmt(tree);
			break;
		case ExpK:
			genExp(tree);
			break;
		default:
			break;
		}
		recursive_gen(tree->sibling);
	}
}

// 生成中间代码
void codeGen(struct Node* syntaxTree)
{
	// 开头
	genRM("LD", mp, 0, ac);
	genRM("ST", ac, 0, ac);
	// 生成
	recursive_gen(syntaxTree);
	// 结尾
	genRO("HALT", 0, 0, 0);
	printf("\n\n");
}

static int location = 0;

// 遍历语法树
static void traverse(struct Node* t, void(*preProc) (struct Node*), void(*postProc) (struct Node*))
{
	if (t != NULL)
	{
		preProc(t);
		{ int i;
		for (i = 0; i < MAXNUMCHILD; i++)
			traverse(t->child[i], preProc, postProc);
		}
		postProc(t);
		traverse(t->sibling, preProc, postProc);
	}
}

// 什么都不做
static void nullProc(struct Node* t)
{
	if (t == NULL) return;
	else return;
}

// 类型出错
static void typeError(struct Node* t, char* message)
{
	printf( "Type error! %s line %d\n", message, t->Row);
}

// 向符号表中插入
static void insertNode(struct Node* t)
{
	struct Node* p_temp = t;
	int lookup_temp = -1;
	switch (t->nodekind)
	{
	case StmtK:
		switch (t->kind.stmt)
		{
		case IfK:
			break;
		case DoWhileK:
			break;

		case AssignK:
			lookup_temp = lookup_table_type(t->attr.name);
			if (lookup_temp == -1) // 如果他还不在符号表内，报错
			{
				printf("Symb error! %s has not been declared! line:%d\n", p_temp->attr.name, p_temp->Row);
			}
			else  // 如果他在符号表内，正常插入
			{
				insert_table(t->attr.name, t->Row, 0, lookup_temp);
				t->kind.stmt = lookup_temp;
			}
			break;

		case IntDeclareK:
			lookup_temp = lookup_table_type(t->attr.name);

			if (lookup_temp == -1) // 如果他还不在符号表内，正常插入
			{
				insert_table(t->attr.name, t->Row, location++,t->attr.name);
				
			}
			else // 如果他在符号表内，报错
			{
				printf("Symb error! %s has already been declared! line:%d\n", p_temp->attr.name, p_temp->Row);
			}
			break;

		case RealDeclareK:
			lookup_temp = lookup_table_type(t->attr.name);

			if (lookup_temp == -1) // 如果他还不在符号表内，正常插入
			{
				insert_table(t->attr.name, t->Row, location++, t->attr.name);
			}
			else // 如果他在符号表内，报错
			{
				printf("Symb error! %s has already been declared! line:%d\n", p_temp->attr.name, p_temp->Row);
			}
			break;

		case ReadK:
			lookup_temp = lookup_table_type(t->attr.name);
			if (lookup_temp == -1)  // 如果他还不在符号表内，报错
			{
				printf("Symb error! %s has not been declared! line:%d\n", p_temp->attr.name, p_temp->Row);
			}
			else // 如果他在符号表内，正常插入
			{
				insert_table(t->attr.name, t->Row, 0, lookup_temp);
				t->kind.stmt = lookup_temp;
			}
			break;
		case WriteK:
			break;
		case WhileK:
			break;
		default:
			break;
		}
		break;

	case ExpK:
		switch (t->kind.exp)
		{
		case OpK:
			break;
		case ConstK:
			break;

		case IdK:
			lookup_temp = lookup_table_type(t->attr.name);
			if (lookup_temp == -1) // 如果他还不在符号表内，报错
			{
				printf("Symb error! %s has not been declared! line:%d,__%d\n", p_temp->attr.name, p_temp->Row, p_temp->kind.stmt);
			}
			else // 如果他在符号表内，正常插入
			{
				insert_table(t->attr.name, t->Row, 0, lookup_temp);
				t->kind.stmt = lookup_temp;
			}
			break;
		default:
			break;
		}
		break;

	
	default:
		break;
	}
}

// 建立符号表
void buildSymtab(struct Node* tree)
{
	traverse(tree, insertNode, nullProc);
}


// 检查节点
static void checkNode(struct Node* t)
{
	switch (t->nodekind)
	{
	case ExpK:
		switch (t->kind.exp)
		{
		case OpK: // 如果运算符，则检查两边类型是否相同
			if ((t->child[0]->attr.token != t->child[1]->attr.token) )
			{
				typeError(t, "Different types in this operation!");
			}
			if ((t->attr.token == EQUAL) || (t->attr.token == LT) || (t->attr.token == GT) || (t->attr.token == UNEQUAL))
				t->kind.stmt = Boolean;
			break;
		case ConstK:
			break;
		default:
			break;
		}
		break;
	case StmtK:
		switch (t->kind.stmt)
		{
		case AssignK: // 如果赋值语句，则检查两边类型是否相同
			if (t->child[0]->kind.stmt != t->kind.stmt)
				typeError(t, "Assignment is not the same type!");
			break;

		case IfK: // 如果是if，则检查是否为bool类型
			if (t->child[0]->kind.stmt != Boolean)
				typeError(t->child[0], "If is not Boolean!");
			break;

		case WhileK: // 如果是while，则检查是否为bool类型或整数类型
			if (t->child[0]->kind.stmt != Boolean && t->child[0]->kind.stmt != IntTypeK)
				typeError(t->child[0], "While is not Boolean and not Integer!");
			break;

		default:
			break;
		}
		break;
	default:
		break;
	}
}

// 语义分析主函数
void typecheck(struct Node* tree)
{
	traverse(tree, nullProc, checkNode);
}