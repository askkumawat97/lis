
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/stat.h>	//struct stat, stat(), st_mode S_FLAG
#include<pwd.h>		//struct passwd, getpwuid()
#include<grp.h>		//struct group, getgrgid()
#include<time.h>	//struct tm, gmtime(), strftime()

#include"lis_print.c"

//representing a single file/dir
struct set {
	int esc_seq;	//indicator, quoting
	int blocks;
	int strLen;
	long lsize;

	char *strName;
	char *ext;	//sort by ext, print flow
	char *csize;
	char *ssize;
	char *inode;
	char *perm;
	char *nlink;
	char *uid;
	char *gid;
	char *context;
	char *timeStr;
	struct timespec time[3]; //mtime,ctime,atime

	struct set *link;
};


void initSet(struct set*);
int power(int, int);
int toString(char**, long); //long to string conversion
unsigned long upperBound(double);
int patternChecker(int, int*, char*(*)[], char*);
int patternMatcher(char*, char**);
void fillSet(struct set*, int*, int*, char*);
void sortSet(struct set*, int);
void reverseSet(struct set*);
void format_quotIndcat(struct set*, int*);
void format_timeStyle(struct set*, int*, char*);
void format_size(struct set*, int*, int*);
int sizeAsHumanReadable(char**, unsigned long, int, int);
int sizeAsBlockSizeForm(char**, unsigned long, int*);
int horizntlPrintFlow(struct set*, int*, int*, int, int, int);
int verticalPrintFlow(struct set*, int*, int*, int, int, int);


