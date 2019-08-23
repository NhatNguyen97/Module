#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>


void strreplace(char *c)
{
    int i=0;
	for(i=0 ; i< strlen(c); i++)
	{
		if(c[i] == ' ')
			c[i] = '-';
	}
}

int main()
{
	//char *s = (char*)calloc(64,sizeof(char));
	char *c = (char*)calloc(64,sizeof(char));
	gets(c);
	strreplace(c);
	puts(c);	
	return 0;
}
