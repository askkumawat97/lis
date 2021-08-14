#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

struct blkOptions {
	int shiphenFlagBuff[64];

	int lenOptAll;
	char **allOptions;   // [lenOptAll]

	int lenOpts[4];
	int *listOpts[4];
	
	int numArgsList;
	int *lenArgs;        // [numArgsList]
	char ***listArgs;    // [numArgsList]
};

int readPrerequisiteFile(char*, struct blkOptions*);
void printBlkOptions(struct blkOptions);
int readLine(int, char*);
int readNumber(int);

/*
int main() {
	struct blkOptions blkOpt;
	char *fileName = (char*)malloc(14);
	strcpy(fileName, "imFile_lis.txt");
	int errStat = readPrerequisiteFile(fileName, &blkOpt);
	if(errStat == -1)
		exit(0);
	printBlkOptions(blkOpt);
	free(fileName);
	exit(0);
}
*/

int readPrerequisiteFile(char *fileName, struct blkOptions *blkOpt) {
	char lineBuff[100];
	int lineLen;
	int num, i;

	int fdOpen = open(fileName, O_RDONLY);
	if(fdOpen == 0) {
		printf("prerequisite file '%s' is not opened\n", fileName);
		return -1;
	}

	//init shiphenFlagBuff[]
	i = -1;
	while(++i < 64)
		blkOpt->shiphenFlagBuff[i] = 0;

	//read shiphenFlagBuff[]
	i = -1;
	lineLen = readLine(fdOpen, lineBuff);
	while(++i < lineLen) {
		if(lineBuff[i] == '1')
			lineBuff[i] = 64;
		blkOpt->shiphenFlagBuff[lineBuff[i]-64] = 1;
	}

	//read lenOptAll, allOptions[]
	i = -1;
	num = readNumber(fdOpen);
	blkOpt->lenOptAll = num;
	blkOpt->allOptions = (char**)malloc(sizeof(char**)*num);
	while(++i < num) {
		lineLen = readLine(fdOpen, lineBuff);
		blkOpt->allOptions[i] = (char*)malloc(lineLen+1);
		strcpy(blkOpt->allOptions[i], lineBuff);
	}

	//read lenOpts[], listOpts[]
	i = -1;
	while(++i < 4) {
		num = readNumber(fdOpen);
		blkOpt->lenOpts[i] = num;
		blkOpt->listOpts[i] = (int*)malloc(sizeof(int*)*num);
		int j = -1;
		while(++j < num)
			blkOpt->listOpts[i][j] = readNumber(fdOpen);
	}

	// read numArgs, lenArgs[], listArgs[]
	i = -1;
	blkOpt->numArgsList = num;
	blkOpt->lenArgs = (int*)malloc(sizeof(int*)*num);
	blkOpt->listArgs = (char***)malloc(sizeof(char***)*num);
	while(++i < num) {
		int j = -1;
		int num1 = readNumber(fdOpen);
		blkOpt->lenArgs[i] = num1;
		blkOpt->listArgs[i] = (char**)malloc(sizeof(char**)*num1);
		while(++j < num1) {
			lineLen = readLine(fdOpen, lineBuff);
			blkOpt->listArgs[i][j] = (char*)malloc(sizeof(char*)*lineLen);
			strcpy(blkOpt->listArgs[i][j], lineBuff);
		}
	}

	return 0;
}

void printBlkOptions(struct blkOptions blkOpt) {
	int i, num;

	//print shiphenFlagBuff[64]
	i = -1;
	printf("shiphenFlagBuff[64] = {");
	while(++i < 64-1) {
		if(i%20 == 0) printf("\n\t");
		if(i%10 == 0) printf("  ");
		printf("%2d, ", blkOpt.shiphenFlagBuff[i]);
	}
	printf("%2d}", blkOpt.shiphenFlagBuff[i]);

	//print lenOptAll, allOptions[]
	i = -1;
	printf("\n\n");
	num = blkOpt.lenOptAll;
	printf("lenOptAll = %d\n", num);
	printf("allOptions[%d] = {", num);
	while(++i < num-1)
		printf("\"%s\", ", blkOpt.allOptions[i]);
	printf("\"%s\"}", blkOpt.allOptions[i]);

	//print lenOpts[], listOpts[]
	i = -1;
	printf("\n");
	char *optWithArg[4] = {"Word","Cols","Size","Ptrn"};
	while(++i < 4) {
		num = blkOpt.lenOpts[i];
		printf("\noptWithArg%s[%d][%d] = {", optWithArg[i], i, num);
		int j = -1;
		while(++j < num-1)
			printf("%d, ", blkOpt.listOpts[i][j]);
		printf("%d}", blkOpt.listOpts[i][j]);
	}
	
	//print numArgsList, lenArgs, listArgs[]
	i = -1;
	printf("\n\n");
	num = blkOpt.numArgsList;
	printf("numArgsList = %d\n", num);
	while(++i < num) {
		printf("argList[%d][%d] = {", i, blkOpt.lenArgs[i]);
		int j = -1;
		while(++j < blkOpt.lenArgs[i]-1)
			printf("\"%s\", ", blkOpt.listArgs[i][j]);
		printf("\"%s\"}", blkOpt.listArgs[i][j]);
		printf("\n");
	}
	printf("\n");

}

int readLine(int fdOpen, char *lineBuff) {
	size_t cin;
	char rdBuff;
	int lineLen = 0;

	while(1) {
		cin = read(fdOpen, &rdBuff, 1);
		if(cin == 0 || rdBuff == '\n') {
			lineBuff[lineLen] = '\0';
			return lineLen;
		}
		lineBuff[lineLen] = rdBuff;
		lineLen++;
	}
}

int readNumber(int fdOpen) {
	size_t cin;
	char rdBuff;
	int num = 0;

	while(1) {
		cin = read(fdOpen, &rdBuff, 1);
		if(rdBuff >= 48 && rdBuff <= 57)
			num = num*10+(rdBuff-48);
		else
			return num;

	}
}

