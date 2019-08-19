#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

char *strcut(char *c, int n)
{
	char *temp = (char*)calloc(10,sizeof(char));
	int i=0;
	while( n < strlen(c))
	{
		temp[i] = c[n+1];
		i++;
		n++;
	}
	return temp;
}

int main()
{
	time_t rawtime;
	struct tm *info;
	char *moment = (char*)calloc(512,sizeof(char));
	char *c = (char*)calloc(64,sizeof(char));
	char *k = (char*)calloc(64,sizeof(char));
	c[0] = 49;
	c[1] = 53;
	time(&rawtime);
	info = localtime(&rawtime);
	strcpy(moment,asctime(info));
	printf("len: %d\n",strlen(moment));
	puts(moment);
	strcat(moment,c);
	puts(moment);
	printf("len after: %d\n",strlen(moment));
	k = strcut(moment,24);
	puts(k);
	printf("len after2: %d\n",strlen(k));
	return 0;
}