void pass2(int argc, int *flagSet, char *pattern[2][argc], char *path, struct set *keySet, int winWidth) {
	//FILTER_0 = INITIATING
	if(flagSet[0] == 1) {
		printHelp();
		return;
	}
	else if(flagSet[0] == 2) {
		printVersion();
		return;
	}


	//variables
	int ind = 0;
	int len = 0;
	int errStat = 0;
	int totalBlocks = 0;
	int totalCount = 0; //total no. of file/dir
	int maxLen[50] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	struct set *nextSet, *tempSet;


	//FILTER_1 = RESHAPING
	nextSet = keySet;
	while(nextSet->link != NULL) {
		errStat = patternChecker(argc, flagSet, pattern, nextSet->link->strName);
		if(errStat == 0) { //pattern matched, remove from list
			tempSet = nextSet->link;
			nextSet->link = nextSet->link->link;

			free(tempSet->strName);
			tempSet->strName = NULL;
			tempSet->link = NULL;
			free(tempSet);
			tempSet = NULL;
			continue;
		}
		totalCount++;
		nextSet = nextSet->link;
	}
	if(totalCount == 0)
		return;


	//FILTER_2 = APPENDING
	nextSet = keySet->link;
	while(nextSet != NULL) {
		fillSet(nextSet, flagSet, maxLen, path);
		totalBlocks += nextSet->blocks;
		nextSet = nextSet->link;
	}


	//FILTER_3 = SEQUENCING
	if(flagSet[33] != 0)
		sortSet(keySet, 3); //mtime
	else if(flagSet[32] != 0)
		sortSet(keySet, 1); //name
	else if(flagSet[31] != 0)
		sortSet(keySet, flagSet[31]);

	if(flagSet[7] == 1)
		sortSet(keySet, 8); //dir first
	if(flagSet[6] == 1)
		reverseSet(keySet);


	//FILTER_4 = FORMATING
	nextSet = keySet->link;
	while(nextSet != NULL) {
		format_size(nextSet, flagSet, maxLen);
		format_quotIndcat(nextSet, flagSet);
		if(flagSet[21] != 0)
			format_timeStyle(nextSet, flagSet, pattern[0][0]);

		if(flagSet[14] == 1) { //inode
			len = maxLen[14]-strlen(nextSet->inode);
			if(len != 0) {
				char *tempStr = (char*)malloc(maxLen[14]+1);
				for(int i = len, j = 0; i < maxLen[14]; i++,j++)
					tempStr[i] = nextSet->inode[j];
				tempStr[maxLen[14]] = '\0';

				while(len--)
					tempStr[len] = '0';
				free(nextSet->inode);
				nextSet->inode = tempStr;
			}
		}
		if(flagSet[17] == 1) {//nlink
			len = maxLen[17]-strlen(nextSet->nlink);
			if(len != 0) {
				char *tempStr = (char*)malloc(maxLen[17]+1);
				for(int i = len, j = 0; i < maxLen[17]; i++,j++)
					tempStr[i] = nextSet->nlink[j];
				tempStr[maxLen[17]] = '\0';

				while(len--)
					tempStr[len] = '0';
				free(nextSet->nlink);
				nextSet->nlink = tempStr;
			}
		}

		nextSet = nextSet->link;
	}


	//FILTER_5 = FLOWING
	if(totalCount > 1 && flagSet[34] == 0 && flagSet[38] != 0) { //print flow
		errStat = 0;
		//maxLen of inode, size, context
		len = 0;
		if(flagSet[14] != 0 && maxLen[14] != 0) len += maxLen[14]+1;
		if(flagSet[9] != 0 && maxLen[9] != 0) len += maxLen[9]+1;
		if(flagSet[8] != 0 && maxLen[8] != 0) len += maxLen[8]+1;

		//strLen is length of printable string, flow is used for string fitting
		int ind = 0;
		int *flow1 = (int*)malloc(sizeof(int)*totalCount);
		int *flow2 = (int*)malloc(sizeof(int)*totalCount);
		nextSet = keySet->link;
		while(nextSet != NULL) {
			nextSet->strLen = strlen(nextSet->strName);
			nextSet->strLen += len+2;
			flow1[ind] = nextSet->strLen;
			flow2[ind] = nextSet->strLen;

			ind++;
			nextSet = nextSet->link;
		}

		if(errStat != -1 && flagSet[38] == 1)
			errStat = horizntlPrintFlow(keySet, flow1, flow2, totalCount, len, winWidth);
		else if(errStat != -1 && flagSet[38] == 2)
			errStat = verticalPrintFlow(keySet, flow1, flow2, totalCount, len, winWidth);

		free(flow1);
		free(flow2);

		if(errStat == -1) {
			flagSet[38] = 0;
			flagSet[34] = 2;
		} 
	}


	//if(flagSet[29] != 0) printf("width:\t%d\n", flagSet[36]);
	//if(flagSet[30] != 0) printf("tabsize:\t%d\n", flagSet[37]);
	//if(flagSet[37] != 0) printf("hyperlink:\t%d\n", flagSet[47]);
	//if(flagSet[38] != 0) printf("color:\t%d\n", flagSet[48]);


	//FILTER_6 = PRINTING
	if(flagSet[5] == 1) {
		if(flagSet[40] == 2) { //--color=auto
			int color = strlen(path)%6;
			printf("\033[1;%dm", 31+color);
			printf("\n%s/:\n", path);
			printf("\033[0m"); //reset to default color
		}
		else
			printf("\n%s/:\n", path);
	}
	if(flagSet[10] != 0)
		printf("total %d\n", totalBlocks/2);

	nextSet = keySet->link;
	while(nextSet != NULL) {
		if(flagSet[40] == 2) { //--color=auto
			if(nextSet->perm[0] == 'd') {
				printf("\033[1;37m");
				printf("\033[1;44m");
			}
			else {
				int color = random();
				color = color%6+1;
				color = 30+color;
				printf("\033[1;%dm", color);
			}
		}

		if(flagSet[14] == 1) printf("%s ", nextSet->inode);
		if(flagSet[9] == 1) {
			len = maxLen[9]-strlen(nextSet->ssize);
			while(len--) printf(" ");
			printf("%s ", nextSet->ssize);
		}
		if(flagSet[11] == 1) printf("%s ", nextSet->perm);
		if(flagSet[17] == 1) printf("%s ", nextSet->nlink);
		if(flagSet[12] != 0) printf("%s ", nextSet->uid);
		if(flagSet[18] != 0) printf("%s ", nextSet->uid);
		if(flagSet[19] != 0) printf("%s ", nextSet->gid);
		if(flagSet[8] == 1) printf("%s ", nextSet->context);
		if(flagSet[20] == 1) {
			len = maxLen[20]-strlen(nextSet->csize);
			while(len--) printf(" ");
			printf("%s ", nextSet->csize);
		}
		if(flagSet[21] != 0) printf("%s ", nextSet->timeStr);


		if(flagSet[40] != 2 && nextSet->perm[0] == 'd')
			printf("\033[1;34m");
		printf("%s", nextSet->strName);
		printf("\033[0m"); //reset to default color


		if(flagSet[34] == 1) printf(", ");
		if(flagSet[34] == 2) printf("\n");
		if(nextSet->ext != NULL && flagSet[34] == 0 && (flagSet[38] == 2 || flagSet[38] == 1)) //print flow
			printf("%s", nextSet->ext);

		nextSet = nextSet->link;
	}
	if(flagSet[34] != 2)
		printf("\n");



	//FILTER_7 = REMOVING //remove indicator, quoting
	nextSet = keySet->link;
	while(nextSet != NULL) {
		len = strlen(nextSet->strName);
		if(nextSet->esc_seq == 1) { //ind
			nextSet->strName[len-1] = '\0';
		}
		if(nextSet->esc_seq == 2) { //quot
			nextSet->strName[len-1] = '\0';
			strcpy(nextSet->strName, nextSet->strName+1);
		}
		if(nextSet->esc_seq == 3) { //ind, quot
			nextSet->strName[len-1] = '\0';
			nextSet->strName[len-2] = '\0';
			strcpy(nextSet->strName, nextSet->strName+1);
		}

		nextSet = nextSet->link;
	}


	//FILTER_8 = FREEING
	tempSet = keySet->link;
	while(tempSet != NULL) {
		if(tempSet->ext != NULL) free(tempSet->ext), tempSet->ext = NULL;
		if(tempSet->inode != NULL) free(tempSet->inode), tempSet->inode = NULL;
		if(tempSet->ssize != NULL) free(tempSet->ssize), tempSet->ssize = NULL;
		if(tempSet->perm != NULL) free(tempSet->perm), tempSet->perm = NULL;
		if(tempSet->nlink != NULL) free(tempSet->nlink), tempSet->nlink = NULL;
		if(tempSet->uid != NULL) free(tempSet->uid), tempSet->uid = NULL;
		if(tempSet->gid != NULL) free(tempSet->gid), tempSet->gid = NULL;
		if(tempSet->context != NULL) free(tempSet->context), tempSet->context = NULL;
		if(tempSet->csize != NULL) free(tempSet->csize), tempSet->csize = NULL;
		if(tempSet->timeStr != NULL) free(tempSet->timeStr), tempSet->timeStr = NULL;

		tempSet = tempSet->link;
	}

}





