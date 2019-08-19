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
int k=0; 
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




void *receive(void *varg)
{
	char *msg = (char*)malloc(MESS_SIZE*sizeof(char));
	char *c_1 = (char*)malloc(MESS_SIZE*sizeof(char));
	char *c_2 = (char*)malloc(MESS_SIZE*sizeof(char));
	//while(1)
	//
		bzero(msg,MESS_SIZE);
		if(read(sock, msg, MESS_SIZE) > 0)
		{
			c_1 = strcut_1(msg,24);		
			int humi=0, temp=0;
			humi = (int)c_1[0]; //+ 0.1*(int)c[1];
			temp = (int)c_1[1]; //+ 0.1*(int)c[3];
			if( (humi > 60 && humi <95) && (temp > 10 && temp < 40) )
			{
				c_2 = strcut_2(msg,24);
				printf("Location: Ha Noi City\n");
				printf("Time: %s",c_2);
				printf("Humidity: %d%, Temperature: %d oC\n",humi,temp);
			}
			else
				printf("Humidity or Temperature invalid ->  Please request again\n");
		}
	//}
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
	//char *send_msg = (char*)malloc(MESS_SIZE*sizeof(char));
	int choose = 0;
	//while(1)
	//{
		printf("Do you want to measure humidity and temperature:\n");
		printf("0. No, I don't want to measure\n");
		printf("1. Yes, I want to measure\n");
         	gets(msg);
		choose = convert_CharToInt(msg);
		switch(choose)
		{
			case 0:
				exit(1);
			case 1:
				write(sock, msg, strlen(msg));
				break;
			default:
				printf("Please choose 0 or 1\n");
		}
		//if(strcmp(msg, "0") == 0)
		//	exit(1);
	//}
}


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
	/* After connected to server */
	//puts("Welcome to my server, can you tell me your name: ");
	//gets(name);
	//strcat(name, ": ");
	pthread_t THREAD_1, THREAD_2;
	pthread_create(&THREAD_1, NULL, receive, NULL);
	pthread_create(&THREAD_2, NULL, send_to,  NULL);
	pthread_join(THREAD_1, NULL);
	pthread_join(THREAD_2, NULL);
	return 0; 
} 
