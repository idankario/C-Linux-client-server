#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#define BUFFSIZE 100
#define PORT 0x0da2
#define IP_ADDR 0x7f000001
#include <pthread.h> 

void ErrorHandler(char *mess) 
{
    	perror(mess); 
	exit(1);
}
void sendMsg(char* s,int sock)
{	
	int echolen = strlen(s);
	if(send(sock, s, echolen, 0) != echolen) 
		ErrorHandler("Send failed");
	
}
void checkExit(char* s,int sock,char* buffer)
{
	if(strncmp(s, "exit", 4)== 0)
	{
		sendMsg(s,sock);
		recv(sock, buffer, 1024, 0);
		
		if(recv(sock, buffer, 1024, 0)<0)
		{
			printf("Receive failed");
			exit(1);
		}
		printf( "Message from server: %s \n",buffer);
		exit(0);
	}
}

void *createSocket(void *sock)
{
	int s=*((int *)sock);
	while(1){
		char strF[BUFFSIZE],buffer[BUFFSIZE];
		if(fgets(strF,BUFFSIZE,stdin)==0)
			ErrorHandler("Send failed to long");
		
		strF[strlen(strF)-1]='\0'; //fgets doesn't automatically discard '\n'
		/*  Exit client; */
		checkExit(strF,s, strF);
		
		/*  send() from client; */
		sendMsg(strF,s);
		
		/*  getMessage() from server; */
		if(recv(s, buffer, BUFFSIZE, 0)<0)
			ErrorHandler("Receive failed");
	
		/* Receive the word back from the server */
		printf( "Message from server: %s",buffer);
	}
}

int main(int argc, char *argv[]){
	int i = -1;
	int size=50;
	pthread_t tid[size];
	
	/* Create socket */
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock< 0) 
		ErrorHandler("Failed to create socket\n");

	/* Construct the server sockaddr_in structure */
	struct sockaddr_in s = {0};
	s.sin_family = AF_INET;
	s.sin_port = htons(PORT);
	s.sin_addr.s_addr = htonl(IP_ADDR);

	//Connect the socket to the server using the address
	if (connect(sock, (struct sockaddr*)&s, sizeof(s)) < 0)
		ErrorHandler("Failed to connect with server");

	printf("Successfully connected.\n");
        
	if( pthread_create(&(tid[++i]), NULL, createSocket, &sock) == 0 )
	{
		pthread_join(tid[i],NULL);
		i--;	   
	}
	else
	{
		ErrorHandler("Failed to create thread\n");
		return 1;
	}

	return 0;
}
