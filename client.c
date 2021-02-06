
#include "clientserver.h"

int sock ;
void close_socket() {

    if (sock)
    {
        close(sock);
    }
    exit(1);
}
void sendMsg(char* s)
{	
	int echolen = strlen(s);
	if(send(sock, s, echolen, 0) != echolen) 
		ErrorHandler("Send failed \n",sock);
	
}
int isExit(char* s)
{
        if (strncmp(s, "exit",4) == 0)
     	{	
		sendMsg(s);
		if(recv(sock, s, BUFFSIZE, 0)<0)
		{
			ErrorCloseSock("Receive massage from server failed \n",sock);
			return 1;
		}
		printf( "Message from server: %s \n",s);
		return 1;
	}
	else
		return 0;
}

void *createSocket()
{
	char strF[BUFFSIZE],buffer[BUFFSIZE];
	fflush(stdin);
	if(fgets(strF,BUFFSIZE,stdin)==0)
		ErrorHandler("Command failed to long \n",sock);
	/*  Exit client; */
	if(isExit(strF))
		return 0;
	/*  send() from client; */
	sendMsg(strF);
	
	/*  getMessage() from server; */
	int recvSize = recv(sock, buffer, BUFFSIZE, 0);
	while (strcmp(buffer, MESSAGE_END) > 0) {
		if (!strcmp(buffer, CLOSE_SERVER)) {
		    close(sock);
		    return 0;
		}
		printf("%s", buffer);
		recvSize = recv(sock, buffer, BUFFSIZE, 0);
	}
	if(recvSize)
		ErrorHandler("Receive failed \n",sock);
	/* Receive end */
	printf( "Message from server end  \n");
}

int main(int argc, char *argv[]){
	int i = -1;
	int size=50;
	pthread_t tid[size];
	// this will allow code to handle SIGPIPE instead of crashing
	signal(SIGINT, close_socket);
	/* Construct the server sockaddr_in structure */
	struct sockaddr_in s = {0};
	/* Create socket */
	sock = CreateAndConstructSocket(&s);
	//Connect the socket to the server using the address
	if (connect(sock, (struct sockaddr*)&s, sizeof(s)) < 0)
		ErrorHandler("Failed to connect with server \n",sock);
	printf("Successfully connected.\n");
	if( pthread_create(&(tid[++i]), NULL, createSocket, &sock) == 0 )
	{
		pthread_join(tid[i],NULL);
		i--;	   
		close(sock);
	}
	else
	{
		ErrorHandler("Failed to create thread \n",sock);
		return 1;
	}

	return 0;
}