void initSet(struct set *keySet) {
	keySet->esc_seq = 0;
	keySet->blocks =0;
	keySet->strLen = 0;
	keySet->lsize = 0;
	keySet->time[0].tv_sec = 0;
	keySet->time[0].tv_nsec = 0;
	keySet->time[1].tv_sec = 0;
	keySet->time[1].tv_nsec = 0;
	keySet->time[2].tv_sec = 0;
	keySet->time[2].tv_nsec = 0;

	keySet->strName = NULL;
	keySet->ext = NULL;
	keySet->csize = NULL;
	keySet->ssize = NULL;
	keySet->inode = NULL;
	keySet->perm = NULL;
	keySet->nlink = NULL;
	keySet->uid = NULL;
	keySet->gid = NULL;
	keySet->context = NULL;
	keySet->timeStr = NULL;
	keySet->link = NULL;
}

int power(int base, int exp) {
	if(exp < 0) return 0;

	int ans = 1;
	while(exp) {
		ans = ans*base;
		exp--;
	}
	return ans;
}

int toString(char **buff1, long num) {
	char buff[20];
	buff[19] = '\0';
	int ind = 18;
	while(num) {
		buff[ind] = num%10+48;
		num = num/10;
		ind--;
	}
	*buff1 = (char*)malloc(20-ind);
	strcpy(*buff1, buff+(ind+1));

	return(20-ind-2);
}

unsigned long upperBound(double double1) {
	unsigned long long1 = double1;
	if((double)long1 != double1)
		long1++;
	return long1;
}

int patternChecker(int argc, int *flagSet, char *pattern[2][argc], char *strName) {
	int errStat = 0;
	//discard backup and hidden files
	if((flagSet[2] == 0 && strName[strlen(strName)-1] == '~') || 
		(flagSet[3] == 0 && strName[0] == '.') || 
		(flagSet[4] == 0 && (!strcmp(strName, ".") || !strcmp(strName, ".."))) ) {

		return 0;
	}
	//discard by ignore's pattern
	if(pattern[1][1] != NULL) {
		errStat = patternMatcher(strName, pattern[1]);
		if(errStat == 0)
			return 0;
	}
	//discard by hide's pattern
	if(pattern[0][1] != NULL || flagSet[2] == 1 || flagSet[3] ==1) {
		errStat = patternMatcher(strName, pattern[0]);
		if(errStat == 0)
			return 0;
	}
	return 1;
}

int patternMatcher(char *str, char **pattern) {
	int ind = 1;
	while(pattern[ind] != NULL) {
		int len1 = strlen(str);
		int len2 = strlen(pattern[ind]);
		int ind1 = 0;
		int ind2 = len1;
		int i = 0;
		int flag = 0;

		if(pattern[ind][0] == '*') {
			ind1 = len1-len2+1;
			i = 1;
		}
		else if(pattern[ind][len2-1] == '*')
			ind2 = len2-1;

		while(ind1 < ind2) {
			if(str[ind1] != pattern[ind][i]) {
				ind++;
				flag = 1;
				break;
			}
			ind1++;
			i++;
		}
		if(flag == 0) return 0;
	}
	return 1;
}

