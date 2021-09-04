#include "global.h"

int Row = 0;
int Col = 0;
FILE* inputFile;
TokenType token;

char inputFilePath[128] = "test2.tiny";
     

int main() {
	inputFile = fopen(inputFilePath, "r");
	if (inputFile == NULL) {
		printf("[ERROR] Failed to open the file!\n");
	} 
	 
	struct Node* parseTree; 
	token = to_Token(); // get tokens
	parseTree = MethodDecl_Sequence(); // 构造 parse tree
	if (token != ENDFILE) {
		parError("Code ends error!");
	}
	printf("\nParse Tree:\n");
	printTree(parseTree);
	printf("\n");

	printf("****************************\n");
	printf("Choose the next work:\n");
	printf("1 ----- Analyse(check error)\n");
	printf("2 ------ Mid code generating\n");
	printf("****************************\n");
	printf("Enter your choice: ");
	int c;
	scanf("%d", &c);
	printf("\n\n");
	
	if (c == 1) {
		printf("Check errors:\n");
		printf("------------------------------------------------------\n");
		buildSymtab(parseTree->child[2]); // 语义分析（构造符号表）
		typecheck(parseTree->child[2]); // 语义分析（检查节点信息）
		printf("------------------------------------------------------\n");
		printSymTab();
	}
	else if (c == 2) {
		buildSymtab(parseTree->child[2]); // 语义分析（构造符号表）
		printSymTab();
		codeGen(parseTree->child[2]); // 生成中间代码
	}
	else {
		printf("Wrong input.\n");
		return 1;
	}
	fclose(inputFile);

	system("pause");
	return 0;
}