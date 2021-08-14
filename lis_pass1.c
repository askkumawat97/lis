
int pass1(int, struct beta*, int*, char*(*)[], char**);
void flagSetter(char*, int*);
void pass1print(int, int*, char*(*)[], char**);

int pass1(int argc, struct beta beta1[], int flagSet[], char *pattern[2][argc], char *pathList[]) {
	char *charSet[58] = {
		"C0","D1E0","D1E1","F1","G1","H1","I1","J1",
		"K1L1R1S0T1U1V1W5c2","K1L1R1S1T0U1V1W5c2","K1L1R1S1T1U1V1W1c2",
		"K1L1R1S1T1U1V1W5c2","K1L1R1S2T2U1V1W5c2",
		"M1","O1","S0","V2","V3","W1","W2","W3","W4","W5","X2","]1","]2","]3","`0",
		"`0D1E1i0","`2","`2a1b3","`3","`4a1b3","`5","`6","`7","`8",
		"c1K0L0R0S0T0U0V0W0","c2","d0",
		"d1","d2","d3","e0f0","e0f1","e0f4","e1f1","e2f2","e3f3",
		"g1K0L0R0S0T0U0V0W0c0","g2K0L0R0S0T0U0V0W0c0","h0","h1","h2","i0","i1","i2","j0"
	};

	int shiphen[74] = {
		38,
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
		1,0,50,57/**/,-1,42,15,57/**/,-1,-1,-1,57/**/,-1,43,-1,-1,47,3,33,-1,27,-1,-1,35,-1,6,
		-1,-1,-1,-1,-1,-1,
		2,45,32,5,-1,28,8,26,14,-1,23,11,37,12,9,40,44,4,7,31,30,34,-1,49,-1,-1
	};

	//dhiphen+shiphen	only dhiphen	dhiphen+shiphen	indBig_of_dhiphen1
	int dhiphen[43] = {
		2,1,			13,24,		42,		0,
		6,57/**/,57/**/,	57/**/,		5,57/**/,45,
					41,				3,
					10,36,-1,-1,	44,26,		8,
		-1,0,							11,
		14,23,43,15,12,47,					15,
		3,4,			44,25,		7,		25,
					-1,				30,32,
					-1,-1
	};

	int dhiphen1[37] = {
		54,55,56,			//color
		11,37,49,50,38,			//format
		51,52,53,			//hyperlink
		39,40,41,42,			//ind style
		43,44,46,46,44,47,47,45,48,48,	//quot style
		27,29,33,35,34,			//sort
		17,16,				//time
		18,19,20,21,22			//time style
	};


	int ind = 0;
	int indPat[2] = {0, 0};
	int indPath = 0;
	flagSet[0] = beta1[0].num1; // 1-help 2-version

	if(flagSet[0] != 1 || flagSet[0] != 2)
	while(++ind < argc) {
		if(beta1[ind].flag == 0) { // PATHLIST
			int len = strlen(beta1[ind].str);
			if(beta1[ind].str[len-1] == '/') {
				if(strlen(beta1[ind].str) == 1) //root directory
					flagSet[5] = 0; //desable recurssion
				else
					beta1[ind].str[len-1] = '\0';
			}
			else
				len++; //+1 for '\', if indicator required
			pathList[indPath] = (char*)malloc(len+1);
			strcpy(pathList[indPath], beta1[ind].str);
			pathList[indPath+1] = NULL;
			indPath++;
		}
		else if(beta1[ind].flag == 7) { // =+FORMAT
			flagSetter(charSet[dhiphen1[dhiphen[beta1[ind].indOpt]+beta1[ind].num1]], flagSet);
			pattern[0][0] = (char*)malloc(strlen(beta1[ind].str)+1);
			strcpy(pattern[0][0], beta1[ind].str);
		}
		else if(beta1[ind].flag == 3) { // =PATTERN
			pattern[beta1[ind].num1][++indPat[beta1[ind].num1]] = (char*)malloc(strlen(beta1[ind].str)+1);
			strcpy(pattern[beta1[ind].num1][indPat[beta1[ind].num1]], beta1[ind].str);
		}
		else if(beta1[ind].flag == 4) { // =COLS
			flagSet[29+(beta1[ind].indOpt/37)] = beta1[ind].num1;
		}
		else if(beta1[ind].flag == 5) { // =SIZE
			flagSetter(charSet[24], flagSet);

			flagSet[26] = beta1[ind].num1;
			flagSet[27] = beta1[ind].num2;

			unsigned long long1 = beta1[ind].coeff;
			flagSet[24] = long1;
			long1 = long1 >> (sizeof(long1)*4);
			flagSet[25] = long1;
		}
		else if(beta1[ind].flag == 1) { // SHIPHEN
			int len = strlen(beta1[ind].str);

			if(beta1[ind].num1 == 2) { //tabsize
				flagSet[30] = beta1[ind].num2;
				continue;
			}
			if(beta1[ind].num1 == 1) { //width
				flagSet[29] = beta1[ind].num2;
				continue;
			}
			if(beta1[ind].num1 == 3) { //ignore
				pattern[1][++indPat[1]] = (char*)malloc(strlen(beta1[ind].str+beta1[ind].num2)+1);
				strcpy(pattern[1][indPat[1]], beta1[ind].str+beta1[ind].num2);
				continue;
			}

			int ind1 = -1;
			while(++ind1 < len) {
					flagSetter(charSet[shiphen[beta1[ind].str[ind1]-49]], flagSet);
			}
		}
		else if(beta1[ind].flag == 2) { // =NULL
			flagSetter(charSet[dhiphen[beta1[ind].indOpt]], flagSet);
		}
		else if(beta1[ind].flag == 6) { // =WORD/[=WHEN]
			flagSetter(charSet[dhiphen1[dhiphen[beta1[ind].indOpt]+beta1[ind].num1]], flagSet);
		}
	}
	pattern[0][++indPat[0]] = NULL;
	pattern[1][++indPat[1]] = NULL;
	return indPath;
}


