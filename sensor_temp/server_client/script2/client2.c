#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <pthread.h>

#define PORT 1500
#define MESS_SIZE 1024
#define NAME_SIZE 20

char name[20] = {0} ;
int sock = 3;
int k=0, s=0, t=0;
int  m=0, count=0, count_2=0;
int limit=0;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void error(char *msg)
{
	perror(msg);
	exit(1);
}

int convert_CharToInt(char *c)
{
	int i,s=0,a[10];
	int len = strlen(c);
	for(i=0 ; i<len ; i++)
	{
		a[i] = c[i] - '0';
	}
	for(i=0; i<len ; i++)
		s = a[i] + s*10;
	return s;
}


char *strcut_1(char *c, int n)
{
	char *temp = (char*)calloc(MESS_SIZE,sizeof(char));
	int i=0;
	while( n < strlen(c))
	{
		temp[i] = c[n+1];
		i++;
		n++;
	}
	return temp;
}

char *strcut_2(char *c, int n)
{
	char *temp = (char*)calloc(MESS_SIZE,sizeof(char));
	int i=0;
	while( i <= n )
	{
		temp[i] = c[i];
		i++;
	}
	return temp;
}


void processing_Temp(float temp, float a, char *msg)
{
	if( temp > a)
	{
		char b[sizeof(float)];
		memcpy(b,&temp, sizeof(float));
		msg[strlen(msg) -1] = '\t';
		strcat(msg, b);
		limit++;
		FILE *flog = fopen("temp_log.txt", "a+");
		fprintf(flog, "%s\n", msg);
		fclose(flog);
		printf("Warning!!! The temperature is higher %f ...\n", a); 
	}
}


void *receive(void *varg)
{
	char *msg = (char*)calloc(MESS_SIZE,sizeof(char));
	char *c_1 = (char*)calloc(MESS_SIZE,sizeof(char));
	char *c_2 = (char*)calloc(MESS_SIZE,sizeof(char));
	bzero(msg,MESS_SIZE);
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);
	printf("s: %d\n",s);
	printf("m: %d\n",m);
	printf("t: %d\n",t);
	printf("count_2: %d\n",count_2);
	while(count_2 > 0)
	{	
			read(sock, msg, MESS_SIZE);
			c_1 = strcut_1(msg,24);		
			c_2 = strcut_2(msg,24);
			float temp=0;
			temp = (int)c_1[2] + 0.1*(int)c_1[3];
			printf("Location: Ha Noi City\n");
			printf("Time: %s",c_2);
			printf("Humidity: %d.%d%, Temperature: %d.%d oC\n",(int)c_1[0], (int)c_1[1], (int)c_1[2], (int)c_1[3]);
			processing_Temp(temp, t, c_2);
			count_2--;
	}
	free(msg);
	free(c_1);
	free(c_2);
	msg = NULL;
	c_1 = NULL;
	c_2 = NULL;
}

void *send_to(void *varg)
{
	char *msg = (char*)malloc(MESS_SIZE*sizeof(char));
	pthread_mutex_lock(&mutex);
	int a=0;
	printf("[REQUIRE]-Read data per turn (unit: second): ");
	scanf("%d",&s);
	printf("\n[REQUIRE]-Time monitor (unit: minutes): ");
	scanf("%d",&m); 
	printf("\n[REQUIRE]-Temperature moniter (unit: oC): ");
	scanf("%d",&t);
	a = (int)(m*60)/s;
	count_2 = a;
	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&cond);
	while( a > 0 )
	{
        	msg = "1";
		write(sock, msg, strlen(msg));
		a--;
		sleep(s);
	}
}
/*
void *receive(void *varg)
{
	char *msg = (char*)calloc(MESS_SIZE,sizeof(char));
	char *c_1 = (char*)calloc(MESS_SIZE,sizeof(char));
	char *c_2 = (char*)calloc(MESS_SIZE,sizeof(char));
	bzero(msg,MESS_SIZE);
	pthread_mutex_lock(&mutex);
	printf("s: %d\n",s);
	printf("m: %d\n",m);
	printf("t: %d\n",t);
	printf("count_2: %d\n",count_2);
	while(1)
	{	
			read(sock, msg, MESS_SIZE);
			c_1 = strcut_1(msg,24);		
			c_2 = strcut_2(msg,24);
			float temp;
			temp = (int)c_1[2] + 0.1*(int)c_1[3];
			printf("Location: Ha Noi City\n");
			printf("Time: %s",c_2);
			printf("Humidity: %d.%d%, Temperature: %d.%d oC\n",(int)c_1[0], (int)c_1[1], (int)c_1[2], (int)c_1[3]);
			processing_Temp(temp, t, count);
			count_2--;
	}
	//pthread_mutex_unlock(&mutex);
	free(msg);
	free(c_1);
	free(c_2);
	msg = NULL;
	c_1 = NULL;
	c_2 = NULL;
}
*/
int main(int argc, char const *argv[]) 
{  
    	struct sockaddr_in serv_addr; 
    	char add[20] = "127.0.0.1";
    	//printf("Enter server address: \n");
    	//gets(add);

    	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
		error("Couldn't create socket");

    	serv_addr.sin_family = AF_INET; 
    	serv_addr.sin_port = htons(PORT); 
	
	if(inet_pton(AF_INET, add, &serv_addr.sin_addr) <= 0)
		error("Invalid address/ Address not supported "); 
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        	error("Connection failed"); 
	pthread_t THREAD_1, THREAD_2;
	pthread_create(&THREAD_1, NULL, receive, NULL);
	pthread_create(&THREAD_2, NULL, send_to,  NULL);
	pthread_join(THREAD_1, NULL);
	pthread_join(THREAD_2, NULL);
	return 0; 
} 
