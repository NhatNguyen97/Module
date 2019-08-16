#include <stdio.h>
#include <stdlib.h>
int main()
{
	char *c = (char*)malloc(10*sizeof(char));
	c = '12';
	printf("%d\n",*c);
	return 0;
}
