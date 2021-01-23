#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#define BUFFSIZE 100
#define QUEUE_LEN 20
#define PORT 0x0da2
#define IP_ADDR 0x7f000001
#include <pthread.h> 

void Die(char *mess) 
{
    perror(mess);
    exit(1);
}
void ErrorHandler(char *mess, int sock) 
{
    perror(mess);
    close(sock);
}


void sendMsg(char* s,int sock)
{	
	int echolen = strlen(s);
	if(send(sock, s, echolen, 0) != echolen) 
	{
		ErrorHandler("Send failed",sock);
	}
	
}
void* HandeClient(void* clientsock) {
	while(1)
	{
		char buff[BUFFSIZE];
		int rval;
		int cs=*((int *)clientsock);
		if(rval=recv(cs,buff,sizeof(buff),0)<0)
		{
			perror("Receive failed");
		}
		else
		{	
			printf( "Message from client: %s \n",buff);
			if(send(cs, buff, sizeof(buff), 0) <0) 
				ErrorHandler("Send failed",cs);
			if(strncmp(buff, "exit", 4) == 0)
			{
				close(cs);
				return 0;
			}
		}
	}
}


int main(int argc, char const *argv[])
{
	int serversock, clientsock;
	// this will allow code to handle SIGPIPE instead of crashing
	signal(SIGPIPE, SIG_IGN); 
	pthread_t tid[60];
	/* Create socket */
	if((serversock = socket(AF_INET, SOCK_STREAM,0))<0){
		Die("Failed to create socket");
		return 1;
	}
	/* Construct the server sockaddr_in structure */
	struct sockaddr_in s = {0};
	s.sin_family = AF_INET;
	s.sin_port = htons(PORT);
	s.sin_addr.s_addr = htonl(IP_ADDR);
	//Bind the socket to the server using the address
	if (bind(serversock, (struct sockaddr*)&s, sizeof(s)) < 0)
	{
		Die("Network Error: cannot bind socket");
		return 1;
	}
	//Listen the socket to the server using the address
	if (listen(serversock, QUEUE_LEN) < 0)
	{
		Die("Error with listen to socket");
		return 1;
	}
	
    	int i = -1;

	while(1)
	{
		while(i>59)
		{
			sleep(100);
		}
		struct sockaddr_in  clientIn;
		unsigned int clientlen = sizeof(clientIn);
		//Accept the socket to from client
		if((clientsock = accept(serversock,(struct sockaddr *) &clientIn,(socklen_t*)&clientlen))<0){
			perror("Could not accept this server");
		}

		if( pthread_create(&(tid[i++]), NULL, HandeClient, &clientsock) != 0 )
		   perror("Failed to create thread\n");
		 
		pthread_join(tid[i],NULL);
		i--;
		sleep(100);
	}
	
	
	return 0;

}
