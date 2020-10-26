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


#define STR_MAX 256
#define BUF_SIZE 128
#define PORT_MIN 1024
#define PORT_MAX 65535


int main(int argc, char *argv[]) {


    // Variabili legate al programma
    int i, port, enable = 1, clientSize, pid, fd, result,  fileSize, bufSizeReal = BUF_SIZE, nRead, count;
    char fileName[STR_MAX];
    // Socket descriptors
    int sd;
    // Socket Addresses
    struct sockaddr_in clientAddr, serverAddr;
    // Socket entities
    struct hostent *clientHost;
    // buffer
    char buf[BUF_SIZE];


    // Controllo numero argomenti
    if(argc != 2) {
        fprintf(stderr, "Error: incorrect number of arguments!\nProgram usage: %s Port\n", argv[0]);

        exit(EXIT_FAILURE);

    }

    // Controllo che l'argomento "Port" sia composto da soli numeri
    for(i=0; i<strlen(argv[1]); i++) {
        if (argv[1][i] < '0' || argv[1][i] > '9') {
            fprintf(stderr, "Error: port argument '%s' is not an only-digit!\n", argv[1]);

            exit(EXIT_FAILURE);
        }
    }

    // Converto la porta in intero e controllo che non super il limite
    port = atoi(argv[1]);
    if(port < PORT_MIN || port > PORT_MAX) {
        fprintf(stderr, "Error: port is exceeding the 65K limit!\n");
    }

    // Setto a 0 la serverAddr
    memset((char*) &serverAddr, 0, sizeof(serverAddr));


    // Setto la famiglia di indirizzi (AF_INET perché devo comunicare via internet)
    serverAddr.sin_family = AF_INET;
    // Setto l'indirizzo su cui ascolto (scelgo ogni interfaccia disponibile perché sono il Server)
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    // Setto la porta a cui collegarmi (htons perché converto byte host in byte network)
    serverAddr.sin_port = htons(port);


    // Apertura Socket con relative peroprietà
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        fprintf(stderr, "Error: cannot create Socket!\n");

        exit(EXIT_FAILURE);
    }

    printf("Server: socket created successfuly!\n");


    // Setto opzioni della Socket (enable è stata dichiarata come variabile a parte)
    if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        fprintf(stderr, "Error: cannot set the Socket options! 2\n");

        exit(EXIT_FAILURE);
    }


    // Collego la Socket alla struct "server"
    if(bind(sd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        fprintf(stderr, "Error: 'bind' failed!\n");

        exit(EXIT_FAILURE);
    }


    printf("\n++ Waiting for Client connections ++\n\n");


    // Ciclo principale
    while(1) {

        // Il padre esegue questo codice...

        // Ottengo la dimensione dell'indirizzo del Client
        clientSize = sizeof(struct sockaddr_in);

        // Ricevo il nome del file tramite la primitiva
        // ssize_t recvfrom(int sd, void *message, size_t length, int flags, struct sockaddr *src_addr, socklen_t *src_len);
        if (recvfrom(sd, &fileName, sizeof(fileName), 0, (struct sockaddr*) &clientAddr, &clientSize) < 0) {
            fprintf(stderr, "Error: 'recvfrom' failed!\n");

            // Vado al ciclo successivo
            continue;
            
        } else {

            // Codice  generazione figli

            if((pid = fork()) == 0) { // Figlio
				
				printf("A new fork has been done!\n");
				
                // Ottengo le informazioni del Client che sta comunicando col Server tramite la primitiva
                // struct hostent *gethostbyaddr(const char *addr, int len, int type);
                clientHost = gethostbyaddr((char*) &clientAddr.sin_addr, sizeof(clientAddr.sin_addr), AF_INET);
                if(clientHost == NULL) {
                    fprintf(stderr, "Error: client host information not found!\n");
                } else {
                    printf("An operation is required  the Client: %s %i\n", clientHost->h_name, ntohs(clientAddr.sin_port));
                }


                // Inizia la ciccia qui, trovo la parola più lunga contenuta nel file ricevuto dal Client

                // Verifico se il fileName ricevuto dal Client esiste nel FileSystem del Server
                if((fd = open(fileName, O_RDONLY)) < 0) {
                    fprintf(stderr, "Error: cannot open file '%s'!\n", fileName);

                    // Result contiene l'esito dell'operazione:
                    //
                    // Result = -1 --> file non esistente nel FileSystem del Server :(
                    // Result = 0 --> file esistente nel FileSystem del Server :)
                    //
                    result = -1;

                }else {

					printf("File %s opened correctly!\n", fileName);

                    // Ottengo quanti caratteri sono nel file inviato dal Client
                    fileSize = lseek(fd, 0, SEEK_END);


                    // Porto il file pointer all'inizio del file (poiché con lseek me l'ha portato alla fine)
                    lseek(fd, 0, SEEK_SET);


                    // Ottengo la bufSizeReal
                    if (fileSize < bufSizeReal) {
                        bufSizeReal = fileSize;
                    }

                    // Count
                    count = 0;
                    result = 0;

                    // Ciclo lettura file
                    while((nRead=read(fd, &buf, bufSizeReal * sizeof(char))) > 0) {

                        for (i=0; i<nRead; i++) {

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


                    } // fine while

                    // Chiudo il file
                    close(fd);

                } // fine else

                // Stampo a video la lunghezza della parola più lunga
                if(result == -1)
                    fprintf(stderr, "Error: file %s not found!\n", fileName);
                else
                    printf("Length of the longest word: %d \n", result);

                // Result diventa uguale
                result = htonl(result);

                // Invio al client la lunghezza della parola più lunga
                if(sendto(sd, &result, sizeof(result), 0, (struct sockaddr*) &clientAddr, clientSize) < 0) {
                    fprintf(stderr, "Error: 'sendto' failed!\n");

                    continue;
                }


                // Chiudo la Socket
                close(sd);

                exit(EXIT_SUCCESS);
            }

            else if(pid > 0) { // Padre

                continue;
            }

            else { // Errore fork

                // Codice errore

                fprintf(stderr, "Error: cannot execute a fork!\n");

                continue;
            }

        } // fine else

    } // fine while(1)


    return 0;

}