void fillSet(struct set *nextSet, int flagSet[], int maxLen[], char *path) {
	int len;
	int errStat;

	nextSet->strLen = strlen(nextSet->strName);
	char *pathStr = (char*)malloc(strlen(path)+1+nextSet->strLen+1);
	strcpy(pathStr, path);
	strcat(pathStr, "/");
	strcat(pathStr, nextSet->strName);

	struct stat setStat;
	errStat = stat(pathStr, &setStat);
	if(errStat != 0) {
		printf("lis: cannot access '%s': No such file or directory\n", pathStr);
		nextSet = nextSet->link;
		exit(0);
	}

	nextSet->blocks = setStat.st_blocks;
	if(flagSet[10] == 0) { //-l for permission
		nextSet->perm = (char*)malloc(2);
		*(nextSet->perm+0) = (setStat.st_mode & S_IFDIR) ? 'd' : '-';
		*(nextSet->perm+1) = '\0';
	}
	else {
	nextSet->perm = (char*)malloc(12);
	*(nextSet->perm+0) = (setStat.st_mode & S_IFDIR) ? 'd' : '-';
	*(nextSet->perm+1) = (setStat.st_mode & S_IRUSR) ? 'r' : '-';
	*(nextSet->perm+2) = (setStat.st_mode & S_IWUSR) ? 'w' : '-';
	*(nextSet->perm+3) = (setStat.st_mode & S_IXUSR) ? 'x' : '-';
	*(nextSet->perm+4) = (setStat.st_mode & S_IRGRP) ? 'r' : '-';
	*(nextSet->perm+5) = (setStat.st_mode & S_IWGRP) ? 'w' : '-';
	*(nextSet->perm+6) = (setStat.st_mode & S_IXGRP) ? 'x' : '-';
	*(nextSet->perm+7) = (setStat.st_mode & S_IROTH) ? 'r' : '-';
	*(nextSet->perm+8) = (setStat.st_mode & S_IWOTH) ? 'w' : '-';
	*(nextSet->perm+9) = (setStat.st_mode & S_IXOTH) ? 'x' : '-';
	*(nextSet->perm+10) = '\0';
	}

	if(flagSet[14] == 1) { //inode
		len = toString(&nextSet->inode, (unsigned long)setStat.st_ino);
		if(maxLen[14] < len)
			maxLen[14] = len;
	}

	if(flagSet[17] == 1) { //links
		len = toString(&nextSet->nlink, (unsigned long)setStat.st_nlink);
		if(maxLen[17] < len)
			maxLen[17] = len;
	}

	nextSet->time[0] = setStat.st_mtim;
	nextSet->time[1] = setStat.st_ctim;
	nextSet->time[2] = setStat.st_atim;

	nextSet->lsize = setStat.st_size;

	if(flagSet[8] == 1) { //context
		len = 1;
		nextSet->context = (char*)malloc(1+1);
		strcpy(nextSet->context, "?");
		if(len > maxLen[8])
			maxLen[8] = len;
	}

	if(flagSet[12] == 1 || flagSet[18] == 1) { //uid
		struct passwd *pwduid;
		pwduid = getpwuid(setStat.st_uid);

		if(pwduid != NULL) {
			len = strlen(pwduid->pw_name);
			nextSet->uid = (char*)malloc(len+1);
			strcpy(nextSet->uid, pwduid->pw_name);

			if(maxLen[12] < len) {
				maxLen[12] = len;
				maxLen[12] = len;
			}
		}
	}
	else {
		len = toString(&nextSet->uid, (unsigned long)setStat.st_uid);
		if(maxLen[12] < len) {
			maxLen[12] = len;
			maxLen[12] = len;
		}
	}

	if(flagSet[10] == 1)
	if(flagSet[19] == 1) { //gid
		struct group *grpgid;
		grpgid = getgrgid(setStat.st_gid);

		if(grpgid != NULL) {
			len = strlen(grpgid->gr_name);
			nextSet->gid = (char*)malloc(len+1);
			strcpy(nextSet->gid, grpgid->gr_name);

			if(maxLen[19] < len)
				maxLen[19] = len;
		}
	}
	else {
		len = toString(&nextSet->gid, (unsigned long)setStat.st_gid);
		if(maxLen[19] < len)
			maxLen[19] = len;
	}

}

void reverseSet(struct set *keySet) {
	struct set *sortedSet = (struct set*)malloc(sizeof(struct set));
	sortedSet->link = NULL;
	struct set *tempSet;

	while(keySet->link != NULL) {
		tempSet = keySet->link;
		keySet->link = keySet->link->link;

		tempSet->link = sortedSet->link;
		sortedSet->link = tempSet;
	}
	keySet->link = sortedSet->link;

	sortedSet->link = NULL;
	free(sortedSet);
}

