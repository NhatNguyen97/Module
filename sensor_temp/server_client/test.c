#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

char *convert_IntToChar(int x)
{
	int i=1;
	char *s1 = (char*)calloc(4,sizeof(char));
	int *s2 = (int*)calloc(4,sizeof(int));
	while(x > 0)
	{
		s2[i] = x%10;
		x /= 10;
		i--;
	}
	for(i=0 ; i<2 ; i++)
		s1[i] = s2[i] + '0';
	return s1;
}


int main()
{
	int x=49;
	char *s = (char*)calloc(4,sizeof(char));
	s = convert_IntToChar(x);
	printf("%s",s);
	return 0;
}
