#include <stdio.h> 
#include <string.h>  
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>  
#include <arpa/inet.h>  
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> 
#include <time.h> 
#include <fcntl.h>
#include <string.h>	

#define TRUE 1 
#define FALSE 0 
#define PORT 1500
#define BUFFER 1024
#define DEVICE_NODE "/dev/sensor_device"
	
void error(char *msg)
{
	perror(msg);
	exit(1);
}

int convert_ChartoInt(char *c)
{
         int i=0,s=0,a[10];
         int len = strlen(c);
         for(i=0 ; i<len ; i++)
         {
                 a[i] = c[i] - '0';
         }
	 for(i=0 ; i<len ; i++)
		s = a[i] + s*10;
         return s;
}



int open_sensor_dev()
{
	int fd = open(DEVICE_NODE, O_RDWR);
	if( fd < 0 )
	{
		printf("Can not open the device file %d\n",fd);
		exit(1);
	}
	return fd;
}

void close_sensor_dev(int fd)
{
	close(fd);
}

void read_data(char *msg)
{
          int count=0,fd = -1,ret = 0, humi1=0,humi2 = 0, temp1=0, temp2=0;
          char user_buf[BUFFER];
          fd = open_sensor_dev();
          ret = read(fd, user_buf, BUFFER);
          close_sensor_dev(fd);
          humi1 = user_buf[0];
	  //humi2 = user_buf[1];
          temp1 = user_buf[2];
	  //temp2 = user_buf[3];
          if (ret < 0)
		printf("Could not read a message from %s\n", DEVICE_NODE);
          else
          {
               	if(humi1>0 && temp1>0)
		{
			msg[0] = humi1;
			//msg[1] = humi2;
			msg[1] = temp1;
			//msg[3] = temp2;
		}
          }
 }


int main(int argc , char *argv[]) 
{ 
	int opt = TRUE; 
	int  addrlen , client_socket[30] , max_clients = 30 , activity, i , valread , sd; 
	int max_sd,master_socket, new_socket; 
	struct sockaddr_in address;
	time_t rawtime;
   	struct tm *info;
	
	/* Set of socket file descriptor */ 
	fd_set readfds;
	
	for (i = 0; i < max_clients; i++) 
	{ 
		client_socket[i] = 0; 
	} 
	
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) < 0)
		error("Couldn't create socket");
			 
	/* assign address */ 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
	
	setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT , &opt, sizeof(opt));

	/* socket was create will bind with assigned address */	
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0 ) 
		error("Couldn't bind the address");
	printf("Listening on port %d... \n", PORT); 
	/* queue for socket */	
	if (listen(master_socket, 10) < 0) 
		error("Couldn't listen to the server");
		 
	addrlen = sizeof(address); 
	puts("Waiting for connections ..."); 
	max_sd = master_socket;
		
	while(TRUE) 
	{
		  
		FD_ZERO(&readfds); 
	
		/*add master socket to set */ 
		FD_SET(master_socket, &readfds); 
		for (i = 0; i < max_clients; i++) 
		{ 
			sd = client_socket[i];

		 	if(sd > 0) 
				FD_SET( sd , &readfds); 
					 
			if(sd > max_sd) 
				max_sd = sd;
		}
	
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL); 
		if ((activity < 0) && (errno!=EINTR)) 
		{ 
			puts("select error");
		} 
			
		/* If something happened on the master socket , then its an incoming connection */ 
		if (FD_ISSET(master_socket, &readfds)) 
		{ 
			if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) 
				error("Connection error");
			 
			printf("Connected to client with fd: %d at : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 
		
			/* Welcome 
			char *welcome = "Welcome to my server\n";
			if( send(new_socket, welcome, strlen(welcome), 0) != strlen(welcome) ) 
				perror("send"); 
			*/
			puts("Welcome message sent successfully"); 
				
			for (i = 0; i < max_clients; i++) 
			{ 
				

				if( client_socket[i] == 0 ) 
				{ 
					client_socket[i] = new_socket; 
					printf("Adding to list of sockets as %d\n" , i); 		
					
					break; 
				}
			} 
		} 
			
		/* else its some IO operation on some other socket  */
		for (i = 0; i < max_clients; i++) 
		{ 
			sd = client_socket[i];
			if (FD_ISSET( sd , &readfds)) 
			{ 
				/* Check if it was for closing , and also read the incoming message */
				char *buffer = (char*)malloc(BUFFER * sizeof(char));
				//char *buffer_sensor = (char*)malloc(BUFFER * sizeof(char));
				if ((valread = read( sd , buffer, 1024)) == 0) 
				{ 
					/* Somebody disconnected , get his details and print */ 
					getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen); 
					printf("Host disconnected , ip %s , port %d \n" ,inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
					close( sd ); 
					client_socket[i] = 0; 
				} 
					
				/*  send the message to client */
				else
				{
					char *buffer_sensor = (char*)malloc(BUFFER * sizeof(char));	
					char *moment = (char*)malloc(BUFFER* sizeof(char));	
					read_data(buffer_sensor);
					time(&rawtime);
   					info = localtime( &rawtime );
					strcpy(moment, asctime(info));
					buffer[valread] = '\0';
					/* read data from sensor */
					strcat(moment, buffer_sensor);
					/*
					puts(moment);
					FILE *flog = fopen("chat_log.txt","a+");
					fprintf(flog, "%s\n", moment);
					fclose(flog); 
					*/	
					send(client_socket[i] , moment, strlen(moment) , 0 );
					free(buffer);
					buffer = '\0';
					free(buffer_sensor);
					buffer_sensor = '\0';
					free(moment);
					moment = '\0';
				} 	 
			} 
		} 
	} 
	return 0; 
} 
