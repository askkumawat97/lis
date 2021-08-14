
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct beta {
	int flag;
	int indOpt;
	int num1;
	int num2;
	unsigned long coeff;
	char *str;
};

int parseCommand(int, char**, struct beta*);
int isValidInteger(char*);
int isValidUnit(char*, unsigned long*, int*, int*);   //like 20K, 3M, etc
int validateDhiphenArgStr(char*, int, char**);        //argument of long option validation
int validateDhiphenOption(char*, char**, int*, int*); //long option validation

/*
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
*/


int parseCommand(int argc, char *argv[], struct beta *beta1) {
	//##############################################################################################

	//set according to sort option's ascii value
	char shiphenFlagBuff[74] = {
		1, //digit 1
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		1,1,1,1,0,1,1,1,1,0,0,1,0,1,0,0,1,1,1,1,1,0,0,1,0,1, //capital letters
		0,0,0,0,0,0,
		1,1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0, //small letters
	};

	//prefix + no arguments
	//prefix = mendetory arguments
	//prefix @ optional arguments
	int allOptionsLen = 43;
	char *allOptions[43] = {
		"+all","+almost-all","+author","=block-size","+classify","@color","+context",
		"+dereference","+dereference-command-line","+dereference-command-line-symlink-to-dir",
		"+directory","+dired","+escape","+file-type","=format","+full-time",
		"+group-directories-first","+help","=hide","+hide-control-chars","+human-readable",
		"@hyperlink","=ignore","+ignore-backups","=indicator-style","+inode","+kibibytes",
		"+literal","+no-group","+numeric-uid-gid","+quote-name","=quoting-style","+recursive",
		"+reverse","+show-control-chars","+si","+size","=sort","=tabsize","=time","=time-style",
		"+version","=width"
	};

	int lenOpts[4];
	lenOpts[0] = 2;
	lenOpts[1] = 2;
	lenOpts[2] = 1;
	lenOpts[3] = 8;

	int *listOpts[4];
	listOpts[0] = (int*)malloc(sizeof(int)*2); // =PATTERN
	listOpts[0][0] = 18;
	listOpts[0][1] = 22;
	listOpts[1] = (int*)malloc(sizeof(int)*2); // =COLS
	listOpts[1][0] = 38;
	listOpts[1][1] = 42;
	listOpts[2] = (int*)malloc(sizeof(int)*1); // =SIZE
	listOpts[2][0] = 3;
	listOpts[3] = (int*)malloc(sizeof(int)*8); // =WORD/[=WHEN]
	listOpts[3][0] = 5;
	listOpts[3][1] = 14;
	listOpts[3][2] = 21;
	listOpts[3][3] = 24;
	listOpts[3][4] = 31;
	listOpts[3][5] = 37;
	listOpts[3][6] = 39;
	listOpts[3][7] = 40;

	//the arguments of listOpts[3][i] are listArgs[i] respectively.
	//the symbol '/' in argument string is used for synonyms.
	int numArgsList = 8;
	int lenArgs[8] = {3,5,3,4,10,5,2,5};

	char *listArgs0[3] = {"always/yes/force","never/no/none","auto/tty/if-tty"};
	char *listArgs1[5] = {"verbose/long","commas","horizontal/across","vertical","single-column"};
	char *listArgs2[3] = {"always/yes/force","never/no/none","auto/tty/if-tty"};
	char *listArgs3[4] = {"none","slash","file-type","classify"};
	char *listArgs4[10] = {"literal","shell","shell-always","shell-escape","shell-escape-always",
				"c","c-maybe","escape","locale","clocale"};
	char *listArgs5[5] = {"none","time","size","extension","version"};
	char *listArgs6[2] = {"atime/access/use","ctime/status"};
	char *listArgs7[5] = {"+FORMAT (e.g., +%H:%M)","full-iso","long-iso","iso","locale"};

	char **listArgs[8];
	listArgs[0] = listArgs0;
	listArgs[1] = listArgs1;
	listArgs[2] = listArgs2;
	listArgs[3] = listArgs3;
	listArgs[4] = listArgs4;
	listArgs[5] = listArgs5;
	listArgs[6] = listArgs6;
	listArgs[7] = listArgs7;

	//##############################################################################################


	int errStat;
	int indArgv = 0;
	while(++indArgv < argc) {
		int indChar = 0;
		int lenStr = strlen(argv[indArgv]);
		
		if(argv[indArgv][0] == '-' && argv[indArgv][1] == '-') { //validation for long option
			int indBig = 0;
			int indEnd = allOptionsLen;

			errStat = validateDhiphenOption(argv[indArgv], allOptions, &indBig, &indEnd);
			//-1 error, 0 null substr, 1 substr, >1 symbol '=' occurs indChar in returned

			if(errStat == -1) {
				printf("%s: unrecognized option '%s'\n", argv[0], argv[indArgv]);
				printf("Try '%s --help' for more information.\n", argv[0]);
				return -1;
			}

			if(indBig != indEnd-1) {
				printf("%s: option '%s' is ambiguous; possibilities:", argv[0], argv[indArgv]);
				while(indBig < indEnd)
					printf(" \'--%s\'", allOptions[indBig++]+1);
				printf("\n");
				printf("Try '%s --help' for more information.\n", argv[0]);
				return -1;
			}
			else {
				indChar = errStat;
				if(allOptions[indBig][0] == '+' && argv[indArgv][indChar] == '=') {
					printf("%s: option '--%s' doesn't allow an argument\n", argv[0], allOptions[indBig]+1);
					printf("Try '%s --help' for more information.\n", argv[0]);
					return -1;
				}
				else if(allOptions[indBig][0] == '=' && argv[indArgv][indChar] != '=') {
					printf("%s: option '--%s' requires an argument\n", argv[0], allOptions[indBig]+1);
					printf("Try '%s --help' for more information.\n", argv[0]);
					return -1;
				}
				else if(allOptions[indBig][0] != '=' && argv[indArgv][indChar] != '=') { //long option without arguments
					//##################################################################################
					//printf("valid long option: '--%s' have no arguments\n", allOptions[indBig]+1);
					int ind1 = strcmp(allOptions[indBig]+1, "help");
					int ind2 = strcmp(allOptions[indBig]+1, "version");
					if(ind1 != 0) ind1 = 1;
					if(ind2 != 0) ind2 = 1;

					int ind = ind1 & ind2;
					beta1[indArgv*ind].flag = 2;
					beta1[indArgv*ind].indOpt = indBig;
					beta1[indArgv*ind].num1 = (ind1+1)*((ind+1)%2); //0-other 1-help 2-version
					if(ind == 0)
						break;
					//##################################################################################
				}
				else if(allOptions[indBig][0] != '+' && argv[indArgv][indChar] == '=') { //long option with arguments
					//store remaining string of argv in strOptArg
					int ind = 0;
					char *strOptArg = (char*)malloc(lenStr-indChar+2+1);
					while(indChar < lenStr)
						strOptArg[ind++] = argv[indArgv][++indChar];
					strOptArg[ind] = '\0';

					//find type of argument(indBlock) and in which listArgs[indOpt]
					int indBlock = -1;
					int indOpt;
					while(++indBlock < 4) {
						indOpt = -1;
						int len = lenOpts[indBlock];
						while(++indOpt < len)
							if(indBig == listOpts[indBlock][indOpt]) {
								indBlock += 4;
								break;
							}
					}
					indBlock -= 4+1;

					if(indBlock == 0) { //for =PATTERN
						//#########################################################################################
						//printf("valid long option '--%s' with PATTERN '%s'\n", allOptions[indBig]+1, strOptArg);
						int ind1 = strcmp(allOptions[indBig]+1, "hide");
						if(ind1 != 0) ind1 = 1;

						beta1[indArgv].flag = 3;
						beta1[indArgv].indOpt = indBig;
						beta1[indArgv].num1 = ind1; //0-hide, 1-ignore
						beta1[indArgv].str = (char*)malloc(strlen(strOptArg)+1);
						strcpy(beta1[indArgv].str, strOptArg);
						//#########################################################################################
					}
					else if(indBlock == 1) { //for =COLS
						errStat = isValidInteger(strOptArg);

						if(errStat == -1) {
							printf("%s: invalid %s: ‘%s’\n", argv[0], allOptions[indBig]+1, strOptArg);
							return -1;
						}
						//#########################################################################################
						//printf("valid long option '--%s' with COLS '%d'\n", allOptions[indBig]+1, errStat);
						beta1[indArgv].flag = 4;
						beta1[indArgv].indOpt = indBig;
						beta1[indArgv].num1 = errStat;
						//#########################################################################################
					}
					else if(indBlock == 2) { //for =SIZE
						char *strUnit;
						unsigned long coeff = 0;
						int base = 0;
						int exp = 0;
		
						errStat = isValidUnit(strOptArg, &coeff, &base, &exp);
						//-2 large value, -1 invalid
			
						if(errStat == -1) {
							printf("%s: invalid --%s argument '%s'\n", argv[0], allOptions[indBig]+1, strOptArg);
							return -1;
						}
						else if(errStat == -2) {
							printf("%s: --%s argument '%s' too large\n", argv[0], allOptions[indBig]+1, strOptArg);
							return -1;
						}
						//###################################################################################################
						//printf("valid long option '--%s' with SIZE '%lu*%d^%d'\n", allOptions[indBig]+1, coeff, base, exp);
						beta1[indArgv].flag = 5;
						beta1[indArgv].indOpt = indBig;
						beta1[indArgv].coeff = coeff;
						beta1[indArgv].num1 = base;
						beta1[indArgv].num2 = exp;
						//###################################################################################################
					}
					else if(indBlock == 3) { //for =WORD, [=WHEN]
						errStat = validateDhiphenArgStr(strOptArg, lenArgs[indOpt], listArgs[indOpt]);
						// -3 +FORMAT, -2 error embiguity, -1 error invalid, 0 substr

						if(errStat == -3) { //+FORMAT in time
							//#################################################################################
							beta1[indArgv].flag = 7;
							beta1[indArgv].indOpt = indBig;
							beta1[indArgv].str = (char*)malloc(strlen(strOptArg)+1);
							strcpy(beta1[indArgv].str, strOptArg+1);
							beta1[indArgv].num1 = 0;
							//#################################################################################
							continue;
						}
						else if(errStat >= 0) {
							//#####################################################################################################
							//printf("valid argument of long option '--%s' matched at index %d\n", allOptions[indBig]+1, errStat);
							beta1[indArgv].flag = 6;
							beta1[indArgv].indOpt = indBig;
							beta1[indArgv].num1 = errStat;
							//#####################################################################################################
							continue;
						}
						else if(errStat == -1)
							printf("%s: invalid argument ‘%s’ for ‘--%s’\n", argv[0], strOptArg, allOptions[indBig]+1);
						else if(errStat == -2)
							printf("%s: ambiguous argument ‘%s’ for ‘--%s’\n", argv[0], strOptArg, allOptions[indBig]+1);
							
						int ind = 0;
						printf("Valid arguments are:\n");
						while(ind < lenArgs[indOpt]) {
							int indChar = 0;
							int lenStr = strlen(listArgs[indOpt][ind]);
							printf("  - \'");
							while(indChar < lenStr) {
								if(listArgs[indOpt][ind][indChar] == '/')
									printf("\', \'");
								else
									printf("%c", listArgs[indOpt][ind][indChar]);
								indChar++;
							}
							ind++;
							printf("\'\n");
						}
						printf("Try '%s --help' for more information.\n", argv[0]);
						return -1;
					}
					free(strOptArg);
				}
			}
		}
		else if(argv[indArgv][0] == '-' && lenStr != 1) { // validation for sort option
			while(++indChar < lenStr) {
				if(shiphenFlagBuff[argv[indArgv][indChar]-49] == 0) { // take %74, core dump prevention
					printf("%s: invalid option -- '%c'\n", argv[0], argv[indArgv][indChar]);
					printf("Try '%s --help' for more information.\n", argv[0]);
					return -1;
				}

				//option with argument
				if(argv[indArgv][indChar] == 'T' || argv[indArgv][indChar] == 'w' || argv[indArgv][indChar] == 'I') { //options with arg
					if(argv[indArgv][indChar+1] == '\0') {
						printf("lis: option requires an argument -- '%c'\n", argv[indArgv][indChar]);
						printf("Try '%s --help' for more information.\n", argv[0]);
						return -1;

					}
					errStat = isValidInteger(argv[indArgv]+indChar+1);

					char *str = (char*)malloc(10);
					if(argv[indArgv][indChar] == 'T') {
						strcpy(str, "tab size");
						beta1[indArgv].num1 = 2;
					}
					else if(argv[indArgv][indChar] == 'w') {
						strcpy(str, "line width");
						beta1[indArgv].num1 = 1;
					}

					if(errStat == -1) {
						if(argv[indArgv][indChar] == 'I') {
							beta1[indArgv].num1 = 3;
							beta1[indArgv].num2 = indChar;
							free(str);
							break;
						}
						printf("%s: invalid %s: ‘%s’\n", argv[0], str, argv[indArgv]+indChar);
						return -1;
					}

					beta1[indArgv].num2 = errStat;
					argv[indArgv][indChar] = '\0';
					free(str);
					break;
				}
			}

			//####################################################################
			//printf("valid sort options: '%s'\n", argv[indArgv]);
			beta1[indArgv].flag = 1;
			beta1[indArgv].str = (char*)malloc(strlen(argv[indArgv])-1+1);
			strcpy(beta1[indArgv].str, argv[indArgv]+1);
			//####################################################################
		}
		else { //path
			//####################################################################
			//printf("Normal argument %s\n", argv[indArgv]);
			beta1[indArgv].flag = 0;
			beta1[indArgv].str = (char*)malloc(strlen(argv[indArgv])+1);
			strcpy(beta1[indArgv].str, argv[indArgv]);
			//####################################################################
		}
	}

	return 0;
}