void format_quotIndcat(struct set *nextSet, int flagSet[]) {
	char quotList[6] = {'\'','\'','\"','\"','\"','\"'}; //'“','”'};
	int flag = 0;
	if(flagSet[35] != 0 && nextSet->perm[0] == 'd') { // dir indicator
		char *tempStrName = (char*)malloc(nextSet->strLen+1);
		strcpy(tempStrName, nextSet->strName);
		strcat(tempStrName, "\\");
		free(nextSet->strName);
		nextSet->strName = tempStrName;
		nextSet->strLen++;
		flag = 1;
	}

	//finding escape sequence
	int ind = -1;
	while(nextSet->strName[++ind] != '\0') {
		if(nextSet->strName[ind] == ' ')
			nextSet->esc_seq = 1;
	}

	int len = ind;
	if(flagSet[36] != 0 && nextSet->esc_seq == 0) { //normal string
		char *strNameNew = (char*)malloc(len+1);
		strcpy(strNameNew, nextSet->strName);
		free(nextSet->strName);

		nextSet->strName = (char*)malloc(len+3);
		nextSet->strName[0] = quotList[(flagSet[36]-1)*2];
		strcpy(nextSet->strName+1, strNameNew);
		nextSet->strName[len+1] = quotList[(flagSet[36]-1)*2+1];
		nextSet->strName[len+2] = '\0';
		free(strNameNew);

		if(flag == 1) flag = 3;
		else flag = 2;
	}
	else if(flagSet[37] != 0 && nextSet->esc_seq == 1) { //esc-seq
		if(flagSet[37] != 4) {
			char *strNameNew = (char*)malloc(len+1);
			strcpy(strNameNew, nextSet->strName);
			free(nextSet->strName);

			nextSet->strName = (char*)malloc(len+3);
			nextSet->strName[0] = quotList[(flagSet[37]-1)*2];
			strcpy(nextSet->strName+1, strNameNew);
			nextSet->strName[len+1] = quotList[(flagSet[37]-1)*2+1];
			nextSet->strName[len+2] = '\0';

			if(flag == 1) flag = 3;
			else flag = 2;
		}
	}
	nextSet->esc_seq = flag;
}

