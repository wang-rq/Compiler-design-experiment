#include "global.h"

#define MAXLINE 256

char curLine[MAXLINE];
int LEN = 0; 
int IF_EOF = 0;

// DFA status
typedef enum {
	ST_START,
	ST_ASSIGN,
	ST_EQUAL,
	ST_UNEQUAL,
	ST_COMMENT,
	ST_NUMBER,
	ST_REAL,
	ST_STRING,
	ST_ID,
	ST_FINISH
}dfaStatus;

// keywords
struct {
	char* str;
	TokenType token;
} keyWords[NUM_KEY] = { {"WHILE", WHILE}, {"DO",DO}, {"FOR", FOR}, {"UPTO", UPTO}, {"DOWNTO", DOWNTO}, {"IF", IF}, {"ELSE", ELSE}, {"READ", READ}, {"WRITE", WRITE}, {"BEGIN", BEGIN}, {"END", END}, {"MAIN", MAIN}, {"RETURN", RETURN}, {"INT", INT}, {"REAL", REAL} };

char stringSave[MAXTOKENLEN + 1];

char getNextChar() {
	if (Col < LEN) {
		return curLine[Col++];
	}
	else {
		if (fgets(curLine, MAXLINE - 1, inputFile)) {
			Row++;
			LEN = strlen(curLine);
			Col = 0;
			return curLine[Col++];
		}
		else {
			IF_EOF = 1;
			return EOF;
		}
	}
}

TokenType keyWordsMap(char* str) {
	int i;
	for (i = 0; i < NUM_KEY; i++) {
		if (!strcmp(str, keyWords[i].str))
			return keyWords[i].token;
	}
	return ID;
}

void lexError()
{
	printf("\n[ERROR] Error in line %d, column %d!\n", Row, Col);
	getchar();
	exit(1);
}

TokenType to_Token(void) {
	dfaStatus curStatus = ST_START;
	int stringSaveIndex = 0;
	TokenType curToken;
	// if the cur char should be save
	int save;

	while (curStatus != ST_FINISH) {
		char c = getNextChar();
		save = 1;
		// status transfer
		switch (curStatus)
		{
		case ST_START:
			if (isalpha(c)) {
				curStatus = ST_ID;
			}
			else if (isdigit(c)) {
				curStatus = ST_NUMBER;
			}
			else if (c == '=') {
				curStatus = ST_EQUAL;
			}
			else if (c == ':') {
				curStatus = ST_ASSIGN;
			}
			else if (c == '!') {
				curStatus = ST_UNEQUAL;
			}
			else if (c == '"') {
				curStatus = ST_STRING;
				save = 0;
			}
			else if (c == ' ' || c == '\t' || c == '\n') {
				save = 0;
			}
			else if (c == '/') {
				if (getNextChar() == '*') {
					save = 0;
					curStatus = ST_COMMENT;
				}
				else {
					if (!IF_EOF) Col--;
				}
			}
			else
			{
				curStatus = ST_FINISH;
				switch (c)
				{
				case EOF:
					save = 0;
					curToken = ENDFILE;
					break;
				case '+':
					curToken = ADD;
					break;
				case '-':
					curToken = SUB;
					break;
				case '*':
					curToken = MUL;
					break;
				case '/':
					curToken = DIV;
					break;
				case '%':
					curToken = MOD;
					break;
				case '<':
					curToken = LT;
					break;
				case '>':
					curToken = GT;
					break;
				case ',':
					curToken = COMMA;
					break;
				case ';':
					curToken = SEMI;
					break;
				case '(':
					curToken = LEFTPAREN;
					break;
				case ')':
					curToken = RIGHTPAREN;
					break;
				default:
					curToken = ERROR;
					break;
				}
			}
			break;
		case ST_EQUAL:
			curStatus = ST_FINISH;
			if (c == '=') {
				curToken = EQUAL;
			}
			else {
				if (!IF_EOF) Col--;
				save = 0;
				curToken = ERROR;
			}
			break;
		case ST_UNEQUAL:
			curStatus = ST_FINISH;
			if (c == '=')
				curToken = UNEQUAL;
			else {
				if (!IF_EOF) Col--;
				save = 0;
				curToken = ERROR;
			}
			break;
		case ST_ASSIGN:
			curStatus = ST_FINISH;
			if (c == '=') {
				curToken = ASSIGN;
			}
			else {
				if (!IF_EOF) Col--;
				save = 0;
				curToken = ERROR;
			}
			break;
		case ST_COMMENT:
			save = 0;
			if (c == '/') {
				if (c == EOF)
				{
					curStatus = ST_FINISH;
					curToken = ENDFILE;
				}
				curStatus = ST_START;
			}
			break;
		case ST_ID:
			if (!isalpha(c) && !isdigit(c)) {
				// ID is finished
				if (!IF_EOF) Col--;
				save = 0;
				curStatus = ST_FINISH;
				curToken = ID;
			}
			break;
		case ST_NUMBER:
			if (!isdigit(c)) {
				if (c == '.') {
					curStatus = ST_REAL;
				}
				else if (isalpha(c)) {
					if (!IF_EOF) Col--;
					save = 0;
					curStatus = ST_FINISH;
					curToken = ERROR;
				}
				else {
					if (!IF_EOF) Col--;
					curStatus = ST_FINISH;
					curToken = NUM;
				}
			}
			break;
		case ST_REAL:
			if (!isdigit(c)) {
				// digit funished
				if (!IF_EOF) Col--;
				save = 0;
				curStatus = ST_FINISH;
				curToken = NUM;
			}
			break;
		case ST_FINISH:break;
		case ST_STRING:
			save = 1;
			if (c == '"') {
				save = 0;
				curStatus = ST_FINISH;
				curToken = QSTR;
			}
			break;
		default:
			curStatus = ST_FINISH;
			curToken = ERROR;
			break;
		}
		if (save && stringSaveIndex < MAXTOKENLEN)
			stringSave[stringSaveIndex++] = c;
    
		if (curStatus == ST_FINISH) {
			if (curToken == ERROR) {
				lexError();
			}
			stringSave[stringSaveIndex] = '\0';
			if (curToken == ID) {
				curToken = keyWordsMap(stringSave);
			}
		}
	}
	printToken(curToken, stringSave);
	return curToken;
}    