int isValidInteger(char *strOptArg) {
	int lenStrOptArg = strlen(strOptArg);
	int ind = 0;
	int num = 0;
	while(ind < lenStrOptArg) {
		if(strOptArg[ind] < 48 || strOptArg[ind] > 57)
			return -1;
		num = num*10+(strOptArg[ind]-48);
		ind++;
	}
	return num;
}

int isValidUnit(char *strOptArg, unsigned long *coeff, int *base, int *exp) {
	char buffer[12] = {'k','K','m','M','g','G','t','T','p','P','e','E'}; //,'z','Z','y','Y'};
	int ind = 0; //ind of strOptArg[]
	int num = 1;
	unsigned long coeff1 = 0, coeff2 = 0, coeff3 = 1;

	while(1) {
		if(strOptArg[ind] < 48 || strOptArg[ind] > 57)
			break;
		*coeff = *coeff*10+(strOptArg[ind]-48);
		num = 0;
		ind++;

		//range validation for coeff
		if(coeff1 > *coeff)
			return -2;
		coeff1 = *coeff;
	}
	*coeff += num; //make coeff=0 to coeff=1, if while is stop at first iteration

	//checking units
	int indBuff = -1;
	int flag = -1;
	while(++indBuff < 12)
		if(strOptArg[ind] == buffer[indBuff]) {
			flag = indBuff;
			*exp = flag/2+1;
			break;
		}

	if(*coeff == 0)
		return -1;
	else if(num != 1 && strOptArg[ind] == '\0')
		*base = 0;
	else if(flag != -1 && strOptArg[ind+1] == '\0')
		*base = 1024;
	else if(flag != -1 && flag%2 != 0 && strOptArg[ind+1] == 'B' && strOptArg[ind+2] == '\0')
		*base = 1000;
	else
		return -1;

	//range validation for units
	coeff2--;
	coeff1 = coeff2/(*coeff);
	int exp1 = *exp;
	while(exp1--) {
		coeff3 *= (unsigned long)(*base);
		if(coeff3 > coeff1)
			return -2;
	}
	
	return 0;
}

