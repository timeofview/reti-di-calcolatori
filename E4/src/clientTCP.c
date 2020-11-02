#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>

#define PORT_MIN 1024
#define PORT_MAX 65535

/*
	Il secondo Client, chiede ciclicamente all’utente il nome del direttorio, invia al server la
	richiesta, riceve la lista di nomi di file remoti che stanno nei direttori contenuti nel direttorio
	specificato,e la stampa a video
*/


#define STR_MAX 256


int main(int argc, char * argv[]) {


    int sd, nRead, port, i;
    char dirName[STR_MAX], serverReply[STR_MAX];
    struct sockaddr_in serverAddr;
    struct hostent *serverHost;
    struct timeval timeout; // Struct timeout, utilizzata per interrompere la connessione se scade
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;



    // Controllo argomenti
    if(argc != 3) {
        fprintf(stderr, "Error: invalid argument number!\nProgram usage: %s Address Port", argv[0]);

        exit(EXIT_FAILURE);
    }


    // Controllo che l'argomento "Port" sia composto da soli numeri
    for(i=0; i<strlen(argv[2]); i++) {
        if (argv[2][i] < '0' || argv[2][i] > '9') {
            fprintf(stderr, "Error: port argument '%s' is not an only-digit!\n", argv[2]);

            exit(EXIT_FAILURE);
        }
    }


    // Controllo che l'argomento "Port" non ecceda il limite massimo di 65K
    // Converto la porta in intero e controllo che non super il limite
    port = atoi(argv[2]);
    if(port < PORT_MIN || port > PORT_MAX) {
		fprintf(stderr, "Error: port needs to be between %d and %d!\n", PORT_MIN, PORT_MAX);
		
		exit(EXIT_FAILURE);
    }

    serverHost = gethostbyname(argv[1]);
    if(serverHost == NULL) {
        perror("Unknown Server error: ");

    }


    // Setto a 0 il contenuto della variabile serverAddr
    memset((char*) &serverAddr, 0, sizeof(struct sockaddr_in));

    // Setto i parametri di indirizzo Socket
    serverAddr.sin_family = AF_INET; // Famiglia protocolli Internet
    serverAddr.sin_addr.s_addr = ((struct in_addr*) (serverHost->h_addr))->s_addr; // Ottengo l'indirizzo
    serverAddr.sin_port = htons(port); // Assegno la parta in base a ciò che l'utente mi ha inserito

    // Chiedo ciclicamente all'untente il nome del direttorio
    printf("Enter directory name [EOF to end]: ");
    while(scanf("%s", dirName) != EOF) {
	
		// Apertura Socket
		if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("Socket error: ");

			exit(EXIT_FAILURE);
		}

		// Setto il tempo di ricezione nelle opzioni della Socket
		if(setsockopt (sd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval)) < 0) {
			perror("Error while setting socket options: ");
		}
		if(setsockopt (sd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval)) < 0) {
			perror("Error while setting socket options: ");
		}


		// Connect
		if(connect(sd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr)) < 0) {
			perror("Connect failed: ");

			exit(EXIT_FAILURE);
		}
		
		
        // Invio della stringa
        if(write(sd, dirName, strlen(dirName)) < 0) {
            perror("Send failed: ");

            continue;
        }

        // Ricevo la lista delle Stringhe
        //while((nRead = recv(sd, serverReply, STR_MAX * sizeof(char), 0)) > 0) {
        while((nRead = read(sd, serverReply, STR_MAX * sizeof(char))) > 0) {

            // Errore receive
            if(nRead < 0) {
                perror("Socket Read failed: ");

                continue;
            }

            printf("%s\n", serverReply);

            // Flusho la stringa
            memset((char*) &serverReply, 0, STR_MAX * sizeof(char));
            
        } // fine while recv
		
		// Chiudo Socket
		close(sd);
        printf("Enter directory name [EOF to end]: ");
    } // fine while


    printf("\n++ End of communication! ++ \n");


    return 0;
}
