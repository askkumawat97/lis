
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<dirent.h>	//struct dirent, opendir(), closedir()
#include<sys/ioctl.h>	//struct winsize, ioctl()

#include"lis_cmdParser.c"
#include"lis_pass1.c"
#include"lis_pass2.c"

void pass2_rec(int, int*, char*(*)[], char*, int, int);
void createTempSet(struct set*);
void createSet(char*, struct set*);

int main(int argc, char *argv[]) {
	int ind = 0;
	int errStat = 0;

	//PASS_0
	argv[0] = (char*)malloc(3+1);
	strcpy(argv[0], "lis");
	struct beta beta1[argc]; //[0] is fixed for help/version
	//initiating beta
	ind = -1;
	while(++ind < argc) {
		beta1[ind].flag = -1;
		beta1[ind].indOpt = -1;
		beta1[ind].num1 = -1;
		beta1[ind].num2 = -1;
		beta1[ind].coeff = -1;
		beta1[ind].str = NULL;
	}

	//argument parsing
	errStat = parseCommand(argc, argv, beta1);
	if(errStat == -1) {
		//printf("serious trouble with exit status: 2\n");
		exit(0);
	}

/*	//###########################################################
	//printing struct beta, filled by command parser
	ind = -1;
	printf("list of options:\n");
	while(++ind < argc) {
		if(beta1[ind].flag != -1)
			printf("%d\t", beta1[ind].flag);
		if(beta1[ind].indOpt != -1)
			printf("%d\t", beta1[ind].indOpt);
		if(beta1[ind].coeff != -1)
			printf("%lu\t", beta1[ind].coeff);
		if(beta1[ind].num1 != -1)
			printf("%d\t", beta1[ind].num1);
		if(beta1[ind].num2 != -1)
			printf("%d\t", beta1[ind].num2);
		if(beta1[ind].str != NULL)
			printf("\"%s\"", beta1[ind].str);
		printf("\n");
		if(beta1[0].flag != -1)
			break;
	}
	printf("\n");
*/	//###########################################################



	//PASS_1
	char *pattern[2][argc]; //[0][0] time-style, [0][>0] hide, [1][] ignore
	pattern[0][0] = NULL;
	pattern[0][1] = NULL;
	pattern[1][1] = NULL;

	char *pathList[argc];
	pathList[0] = (char*)malloc(3); //+1 for hidden '\'
	strcpy(pathList[0], ".");
	pathList[1] = NULL;

	int flagSet[42] = {
		0,				//help/version
		1,1,0,0,			//file type
		0,0,0,
		0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,5,0,	//file info
		1,1,1,1,			//block size 
		0,				//size flag
		0,8,				//width,tabsize
		1,0,0,				//sort
		0,0,0,1,2,
		0,0,0
	};

/*	//#####################################################################
	ind = -1;
	while(++ind < 41) printf("%4d", ind);
	printf("\n");
	ind = -1;
	while(++ind < 41) printf("%4d", flagSet[ind]);
	printf("\n");
*/	//#####################################################################

	errStat = pass1(argc, beta1, flagSet, pattern, pathList);

	//freeing struct beta melloc'd by command parser
	ind = -1;
	while(++ind < argc)
		if(beta1[ind].str != NULL)
			free(beta1[ind].str);

/*	//#####################################################################
	ind = -1;
	while(++ind < 41) printf("%4d", flagSet[ind]);
	printf("\n");
	pass1print(argc, flagSet, pattern, pathList);
*/	//#####################################################################



	//PASS_3
	//for each path given by command line arguments
	struct winsize ws;
	ioctl(0, TIOCGWINSZ, &ws);
	int winWidth = ws.ws_col;
	int recFlag = flagSet[5];
	ind = -1;
	while(pathList[++ind] != NULL) {
		pass2_rec(argc, flagSet, pattern, pathList[ind], recFlag, winWidth);
		free(pathList[ind]);
		pathList[ind] = NULL;
	}

}


void pass2_rec(int argc, int *flagSet, char *pattern[2][argc], char *path, int recFlag, int winWidth) {
	struct set *keySet = (struct set*)malloc(sizeof(struct set));
	struct set *nextSet, *tempSet;

	createSet(path, keySet); //build a link list(keySet) of file/dir of "path"
	pass2(argc, flagSet, pattern, path, keySet, winWidth);

	if(recFlag == 1) {
		nextSet = keySet;
		while(nextSet->link != NULL) {
			char *pathStr = (char*)malloc(strlen(path)+1+strlen(nextSet->link->strName)+1);
			strcpy(pathStr, path);
			strcat(pathStr, "/");
			strcat(pathStr, nextSet->link->strName);

			DIR *dirPtr = opendir(pathStr);
			if(dirPtr == NULL || !strcmp(nextSet->link->strName, ".") || !strcmp(nextSet->link->strName, "..")) {
				//freeing keySet(files) melloc'd by pass2
				tempSet = nextSet->link;
				nextSet->link = nextSet->link->link;
				free(tempSet->strName);
				tempSet->strName = NULL;
				tempSet->link = NULL;
				free(tempSet);
				tempSet = NULL;
			}
			else { //remaining are directories
				free(nextSet->link->strName);
				nextSet->link->strName = (char*)malloc(strlen(pathStr)+1);
				strcpy(nextSet->link->strName, pathStr);
				nextSet = nextSet->link;
			}
			closedir(dirPtr);
			free(pathStr);
		}
	}

	//do recurssion and freeing keySet(dir) melloc'd by pass2
	while(keySet->link != NULL) {
		if(recFlag == 1)
			pass2_rec(argc, flagSet, pattern, keySet->link->strName, recFlag, winWidth);

		//freeing keySet(files) melloc'd by pass2
		tempSet = keySet->link;
		keySet->link = keySet->link->link;
		free(tempSet->strName);
		tempSet->strName = NULL;
		tempSet->link = NULL;
		free(tempSet);
		tempSet = NULL;

	}
	free(keySet);
	keySet = NULL;
}


void createSet(char *path, struct set *keySet) {
	struct dirent *dirent1;
	DIR *dirPtr = opendir(path);

	if(dirPtr == NULL) { //lis command line argument is file
		keySet->link = (struct set*)malloc(sizeof(struct set));
		keySet = keySet->link;

		char *fileName = NULL;
		int len = strlen(path);
		int i = len-1;
		while(1) {
			if(i == 0) {
				fileName = (char*)malloc(strlen(path)+1);
				strcpy(fileName, path);
				free(path);
				path = (char*)malloc(3);
				strcpy(path, ".");
				break;
			}
			if(path[i] == '/') {
				path[i] = '\0';
				fileName = (char*)malloc(strlen(path+i+1)+1);
				strcpy(fileName, path+i+1);
				break;
			}
			i--;
		}

		initSet(keySet);
		keySet->strName = (char*)malloc(strlen(fileName)+1);
		strcpy(keySet->strName, fileName);
		free(fileName);
	}
	else {
		while(1) { //dir
			dirent1 = readdir(dirPtr);
			if(dirent1 == NULL) break;

			keySet->link = (struct set*)malloc(sizeof(struct set));
			keySet = keySet->link;
			initSet(keySet);
			keySet->strName = (char*)malloc(strlen(dirent1->d_name)+1);
			strcpy(keySet->strName, dirent1->d_name);
		}
	}
	closedir(dirPtr);
}

