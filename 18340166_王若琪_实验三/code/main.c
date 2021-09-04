#include "global.h"

int Row = 0;
int Col = 0;
FILE* inputFile;
TokenType token;

char inputFilePath[128] = "C:/Users/wangr/Desktop/test2.tiny";
     

int main() {
	inputFile = fopen(inputFilePath, "r");
	if (inputFile == NULL) {
		printf("[ERROR] Failed to open the file!\n");
	}
	 
	Node* parseTree; 
	token = to_Token();
	parseTree = MethodDecl_Sequence();
	if (token != ENDFILE) {
		parError("Code ends error!");
	}
	printf("\nParse Tree:\n");
	printTree(parseTree);
	printf("\n");

	fclose(inputFile);

	system("pause");
	return 0;
}