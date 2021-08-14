#include<stdio.h>
#include<stdlib.h>

#include"cmdParser.c"

int main(int argc, char *argv[]) {
	int i = -1;
	argv[0] = (char*)malloc(3);
	strcpy(argv[0], "lis");

	struct beta beta1[argc]; //[0] is fixed for help/version

	//initiating beta
	while(++i < argc) {
		beta1[i].flag = -1;
		beta1[i].indOpt = -1;
		beta1[i].num1 = -1;
		beta1[i].num2 = -1;
		beta1[i].coeff = 0;
		beta1[i].str = NULL;
	}

	//argument parsing
	int errStat = parseCommand(argc, argv, beta1);
	if(errStat == -1)
		exit(0);

	//argument printing
	i = -1;
	while(++i < argc) {
		if(beta1[i].flag != -1)   printf("%d\t", beta1[i].flag);
		if(beta1[i].indOpt != -1) printf("%d\t", beta1[i].indOpt);
		if(beta1[i].coeff != 0)   printf("%lu\t", beta1[i].coeff);
		if(beta1[i].num1 != -1)   printf("%d\t", beta1[i].num1);
		if(beta1[i].num2 != -1)   printf("%d\t", beta1[i].num2);
		if(beta1[i].str != NULL)  printf("\"%s\"", beta1[i].str);
		printf("\n");
		if(beta1[0].flag != -1)
			break;
	}
}

