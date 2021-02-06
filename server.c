#include "clientserver.h"
#define  MAXCLIENT 60
int client_socket[MAXCLIENT];
pid_t pid ;
int master_socket;
void close_sockets() {

    for (int i=0; i<MAXCLIENT; i++) {
        if (client_socket[i]) 
        {
            if (send(client_socket[i], CLOSE_SERVER, 13, 0) < 0)
            {
                perror(CLOSE_SERVER);
            }
            close(client_socket[i]);
        }
    }
    close(master_socket);
    exit(1);
}



int SendClient(int cs) {
	char buff[BUFFSIZE];
	int sizeBuff;
	
	if(sizeBuff=recv(cs,buff,sizeof(buff),0)<0)
	{

		ErrorCloseSock("Receive failed \n",cs);
		return 2;

	}
	else
	{
		if(strncmp(buff, "exit",4) == 0)
		{
			if(send(cs, "Finish handle buffer",21 , 0) <0) 
			{
			    perror("Error sending message");
			    return 1;
			}
			close(cs);
			return 0;
		}
		
		/* Open the command for reading. */
		FILE* fp= popen(buff, "r");
		/* Read the output a line at a time - output it. */
		while (fgets(buff, sizeof(buff), fp) != NULL) {
			if(send(cs, buff, sizeof(buff), 0) <0) 
			{
			    perror("Error sending message");
			    return 1;
			}
		}
		send(cs, "-1", 3, 0);
		/* close */
		fclose(fp);
		printf("end \n");
		return 1;
	}
}

void *HandeClient(void* clientsock) {	

		pid = fork();
		if (!pid) {
			int isSend=SendClient(*((int *)clientsock));
			if(isSend==0)
				return 0;
			else if(isSend==2)
				return 0;
		}
		else if(pid<0)
		{
			perror ("Couldn't fork \n");
			return 0;
		}


}

int main(int argc, char const *argv[])
{
	pthread_t tid[MAXCLIENT+1];
	struct sockaddr_in s = {0};
	master_socket=CreateAndConstructSocket(&s);

	// this will allow code to handle SIGPIPE instead of crashing
	signal(SIGINT, close_sockets); 
	
	//Bind the socket to the server using the address
	if (bind(master_socket, (struct sockaddr*)&s, sizeof(s)) < 0)
		ErrorHandler("Network Error: cannot bind socket \n",master_socket);
	//Listen the socket to the server using the address
	if (listen(master_socket, QUEUE_LEN) < 0)
		ErrorHandler("Error with listen to socket \n",master_socket);
		
	while(1)
	{
		struct sockaddr_in  clientIn;
		unsigned int clientlen = sizeof(clientIn);
		for (int s=0;s<MAXCLIENT; s++) {
   			if(client_socket[s]==0)
   			{
   				int t=s;
   				//Accept the socket to from client
				if((client_socket[t] = accept(master_socket,(struct sockaddr *) &clientIn,(socklen_t*)&clientlen))<0){
					perror("Could not accept this server \n");
				}
				else
				{
					printf("Accept the socket to from client \n");
					if( pthread_create(&(tid[t]), NULL, HandeClient, client_socket+t )!= 0 )
					   perror("Failed to create thread \n");
					pthread_join(tid[t],NULL);
					client_socket[t]==0;
				}
   			}	
                }

	}
	
	return 0;

}