void flagSetter(char *charSetStr, int flagSet[]) {
	int ind = 0;
	while(charSetStr[ind] != '\0') {
		flagSet[charSetStr[ind]-65] = charSetStr[ind+1]-48;
		ind += 2;
	}
}


void pass1print(int argc, int flagSet[], char *pattern[2][argc], char *pathList[]) {
	int ind;
	if(flagSet[0] == 1) {
		printf("help file.\n");
		return;
	}
	if(flagSet[0] == 2) {
		printf("version file.\n");
		return;
	}

	ind = -1;
	printf("\n\npathList:\t");
	while(pathList[++ind] != NULL) printf("%s ", pathList[ind]);

	printf("\ntimeStyle:\t");
	if(pattern[0][0] != NULL) printf("%s", pattern[0][0]);

	ind = 0;
	printf("\nhide:\t\t");
	while(pattern[0][++ind] != NULL) printf("%s ", pattern[0][ind]);

	ind = 0;
	printf("\nignore:\t\t");
	while(pattern[1][++ind] != NULL) printf("%s ", pattern[1][ind]);
	printf("\n\n");

	if(flagSet[1] != 0) printf("normal files\n");
	if(flagSet[2] != 0) printf("backup files\n");
	if(flagSet[3] != 0) printf("hidden files\n");
	if(flagSet[4] != 0) printf("dot files\n");
	printf("\n");
	if(flagSet[5] != 0) printf("recursion\n");
	if(flagSet[6] != 0) printf("reverse\n");
	if(flagSet[7] != 0) printf("directory\n");
	printf("\n");
	if(flagSet[8] != 0) printf("context\n");
	if(flagSet[9] != 0) printf("-s\n");
	if(flagSet[10] != 0) printf("-l\n");
	printf("\n");
	if(flagSet[11] != 0) printf("permision\n");
	if(flagSet[12] != 0) printf("author\n");
	if(flagSet[13] != 0) printf("mode\n");
	if(flagSet[14] != 0) printf("inode\n");
	if(flagSet[15] != 0) printf("dev\n");
	if(flagSet[16] != 0) printf("rdev\n");
	if(flagSet[17] != 0) printf("hlink\n");
	if(flagSet[18] != 0) printf("uid\n");
	if(flagSet[19] != 0) printf("gid\n");
	if(flagSet[20] != 0) printf("     size:\t%d\n", flagSet[20]);
	if(flagSet[21] != 0) printf("     time:\t%d\n", flagSet[21]);
	if(flagSet[22] != 0) printf("timeStyle:\t%d\n", flagSet[22]);
	if(flagSet[23] != 0) printf("   blocks:\t%d\n", flagSet[23]);
	printf("\n");
	if(flagSet[24] != 0) printf("    coeff:\t%d\n", flagSet[24]);
	if(flagSet[25] != 0) printf("    coeff:\t%d\n", flagSet[25]);
	if(flagSet[26] != 0) printf("     base:\t%d\n", flagSet[26]);
	if(flagSet[27] != 0) printf("      exp:\t%d\n", flagSet[27]);
	printf("\n");
	if(flagSet[28] != 0) printf("  hr-base:\t%d\n", flagSet[28]);
	printf("\n");
	if(flagSet[29] != 0) printf("    width:\t%d\n", flagSet[29]);
	if(flagSet[30] != 0) printf("  tabsize:\t%d\n", flagSet[30]);
	printf("\n");
	if(flagSet[31] != 0) printf("     sort:\t%d\n", flagSet[31]);
	if(flagSet[32] != 0) printf("  sort -l:\t%d\n", flagSet[32]);
	if(flagSet[33] != 0) printf(" sort -lt:\t%d\n", flagSet[33]);
	printf("\n");
	if(flagSet[34] != 0) printf("seperator:\t%d\n", flagSet[34]);
	if(flagSet[35] != 0) printf("indicator:\t%d\n", flagSet[35]);
	if(flagSet[36] != 0) printf("     quot:\t%d\n", flagSet[36]);
	if(flagSet[37] != 0) printf("  esc-seq:\t%d\n", flagSet[37]);
	if(flagSet[38] != 0) printf("     flow:\t%d\n", flagSet[38]);
	printf("\n");
	if(flagSet[39] != 0) printf("hyperlink:\t%d\n", flagSet[39]);
	if(flagSet[40] != 0) printf("    color:\t%d\n", flagSet[40]);
	printf("\n");
}

