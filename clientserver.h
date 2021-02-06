#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h> 
#include <signal.h>
#define BUFFSIZE 1024
#define QUEUE_LEN 20
#define PORT 0x0da2
#define IP_ADDR 0x7f000001
#define MESSAGE_END "-1"
#define CLOSE_SERVER "CLOSE_SERVER"
#define CLOSE_CLIENT "CLOSE_CLIENT"
#ifndef CLIENTSERVER_H
#define CLIENTSERVER_H

void ErrorHandler(char *mess, int sock) 
{
    perror(mess);
    if(sock>0)
    	close(sock);
    exit(1);
}
void ErrorCloseSock(char *mess, int sock)
{
    perror(mess);
    close(sock);
}
int CreateAndConstructSocket(struct sockaddr_in* s )
{
	int sock;
	//create a master socket
	if((sock = socket(AF_INET, SOCK_STREAM,0))<0)
		ErrorHandler("Failed to create socket \n",sock);
	/* Construct the server sockaddr_in structure */
	s->sin_family = AF_INET;
	s->sin_port = htons(PORT);
	s->sin_addr.s_addr = htonl(IP_ADDR);
	return sock;
}

#endif





	
	
