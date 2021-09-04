#include "global.h"

void printToken(TokenType token, const char* str) {
	switch (token) {
	case IF:
	case ELSE:
	case WRITE:
	case READ:
	case RETURN:
	case BEGIN:  
	case END:
	case MAIN: 
	case INT:
	case WHILE: printf("[KEYWORD, %s]\n", str); break;
	case DO: printf("[KEYWORD, %s]\n", str); break;
	case FOR: printf("[KEYWORD, %s]\n", str); break;
	case UPTO: printf("[KEYWORD, %s]\n", str); break;
	case DOWNTO: printf("[KEYWORD, %s]\n", str); break;
	case REAL: printf("[KEYWORD, %s]\n", str); break;
	case SEMI: printf("[SEP, ;]\n"); break;
	case COMMA: printf("[SEP, ,]\n"); break;
	case LEFTPAREN: printf("[SEP, (]\n"); break;
	case RIGHTPAREN: printf("[SEP, )]\n"); break;
	case ADD: printf("[OP, +]\n"); break;
	case SUB: printf("[OP, -]\n"); break;
	case MUL: printf("[OP, *]\n"); break;
	case DIV: printf("[OP, /]\n"); break;
	case MOD: printf("[OP, %]\n"); break;
	case ASSIGN: printf("[OP, :=]\n"); break;
	case EQUAL: printf("[OP, ==]\n"); break;
	case UNEQUAL: printf("[OP, !=]\n"); break;
	case GT: printf("[OP, >]\n"); break;
	case LT: printf("[OP, <]\n"); break;
	case ID: printf("[ID, %s]\n", str); break;
	case NUM: printf("[NUM, %s]\n", str); break;
	case QSTR: printf("[QString, %s]\n", str); break;
	case ENDFILE: printf("EOF\n"); break;
	default:
		printf("Unknown token: %d\n", token);
	}
}

int depth = 0;

void printTree(Node* root) {
	int i;
	depth++;
	while (root != NULL) {
		printSpaces();
		if (root->nodekind == MethodK) {
			switch (root->kind.method)
			{
			case MainK:
				printf("[Main Method: %s]\n", root->attr.name);
				break;
			case NormalK:
				printf("[Method: %s]\n", root->attr.name);
				break;
			default:
				break;
			}
		}
		else if (root->nodekind == TypeK) {
			switch (root->kind.type)
			{
			case ReturnTypeK:
				printf("[ReturnType: %s]\n", root->attr.name); break;
			case IntTypeK:
				printf("[INT: %s]\n", root->attr.name); break;
			case RealTypeK:
				printf("[REAL: %s]\n", root->attr.name); break;
			default:
				printf("Unknown StmtNode kind]\n");
				break;
			}
		}
		else if (root->nodekind == ParamK)
		{
			switch (root->kind.stmt)
			{
			case FormalK:
				printf("[FormalParam:]\n"); break;
			case ActualK:
				printf("[ActualParam:]\n"); break;
			default:
				printf("Unknown StmtNode kind\n");
				break;
			}
		}
		else if (root->nodekind == StmtK)
		{
			switch (root->kind.stmt)
			{
			case WhileK:
				printf("[While]\n"); break;
			case DoWhileK:
				printf("[DoWhile]\n"); break;
			case ForK:
				printf("[For]\n"); break;
			case IfK:
				printf("[If]\n"); break;
			case ReturnK:
				printf("[Return]\n"); break;
			case AssignK:
				printf("[Assign: %s]\n", root->attr.name); break;
			case ReadK:
				printf("[Read \"%s\"]\n", root->attr.name); break;
			case WriteK:
				printf("[Write \"%s\"]\n", root->attr.name); break;
			case IntDeclareK:
				printf("[Decl: INT %s]\n", root->attr.name); break;
			case RealDeclareK:
				printf("[Decl: REAL %s]\n", root->attr.name); break;
			default:
				printf("Unknown StmtNode kind\n");
				break;
			}
		}
		else if (root->nodekind == ExpK) {
			switch (root->kind.exp)
			{
			case OpK:
				printToken(root->attr.token, '\0');
				break;
			case ConstK:
				printf("[Const: %f]\n", root->attr.val);
				break;
			case IdK:
				printf("[Id: %s]\n", root->attr.name);
				break;
			case MethodCallK:
				printf("[function Call: %s]\n", root->attr.name);
				break;
			default:
				printf("Unknown ExpNode kind\n");
				break; 
			}
		}
		else
			printf("Unknown node kind\n");
		for (i = 0; i < MAXNUMCHILD; i++)
			printTree(root->child[i]);

		root = root->sibling;
	}
	depth--;
}



void printSpaces(void) {
	int i;
	for (i = 0; i < depth * 2; i++)
		printf("  ");
}
