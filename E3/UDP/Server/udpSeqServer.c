#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define MAX_NAME_LENGHT 256
#define BUF_SIZE 128

int main(int argc, char **argv) {

	int logicalSize, fileSize, bufSizeReal = BUF_SIZE, i;
	int sd, port, len, fd, ris;
	const int on = 1;
	struct sockaddr_in cliaddr, servaddr;
	struct hostent *clienthost;
	char fileName[MAX_NAME_LENGHT];
	int result, count;
	char buf[BUF_SIZE];

	//Controllo argomenti
	if (argc != 2) {
		printf("Error Usage : %s port \n", argv[0]);
		exit(1);
	}

	printf("NUMERO ARGC CONTROLLATO\n");

	len = 0;
	while (argv[1][len] != '\0') {
		if (argv[1][len] < '0' || argv[1][len] > '9') {
			printf("Argument passed is not a integer");
			exit(1);
		}
		len++;
	}

	printf("PORT CONTROLLATA\n");

	port = atoi(argv[1]);
	if (port < 1024 || port > 65535) {
		printf("Error: incorrect number port ");
		exit(1);
	}

	printf("ARGOMENTI CONTROLLATI\n");
	//inizializzo indirizzo server
	memset((char*) &servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);

	//creazione  socket
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd < 0) {
		perror("Error creating socket ");
		exit(1);
	}
	printf("Server: socket created, sd = %d\n", sd);

	//opzioni socket
	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		perror("set opzions socket ");
		exit(1);
	}
	printf("Server: Socket Options setted ok\n");

	// connessione socket
	if (bind(sd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
		perror("bind socket ");
		exit(1);
	}
	printf("Server: bind socket ok\n");

	//INZIO BODY
	while (1) {

		//ricevo nomeFile
		len = sizeof(struct sockaddr_in);
		if (recvfrom(sd, &fileName, sizeof(fileName), 0, (struct sockaddr*) &cliaddr, &len) < 0) {
			perror("recvfrom");
			continue;
		}

		printf("Operazione richiesta sul file %s \n", fileName);
		clienthost = gethostbyaddr((char*) &cliaddr.sin_addr,sizeof(cliaddr.sin_addr), AF_INET);
		if (clienthost == NULL)
			printf("client host information not found\n");
		else
			printf("Operation required from: %s %i\n", clienthost->h_name,
					(unsigned) ntohs(cliaddr.sin_port));

		//trovare lunghezza della parola piu lunga
		fd = open(fileName, O_RDONLY);
		if (fd < 0) {
			printf("Impossible open %s\n", fileName);
			result = -1;
		} else {

			fileSize = lseek(fd, 0, SEEK_END);
			lseek(fd, 0, SEEK_SET);
			if (fileSize < bufSizeReal) {
				bufSizeReal = fileSize;
			}
			
			count=0;
			result=0;
			while (read(fd, &buf, bufSizeReal * sizeof(char)) > 0) {

				logicalSize = sizeof(buf) / sizeof(buf[0]);

				for (i = 0; i < logicalSize; i++) {

					if (buf[i] == ' ' || buf[i] == '\n' || buf[i] == '\t') {
						if (count > result) {
							result = count;
						}
						count = 0;

					} else {
						count++;
					}
				}
				// Flush array
				memset(buf, 0, bufSizeReal * (sizeof buf[0]));
			}

		}
		close(fd);
		printf("RESULT: %d \n ", result);
		result = htonl(result);
		if (sendto(sd, &result, sizeof(result), 0, (struct sockaddr*) &cliaddr,
				len) < 0) {
			perror("ERROR sendto");
			continue;
		}
	}
	return EXIT_SUCCESS;
}
