#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>


#define STR_MAX 256
#define PORT_MIN 1024
#define PORT_MAX 65535


int main(int argc, char *argv[]) {

	// Varibili programma 
	int i, port, serverSize, reply;
	// Socket descriptors 
	int sd;
	// Nome del file da inviare
	char fileName[STR_MAX];
	// HostEntity serverHost
	struct hostent *serverHost;
	// Indirizzi Socket
    struct sockaddr_in clientAddr, serverAddr;

	// Struct timeout, utilizzata per interrompere la connessione se scade
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

	// Controllo numero argomenti
	if (argc != 3) {
        printf("Error: incorrect argument number!\nProgram usage: %s ServerAddress ServerPort\n", argv[0]);
        exit(1);
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
        fprintf(stderr, "Error: port is exceeding the 65K limit!\n");
    }
	
	
	// Setto a 0 il contenuto della variabile clientAddr
    memset((char*) &clientAddr, 0, sizeof(struct sockaddr_in));
	
	
	// Inizializzazione parametri indirizzo Socket
	clientAddr.sin_family = AF_INET; // Famiglia protocolli Internet
    clientAddr.sin_addr.s_addr = INADDR_ANY; // Indica che si collega ad una qualsiasi interfaccia di rete
    clientAddr.sin_port = 0; // Indica la porta  alla quale connettermi
	
	
	// Setto a 0 il contenuto della variabile serverAddr
	memset((char*) &serverAddr, 0, sizeof(struct sockaddr_in));

    // Ottengo il serverHost attraverso gethostbyname
    serverHost = gethostbyname(argv[1]);
    if(serverHost == NULL){
		fprintf(stderr, "Error: cannot resolve host %s!\n", argv[1]);
		
		exit(EXIT_FAILURE); 
	}
	
	// Setto i parametri di indirizzo Socket 
	serverAddr.sin_family = AF_INET; // Famiglia protocolli Internet
    serverAddr.sin_addr.s_addr = ((struct in_addr*) (serverHost->h_addr))->s_addr; // Ottengo l'indirizzo
    serverAddr.sin_port = htons(port); // Assegno la parta in base a ciò che l'utente mi ha inserito

	// Creazione Socket
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0) {
        fprintf(stderr, "Error: cannot create socket!\n");
        
        exit(EXIT_FAILURE);
    }
	
	// Collego la socket sd al clientAddr
    if(bind(sd, (struct sockaddr*) &clientAddr, sizeof(struct sockaddr_in)) < 0) {
        fprintf(stderr, "Error: cannot bind socket!\n");
        
        exit(EXIT_FAILURE);
    }
	
	
	// Setto il tempo di ricezione nelle opzioni della Socket
    if (setsockopt (sd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval)) < 0) {
        fprintf(stderr, "Error: setsockopt failed\n");
	}
    if (setsockopt (sd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval)) < 0) {
        fprintf(stderr, "Error: setsockopt failed\n");
	}
	

	
    printf("Enter the file path [EOF to end]: ");
    
    // Ciclo principale
    while(scanf("%s", fileName) != EOF) {
		
		// Invio il nome del file
        if ((sendto(sd, fileName, sizeof(fileName), 0, (struct sockaddr*) &serverAddr, sizeof(struct sockaddr_in))) < 0) {
            printf("Error: cannot send file %s\n", fileName);
            
            continue;
        }
        
        printf("File '%s' has been sent to the Server!\n", fileName);
        
        // Ottengo la dimensione del Server
        serverSize = sizeof(serverAddr);
        
        // Ricezione della riga più lunga presente nel File
        if (recvfrom(sd, &reply, sizeof(int), 0, (struct sockaddr*) &serverAddr, &serverSize) < 0) {
            fprintf(stderr, "Error: cannot receive the maximum string length from Server!\n");
            
            continue;
        }
        
        reply = (int)ntohl(reply);
        
        if(reply < 0)
			fprintf(stderr, "Error: file '%s' not found in the Server's FileSystem!\n", fileName);
		else
			printf("Maximum string length from Server: %d\n", reply);


        printf("Enter the file path: ");

	}
	
	// Chiusura comunicazione
    close(sd);
    printf("End of comunication!\n");


	return 0;

}
