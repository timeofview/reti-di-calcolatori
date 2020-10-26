#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>


#define MAXBACKLOG 10
#define DIM_BUFF 1024

void child_handler(int signo){
	int state;
	printf("Waiting a child\n");
	wait(&state);
	printf("Completed 1 transaction\n");
}

int main(int argc, char** argv){
	int listen_sd, conn_sdn, port, len, n_line, line_counter, nread;
	const int on = 1;
	struct sockaddr_in cliaddr, servaddr;
	struct hostent *host;
	char buff[DIM_BUFF], buffToSend[DIM_BUFF];

	//check arguments
	if(argc!=2){ 
		printf("Usage: server port"); 
		exit(1); 
	}
	else {
		port = atoi(argv[1]); //check port?
	}
	memset ((char *)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;

	//address init
	servaddr.sin_addr.s_addr = INADDR_ANY;

	//port parse and init
	servaddr.sin_port = htons(port);

	//socket creation and binding
	listen_sd=socket(AF_INET, SOCK_STREAM, 0);
	
	//check descriptor
	if(listen_sd < 0){
		perror("Socket creation"); 
		exit(2);
	}
	
	//setting socket
	if(setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR,&on, sizeof(on))<0){
		perror("Socket has not been set"); 
		exit(3);
	}

	//binding listening socket
	if(bind(listen_sd,(struct sockaddr *) &servaddr,sizeof(servaddr))<0){
		perror("Socket has not been bound"); 
		exit(4);
	}

	//listening queue
	if (listen(listen_sd, MAXBACKLOG)<0){
		perror("Listen queue error"); 
		exit(5);
	}

	signal(SIGCHLD, child_handler);

	//mainloop
	while(1){

		//if not accepted
		if((conn_sdn = accept(listen_sd,(struct sockaddr *)&cliaddr,&len))<0){
			if (errno==EINTR){ 	 //tocheck
				perror("Forzo la continuazione della accept");
				continue;
			}
			else{
				exit(6);
			}
		}
        int pid = fork();
		if(pid<0){
		    perror("Errore nella fork\n");
		    exit(7);
		}
		else if (pid==0){

			line_counter = 1;

			//close fds
			close (listen_sd);

			//read row to delete
			read(conn_sdn,&n_line,sizeof(int));
            int offset;
			//main_child_loop
			while((nread=read(conn_sdn,buff,DIM_BUFF))>0){
			    offset=0;
				for(int i = 0; i < nread; i++){
					if(buff[i]=='\n'){
						line_counter++;
					}
					if(n_line!=line_counter){
						buffToSend[offset]=buff[i];
						offset++;
					}
				}
				//Da aggiustare
				buffToSend[offset+1]='\0';
				write(conn_sdn,buffToSend,offset+1);
			}
			shutdown(conn_sdn,2);
			exit(0);
		} else{
            close(conn_sdn);
		}
	}
}