int validateDhiphenArgStr(char *strOptArg, int lenArgs, char **listArgs) {
	int ind1 = 0;       //ind of listArgs[]
	int indStr = 0;     //ind of strOptArg[]
	int findAtInd = -1; //ind of listArgs[indBlock][]
	int cntSubStr = 0;

	//matching string by string
	while(ind1 < lenArgs) {
		int ind2 = 0; //ind of listArgs[i][]
		int jflag = 0;
		if(strOptArg[ind2] == '+' && listArgs[0][0] == '+') {
			findAtInd = -3;
			break;
		}
		while(1) {
			if(strOptArg[ind2] == '\0')
				break;
			if(listArgs[ind1][indStr] == '\0') {
				ind1++;
				indStr = 0;
				jflag = 1;
				break;
			}
			if(listArgs[ind1][indStr] == '/') {
				indStr++;
				jflag = 1;
				break;
			}
			if(strOptArg[ind2] != listArgs[ind1][indStr]) {
				while(listArgs[ind1][indStr] != '/' && listArgs[ind1][indStr] != '\0')
					indStr++;
				jflag = 1;
				break;
			}
			ind2++;
			indStr++;
		}
		if(jflag == 0) { //issubstr
			findAtInd = ind1;
			cntSubStr++;
			if(cntSubStr > 1)
				return -2; //ambiguous argument
			ind1++;
			indStr = 0;
		}
	}
		
	return findAtInd;

}

int validateDhiphenOption(char *strArgv, char **allOptions, int *indBig, int *indEnd) {
	//the technique using in this function for searching is because of 
	//ambiguous options list found sequencially in range (indBig, indEnd)
	int indCur = 0;
	int lenStrArgv = strlen(strArgv);
	int indChar = 1;
	int flag1 = 0;
	//vertical character matching
	while(++indChar < lenStrArgv) {
		if(strArgv[indChar] == '=')
			return indChar;

		indCur = *indBig;
		flag1 = 1;
		while(indCur < *indEnd) {
			if(flag1 == 1 && strArgv[indChar] == allOptions[indCur][indChar-1]) {
				flag1 = 2;
				*indBig = indCur;
			}
			else if(flag1 == 2 && strArgv[indChar] != allOptions[indCur][indChar-1]) {
				*indEnd = indCur;
				break;
			}
			indCur++;
		}

		if(flag1 == 1)
			return -1;
		if(strArgv[indChar+1] == '\0' && allOptions[*indBig][indChar] == '\0') {
			*indEnd = *indBig+1;
			break;
		}
		if(strArgv[indChar+1] == '=' && allOptions[*indBig][indChar] == '\0') {
			*indEnd = *indBig+1;
		}
	}
	return flag1;
}


