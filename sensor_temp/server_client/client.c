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
#define MESS_SIZE 500
#define NAME_SIZE 20

char name[20] = {0} ;
int sock = 3; 
void error(char *msg)
{
	perror(msg);
	exit(1);
}


void *receive(void *varg)
{
	char *msg = (char*)malloc(MESS_SIZE*sizeof(char));
	while(1)
	{
		bzero(msg,MESS_SIZE);
		if(read(sock, msg, MESS_SIZE) > 0)
		puts(msg);
	}
}

void *send_to(void *varg)
{
	char *msg = (char*)malloc(MESS_SIZE*sizeof(char));
	char *send_msg = (char*)malloc(MESS_SIZE*sizeof(char));
	while(1)
	{
	puts("How long have you want to measure: ");
	do{
		gets(msg);
		if(0 == strlen(msg))
			puts("\nHow long have you want to measure : ");
		}
		while(0 >= strlen(msg));
		strcpy(send_msg, name);
		strcat(send_msg, msg);
		write(sock, send_msg, strlen(send_msg));
		if(strcmp(msg, "bye") == 0)
			exit(1);
	}
	free(msg);
	free(send_msg);
	msg = '\0';
	send_msg = '\0';
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