void format_timeStyle(struct set *nextSet, int flagSet[], char *pattern) {
	char *monthStr[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	//time format = "2000-12-31 11:59:59.123456789 +0530";
	unsigned long long1 = 0;
	char *convert;
	int ind = -1, ind1 = -1, len = 0;
	nextSet->timeStr = (char*)malloc(35+1);
	struct tm *timeStamp;

	nextSet->time[flagSet[21]-1].tv_sec = nextSet->time[flagSet[21]-1].tv_sec+19800; //+0530
	timeStamp = gmtime(&nextSet->time[flagSet[21]-1].tv_sec);
	if (timeStamp == NULL)
		printf("invalid time\n");

	if(flagSet[22] == 1) { // =+FORMAT
		char buf[256];
		strftime(buf, 256, pattern, timeStamp);
		nextSet->timeStr = (char*)malloc(strlen(buf)+1);
		strcpy(nextSet->timeStr, buf);
		return;
	}


	timeStamp->tm_year = 1900+timeStamp->tm_year;
	timeStamp->tm_mon = 1+timeStamp->tm_mon;

	//date
	long1 = timeStamp->tm_year;
	long1 = long1*1000;
	long1 = long1+timeStamp->tm_mon;
	long1 = long1*1000;
	long1 = long1+timeStamp->tm_mday;
	len = toString(&convert, long1);
	convert[4] = '-';
	convert[7] = '-';

	if(flagSet[22] == 2 || flagSet[22] == 3)
		ind1 = -1;
	else if(flagSet[22] == 4)
		ind1 = 4;
	else {
		strcpy(nextSet->timeStr, monthStr[timeStamp->tm_mon-1]);
		nextSet->timeStr[3] = ' ';
		ind = 3;
		ind1 = 7;
	}

	while(convert[++ind1] != '\0')
		nextSet->timeStr[++ind] = convert[ind1];
	free(convert);


	//time
	long1 = timeStamp->tm_hour;
	long1 = long1*1000;
	long1 = long1+timeStamp->tm_min;
	long1 = long1*1000;

	if(flagSet[22] == 2)
		long1 = long1+timeStamp->tm_sec;

	len = toString(&convert, long1);
	convert[2] = ':';

	if(flagSet[22] == 2)
		convert[5] = ':';
	else
		convert[5] = '\0';

	ind1 = -1;
	nextSet->timeStr[++ind] = ' ';
	while(convert[++ind1] != '\0')
		nextSet->timeStr[++ind] = convert[ind1];
	free(convert);


	if(flagSet[22] == 2) {
		//nsec
		long1 = 1000000000;
		long1 = long1+(unsigned long)nextSet->time[flagSet[21]-1].tv_nsec;
		len = toString(&convert, long1);

		ind1 = 0;
		nextSet->timeStr[++ind] = '.';
		while(convert[++ind1] != '\0')
			nextSet->timeStr[++ind] = convert[ind1];
		free(convert);
		//time zone
		nextSet->timeStr[++ind] = ' ';
		nextSet->timeStr[++ind] = '+';
		nextSet->timeStr[++ind] = '0';
		nextSet->timeStr[++ind] = '5';
		nextSet->timeStr[++ind] = '3';
		nextSet->timeStr[++ind] = '0';
	}

	nextSet->timeStr[++ind] = '\0';
}

void format_size(struct set *nextSet, int flagSet[], int maxLen[]) {
	int len = 0;
	//for ssize
	if(flagSet[9] == 1) {
		unsigned long long1;
		double double1 = (double)nextSet->lsize/(double)4096;
		long1 = upperBound(double1);
		long1 = long1*4;

		if(flagSet[28] == 0)
			len = toString(&nextSet->ssize, long1);
		else if(flagSet[28] == 1)
			len = sizeAsBlockSizeForm(&nextSet->ssize, long1*1024, flagSet);
		else
			len = sizeAsHumanReadable(&nextSet->ssize, long1*1024, -2, flagSet[28]);

		if(len > maxLen[9])
			maxLen[9] = len;
	}

	//for csize
	if(flagSet[10] == 1) {
		if(flagSet[28] == 0)
			len = toString(&nextSet->csize, nextSet->lsize);
		else if(flagSet[28] == 1)
			len = sizeAsBlockSizeForm(&nextSet->csize, nextSet->lsize, flagSet);
		else
			len = sizeAsHumanReadable(&nextSet->csize, nextSet->lsize, -2, flagSet[28]);

		if(len > maxLen[20])
			maxLen[20] = len;
	}
}

int sizeAsHumanReadable(char **buff, unsigned long alpha, int exp, int flagSet) {
	int len = 0;
	double double1 = 0.0;
	double double2 = 0.0;
	unsigned long long1 = 0;
	char unitList[12] = {'k','K','m','M','g','G','t','T','p','P','e','E'};
	char *sizeStr;

	int base = 1000+(flagSet-2)*24;
	double1 = (double)alpha;
	while(double1 > base) {
		double1 = double1/(double)base;
		//rounding off(double1) upto one decimal digit
		double1 = double1*10.0;
		long1 = upperBound(double1);
		double1 = (double)long1/(double)10;

		exp += 2;
	}
	if(double1 >= 10) {
		long1 = upperBound(double1);
		len = toString(&sizeStr, long1);
	}
	else {
		long1 = (unsigned long)double1;
		len = toString(&sizeStr, long1);
		strcat(sizeStr, ".");
		double2 = double1-(double)long1;
		float float1 = (float)(double2*(double)10);
		int int1 = (int)float1;
		char *cint;
		int len1 = toString(&cint, int1);
		if(len1 == 0)
			strcat(sizeStr, "0");
		else
			strcat(sizeStr, cint);
		len += 2;
	}

	char unit[2];
	unit[0] = unitList[exp+(flagSet-2)];
	unit[1] = '\0';

	len++;
	strcat(sizeStr, unit);
	*buff = (char*)malloc(strlen(sizeStr)+1);
	strcpy(*buff, sizeStr);
	return len;
}

int sizeAsBlockSizeForm(char **buff, unsigned long alpha, int flagSet[]) {
	int len = 0;
	double double1 = 0.0;
	unsigned long long1 = 0;

	//int[2] to long conversion for blksize
	long1 = (unsigned long)flagSet[25];
	long1 = long1 << (sizeof(long1)*4);
	long1 = long1+(unsigned long)flagSet[24];
	unsigned long bsize = long1*(unsigned long)power(flagSet[26],flagSet[27]);

	double1 = (double)alpha/(double)bsize;
	long1 = upperBound(double1);
	len = toString(buff, long1);
	return len;
}

int horizntlPrintFlow(struct set *keySet, int *flow1, int *flow2, int totalCount, int len, int winWidth) {
	int ind = 0;
	int indEnd = totalCount;
	int sum = 0;
	int flag = 0;
	int sumDiff = 0;
	struct set *nextSet = NULL;
	int *flow3 = (int*)malloc(sizeof(int)*totalCount);

	while(1) {
		if(sumDiff == totalCount)
			break;

		sumDiff++;
		int j = 0;
		for(int k = 0; k < sumDiff; k++) {
			int max = flow1[k];
			for(int k1 = k+sumDiff; k1 < totalCount; k1 += sumDiff)
				if(max < flow1[k1])
					max = flow1[k1];
			flow2[j++] = max;
		}
		indEnd = j;

		sum = 0;
		flag = 0;
		for(int k = 0; k < indEnd; k++) {
			sum = sum+flow2[k];
			if(sum > winWidth) {
				flag = 1;
				break;
			}
		}
		if(flag == 1) {
			sumDiff--;
			indEnd--;
			if(sumDiff == 0) //single column, enable -1
				return -1;
			break;
		}

		for(int k = 0; k < indEnd; k++) //flow3 is prev iteration value of flow2
			flow3[k] = flow2[k];

	}

	//put spaces in ext
	char blankChar = ' ';
	int lineLength = 0;
	int ind1 = 0;
	ind = 0;
	nextSet = keySet->link;
	while(nextSet != NULL) {
		ind1 = ind%sumDiff;
		lineLength += flow3[ind1];
		len = flow3[ind1] - nextSet->strLen+2;
		if(ind1 == sumDiff-1) {
			len = len+winWidth-lineLength;
			lineLength = 0;
		}

		nextSet->ext = (char*)malloc(len+1);
		for(int k = 0; k < len; k++)
			nextSet->ext[k] = blankChar;
		nextSet->ext[len] = '\0';

		ind++;
		nextSet = nextSet->link;
	}
	free(flow3);
	return 0;
}

int verticalPrintFlow(struct set *keySet, int *flow1, int *flow2, int totalCount, int len, int winWidth) {
	int ind = 0;
	int indEnd = totalCount;
	int sum = 0;
	int flag = 0;
	int sumDiff = 2;
	struct set *nextSet = NULL;

	while(1) {
		sum = 0;
		flag = 0;
		for(int k = 0; k < indEnd; k++) {
			sum = sum+flow2[k];
			if(sum > winWidth) {
				flag = 1;
				break;
			}
		}

		if(flag == 0) //string fitting properly
			break;
		else { // calculate maximum length of columns by group of sumDiff
			if(sumDiff == totalCount) //single column, enable -1
				return -1;

			int j = 0;
			for(int k = 0; k < totalCount; k += sumDiff) {
				int max = flow1[k];
				for(int k1 = k+1; k1 < k+sumDiff && k1 < totalCount; k1++)
					if(max < flow1[k1])
						max = flow1[k1];
				flow2[j++] = max;
			}
			indEnd = j;
			sumDiff++;
		}
	}

	//put spaces in ext
	char blankChar = ' ';
	sumDiff--;
	ind = 0;
	nextSet = keySet->link;
	while(nextSet != NULL) {
		len = flow2[ind/sumDiff] - nextSet->strLen+2;
		nextSet->strLen = flow2[ind/sumDiff];
		nextSet->ext = (char*)malloc(len+1);
		for(int k = 0; k < len; k++)
			nextSet->ext[k] = blankChar;
		nextSet->ext[len] = '\0';

		ind++;
		nextSet = nextSet->link;
	}

	//sort diagonally, apend remaining line feed to end of line string
	indEnd = totalCount;
	struct set **set1 = (struct set**)malloc(sizeof(struct set*)*totalCount);

	ind = 0;
	nextSet = keySet->link;
	while(nextSet != NULL) {
		set1[ind++] = nextSet;
		nextSet = nextSet->link;
	}

	int lineLength = 0;
	nextSet = keySet;
	for(int k = 0; k < sumDiff; k++) {
		lineLength = 0;
		for(int k1 = k; k1 < totalCount; k1 += sumDiff) {
			nextSet->link = set1[k1];
			nextSet = nextSet->link;
			lineLength += nextSet->strLen;
		}
		len = winWidth-lineLength;

		int len1 = strlen(nextSet->ext);
		char *tempStr = (char*)malloc(len1+len+1);
		strcpy(tempStr, nextSet->ext);

		for(int k = len1; k < len1+len; k++)
			tempStr[k] = blankChar;
		tempStr[len1+len] = '\0';

		free(nextSet->ext);
		nextSet->ext = tempStr;
	}
	nextSet->link = NULL;
	free(set1);
	return 0;
}

void sortSet(struct set *keySet, int flag) {
	int cmp = 0;
	struct set *sortedSet = (struct set*)malloc(sizeof(struct set));
	sortedSet->link = NULL;
	struct set *tempSet, *nextSet;

	//printf("sort Flag: %d\n", flag);
	if(flag == 8) { //dir first
		struct set *nextKeySet = keySet;
		nextSet = sortedSet;
		while(nextKeySet->link != NULL) {
			if(nextKeySet->link->perm[0] == 'd') {
				nextSet->link = nextKeySet->link;
				nextKeySet->link = nextKeySet->link->link;
				nextSet->link->link = NULL;
				nextSet = nextSet->link;
				continue;
			}
			nextKeySet = nextKeySet->link;
		}
		nextSet->link = keySet->link;
		keySet->link = sortedSet->link;
	}
	else if(flag == 6) { //version (numbers in str)
		while(keySet->link != NULL) {
			tempSet = keySet->link;
			keySet->link = keySet->link->link;
			tempSet->link = NULL;

			//insert tempSet into sortedSet
			nextSet = sortedSet;
			while(1) {
				if(nextSet->link == NULL) {
					nextSet->link = tempSet;
					break;
				}

				int num1 = 0;
				int num2 = 0;
				int j1 = 0;
				int j2 = 0;

				while(1) {
					char char1 = nextSet->link->strName[j1];
					char char2 = tempSet->strName[j2];

					if(char1 == '\0' || char2 == '\0')
						break;
					if(char1 != char2) {
						if(char1 > 47 && char1 < 58)
							while(char1 > 47 && char1 < 58) {
								num1 = num1*10+char1-48;
								char1 = nextSet->link->strName[++j1];
							}
						if(char2 > 47 && char2 < 58)
							while(char2 > 47 && char2 < 58) {
								num2 = num2*10+char2-48;
								char2 = tempSet->strName[++j2];
							}

						if(num1 == num2) {
							if(char1 != char2) {
								cmp = char1-char2;
								break;
							}
						}
						else {
							cmp = num1-num2;
							break;
						}
					}
					j1++;
					j2++;
				}

				if(cmp > 0) {
					tempSet->link = nextSet->link;
					nextSet->link = tempSet;
					break;
				}
				nextSet = nextSet->link;
			}
		}
		keySet->link = sortedSet->link;
	}
	else if(flag == 1) { //sort by strName
		while(keySet->link != NULL) {
			tempSet = keySet->link;
			keySet->link = keySet->link->link;
			tempSet->link = NULL;

			//insert tempSet into sortedSet
			nextSet = sortedSet;
			while(1) {
				if(nextSet->link == NULL) {
					nextSet->link = tempSet;
					break;
				}

				int i = 0;
				cmp = 0;
				while(1) {
					if(nextSet->link->strName[i] == '\0' && tempSet->strName[i] != '\0')
						break;
					else if(nextSet->link->strName[i] != '\0' && tempSet->strName[i] == '\0') {
						cmp = 1;
						break;
					}
					else if(nextSet->link->strName[i] == '.' && tempSet->strName[i] != '.') {
						cmp = 1;
						break;
					}
					else if(nextSet->link->strName[i] != '.' && tempSet->strName[i] == '.')
						break;
					else if(nextSet->link->strName[i] != tempSet->strName[i]) {
						if(nextSet->link->strName[i] > tempSet->strName[i])
							cmp = 1;
						break;
					}
					i++;
				}

				if(cmp > 0) {
					tempSet->link = nextSet->link;
					nextSet->link = tempSet;
					break;
				}
				nextSet = nextSet->link;
			}
		}
		keySet->link = sortedSet->link;
	}
	else if(flag == 7) { //sort by extension
		//set ext pointer to starting of extension in strName
		nextSet = keySet->link;
		while(nextSet != NULL) {
			int ind = -1;
			while(nextSet->strName[++ind] != '\0') {
				if(nextSet->strName[ind] == '.') {
					nextSet->ext = nextSet->strName+ind+1;
					break;
				}
			}
			nextSet = nextSet->link;
		}

		while(keySet->link != NULL) {
			tempSet = keySet->link;
			keySet->link = keySet->link->link;
			tempSet->link = NULL;

			//insert tempSet into sortedSet
			nextSet = sortedSet;
			while(1) {
				if(nextSet->link == NULL) {
					nextSet->link = tempSet;
					break;
				}

				if(nextSet->link->ext == NULL && tempSet->ext == NULL)
					cmp = strcmp(nextSet->link->strName, tempSet->strName);
				else if(nextSet->link->ext == NULL)
					cmp = 0;
				else if(tempSet->ext == NULL)
					cmp = 1;
				else {
					cmp = strcmp(nextSet->link->ext, tempSet->ext);
					if(cmp == 0)
					cmp = strcmp(nextSet->link->strName, tempSet->strName);
				}

				if(cmp > 0) {
					tempSet->link = nextSet->link;
					nextSet->link = tempSet;
					break;
				}
				nextSet = nextSet->link;
			}
		}
		keySet->link = sortedSet->link;
		while(nextSet != NULL) {
			nextSet->ext = NULL;
			nextSet = nextSet->link;
		}
	}
	else if(flag == 5) { //sort by size
		while(keySet->link != NULL) {
			tempSet = keySet->link;
			keySet->link = keySet->link->link;
			tempSet->link = NULL;

			//insert tempSet into sortedSet
			nextSet = sortedSet;
			while(1) {
				if(nextSet->link == NULL) {
					nextSet->link = tempSet;
					break;
				}

				cmp = nextSet->link->lsize - tempSet->lsize;
				if(cmp > 0) {
					tempSet->link = nextSet->link;
					nextSet->link = tempSet;
					break;
				}
				nextSet = nextSet->link;
			}
		}
		keySet->link = sortedSet->link;
	}
	else { //sort by 2-mtime,3-ctime,4-atime
		int ind = flag-2; //sort by nextSet->time[ind]
		while(keySet->link != NULL) {
			tempSet = keySet->link;
			keySet->link = keySet->link->link;
			tempSet->link = NULL;

			//insert tempSet into sortedSet
			nextSet = sortedSet;
			while(1) {
				if(nextSet->link == NULL) {
					nextSet->link = tempSet;
					break;
				}

				cmp = (unsigned long)nextSet->link->time[ind].tv_sec 
					- (unsigned long)tempSet->time[ind].tv_sec;
				if(cmp > 0) {
					tempSet->link = nextSet->link;
					nextSet->link = tempSet;
					break;
				}
				nextSet = nextSet->link;
			}
		}
		keySet->link = sortedSet->link;
	}
	sortedSet->link = NULL;
	free(sortedSet);
}


