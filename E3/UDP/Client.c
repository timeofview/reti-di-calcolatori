#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define LINE_LENGTH 256

int main(int argc, char **argv) {
	struct hostent *host;
	struct sockaddr_in clientaddr, servaddr;
	int port, sd, num1, ok, ris;
	char nomeFile[LINE_LENGTH];
	char c;
	int fd, len;
	struct timeval timeout;
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;

	if (argc != 3) {
		printf("Error:%s serverAddress serverPort\n", argv[0]);
		exit(1);
	}

	//controllo argometi PORT
	num1 = 0;
	while (argv[2][num1] != '\0') {
		if (argv[2][num1] < '0' || argv[2][num1] > '9') {
			printf("Incorrect Number Port : it's not a integer ");
			exit(1);
		}
		num1++;
	}
	port = atoi(argv[2]);
	if (port < 1024 || port > 65535) {
		printf("%s  : incorrect port \n", argv[2]);
		exit(2);
	}

	//inizializzo indirizzo client
	memset((char*) &clientaddr, 0, sizeof(struct sockaddr_in));
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_addr.s_addr = INADDR_ANY;

	clientaddr.sin_port = 0; //mi lego a un qualsiasi indirizzo libero ( non funziona dappertutto)

	//inizializzo indirizzo server
	memset((char*) &servaddr, 0, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	host = gethostbyname(argv[1]);

	//controllo argomenti HOST
	if (host == NULL) {
		printf("%s  : host not found", argv[1]);
		exit(2);
	}

	servaddr.sin_addr.s_addr = ((struct in_addr*) (host->h_addr))->s_addr;
	servaddr.sin_port = htons(port);

	//creazione socket
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd < 0) {
		perror("socket opening");
		exit(1);
	}
	printf("Client: socket created =%d\n", sd);

	//bind socket
	if (bind(sd, (struct sockaddr*) &clientaddr, sizeof(clientaddr)) < 0) {
		perror("error during the binding of socket");
		exit(2);
	}
	printf("Client: bind socket ok, at port %d\n", clientaddr.sin_port);



	    if (setsockopt (sd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
	                sizeof(timeout)) < 0)
	        error("setsockopt failed\n");

	    if (setsockopt (sd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
	                sizeof(timeout)) < 0)
	        error("setsockopt failed\n");


	// INIZIO BODY DEL CLIENT
	printf("Path del file da elaborare? ");
	while(scanf("%s", nomeFile) != EOF){ //CORREGGERE ARCHITETTURA CICLO


		//operazioni
		if ((sendto(sd, nomeFile, sizeof(nomeFile), 0,
				(struct sockaddr*) &servaddr, sizeof(servaddr))) == -1) {
			printf("Sending file %s....ERROR occurred ", nomeFile);
			continue;
		}
		printf("Transfer of nameFile %s completed\n", nomeFile);
		//ricevere intero rappresentante la lunghezza della parola piu lunga nel file

		//ricezione intero
		len = sizeof(servaddr);
		if (recvfrom(sd, &ris, sizeof(ris), 0, (struct sockaddr*) &servaddr,
				&len) < 0) {
			perror("recvfrom");
			continue;
		}

		printf("Operation result : %d\n", (int) ntohl(ris));

		printf("Path del file da elaborare? ");
	}

	//CHIUSURA DI TUTTO
	close(sd);
	printf("Client terminated\n");
	exit(0);
}
