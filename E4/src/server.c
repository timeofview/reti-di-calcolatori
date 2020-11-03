#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h> // per le directory
#include <sys/time.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/select.h>


#define DEFAULT_PORT 54321
#define PORT_MIN 1024
#define PORT_MAX 65535
#define STR_MAX 256
#define BACKLOG_MAX 10



// Funzioni
void childHandler(int);


// RequestUDP, indica una richiesta di tipo UDP
typedef struct {
    char file_in[STR_MAX];
    char word[STR_MAX];
} request;


int main(int argc, char *argv[]) {

    int sdUdp, sdListen, sdTcp, port, clientSize, pid, i, j, numfds, cntFound, fdInput, fdOutput, nRead, toRemoveLen;
    const int reuse = 1;

    struct sockaddr_in clientAddr, serverAddr;
    struct hostent *clienthost;
    char buff[STR_MAX];
    char *errorString = "Cannot open Directory!\n";
    fd_set rset;
    request req;
    char dirName[STR_MAX];
    DIR *d;
    DIR *d2;
    struct dirent* dir ; // DirEntity, mi sta ad indicare un'entità di tipo Directory
    struct dirent* dir2 ; // DirEntity, mi sta ad indicare un'entità di tipo Directory


    // Controllo argomenti
    if(argc == 1) {
        port = DEFAULT_PORT;
    } else if(argc != 2) {
        fprintf(stderr, "Error: invalid argument number!\nProgram usage: %s Port[OPTIONAL]", argv[0]);

        exit(EXIT_FAILURE);

    } else {

        // Controllo che l'argomento "Port" sia composto da soli numeri
        for(i=0; i<strlen(argv[1]); i++) {
            if (argv[1][i] < '0' || argv[1][i] > '9') {
                fprintf(stderr, "Error: port argument '%s' is not an only-digit!\n", argv[1]);

                exit(EXIT_FAILURE);
            }
        }


        // Controllo che l'argomento "Port" non ecceda il limite massimo di 65K
        // Converto la porta in intero e controllo che non super il limite
        port = atoi(argv[1]);
        if(port < PORT_MIN || port > PORT_MAX) {
            fprintf(stderr, "Error: port needs to be between %d and %d!\n", PORT_MIN, PORT_MAX);

            exit(EXIT_FAILURE);
        }

    } // fine else


    // Setto a zero l'indirizzo di "serverAddr"
    memset((char *)&serverAddr, 0, sizeof(serverAddr));

    // Inizializzo i parametri dell'indirizzo del Server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);
    //memset(&(serverAddr.sin_zero), '\0', 8);


    // Creazione Socket UDP
    if ((sdUdp = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("UDP Socket creation error: ");
        exit(EXIT_FAILURE);
    }

    // Creazione Socket TCP
    sdListen = socket(AF_INET, SOCK_STREAM, 0);
    if(sdListen < 0) {
        perror("TCP Socket creation error: ");

        exit(EXIT_FAILURE);
    }

    // Opzioni Socket UDP
    if (setsockopt(sdUdp, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("Failed setting UDP socket options: ");

        exit(EXIT_FAILURE);
    }

    // Opzioni Socket TCP
    if(setsockopt(sdListen, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("Failed setting TCP socket options: ");

        exit(EXIT_FAILURE);
    }


    // Binding Socket TCP
    if(bind(sdListen, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        perror("Socket bind failed: ");

        exit(EXIT_FAILURE);
    }

    // Binding Socket UDP
    if (bind(sdUdp, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Socket bind failed: ");
        exit(EXIT_FAILURE);
    }

    // listen della socket tcp
    if (listen(sdListen, BACKLOG_MAX) < 0) {
        perror("Listen failed: ");

        exit(EXIT_FAILURE);
    }

    // Aggancio gestore
    signal(SIGCHLD, childHandler);

    // Setta a zero il set "rset" che contiene altri file descriptors
    FD_ZERO(&rset);

    // The nfds argument specifies the range of file descriptors to be tested. The select() function tests file descriptors in the range of 0 to nfds-1
    // Ottengo il valore della variabile numfds
    if(sdListen > sdUdp) {
        numfds = sdListen + 1;
    } else {
        numfds = sdUdp + 1;
    }


    printf("++ ...Waiting for Client connections... ++\n\n");

    clientSize = sizeof(struct sockaddr_in);

    // Ciclo principale
    while(1) {

        // Metto in rset i due socket descriptors
        FD_SET(sdListen, &rset);
        FD_SET(sdUdp, &rset);

        // Si blocca e seleziona la Socket che è pronta
        if(select(numfds, &rset, NULL, NULL, NULL) < 0) {

            // Se il motivo per la quale è uscito
            if(errno == EINTR) {
                continue;
            } else {
                perror("Select error: ");
                exit(EXIT_FAILURE);
            }

        }

        // Dimensione cliente

        // Inizio UDP
        // Controllo se la socket sdUdp ha dei dati disponibili
        if(FD_ISSET(sdUdp, &rset)) {

            if (recvfrom(sdUdp, &req, sizeof(req), 0, (struct sockaddr *) &clientAddr, &clientSize) < 0) {
                perror("Recvfrom error ");
                continue;
            }

            clienthost = gethostbyaddr((char *)&clientAddr.sin_addr, sizeof(clientAddr.sin_addr), AF_INET);
            if (clienthost == NULL) {
                printf("Client host information not found\n");
            }
            else {
                printf("Deleting word '%s' from file '%s'...\n", req.word, req.file_in);
            }

            // Open file and separting FAILURE from SUCCESS
            if ((fdInput = open(req.file_in, O_RDONLY)) < 0) {
                perror("Cannot open file: ");

                cntFound = -1;
            } else {

                char fileOutName[strlen(req.file_in) + 4];


                sprintf(fileOutName, "%s.tmp", req.file_in);
                printf("File Temp: %s\n", fileOutName);


                fdOutput = open(fileOutName, O_WRONLY | O_CREAT, 0777);


                // Length of word to remove
                toRemoveLen = strlen(req.word);

                cntFound = 0;
                while((nRead = read(fdInput, buff, STR_MAX)) > 0) {

                    for(i=0; i<nRead; i++) {

                        if(buff[i] == req.word[0]) {

                            for(j=0; j<toRemoveLen; j++) {

                                if(buff[j+i] != req.word[j]) {

                                    break;
                                }
                            }

                            if(j == toRemoveLen && (buff[j+i] == ' ' || buff[j+i] == '\t' || buff[j+i] == '\n')) {
                                i += toRemoveLen;
                                cntFound++;
                            }
                        }
                        write(fdOutput, &buff[i], sizeof(char));
                    }

                }

                // Closing file out and overwriting
                close(fdOutput);
                remove(req.file_in);
                rename(fileOutName, req.file_in);
            }

            // Closing file in
            close(fdInput);
            printf("Sending %d to Client...\n", cntFound);

            // Sending result to Client
            if(sendto(sdUdp, &cntFound, sizeof(cntFound), 0, (struct sockaddr *)&clientAddr, sizeof(struct sockaddr))<0) {
                perror("Last SendTo failed:  ");

                continue;
            }

        } // FINE UDP

        // Inizio TCP
        if(FD_ISSET(sdListen, &rset)) {

            if((sdTcp = accept(sdListen, (struct sockaddr *)&clientAddr, &clientSize)) < 0 ) {
                if(errno == EINTR) {
                    continue;
                } else {
                    perror("Accept failed: ");

                    continue;
                }
            } // fine if TCP

            // Generazione figli: Server parallelo
            if((pid = fork()) == 0) { // Figlio

                printf("\n++ Client connected! Fork has been done! ++\n");

                // Devo restituire i nomi dei file presenti nei direttori di secondo livello (incluse anche le directory)
                //if (recv(sdTcp, &dirName, STR_MAX*sizeof(char), 0) < 0) {
                if(read(sdTcp, &dirName, STR_MAX * sizeof(char)) < 0) {
                    perror("Read failed: ");

                    exit(EXIT_FAILURE);
                }


                printf("Received the directory name: '%s'\n" , dirName);

                // Apro la directory del Client
                d = opendir(dirName);


                // Se sono riuscito ad aprire la directory
                if(d) {

                    printf("\t'%s' Directory correctly opened!\n", dirName);

                    // Itero sui file della directory
                    while((dir = readdir(d)) != NULL) {

                        // Se "dir" è una directory
                        if(dir->d_type == DT_DIR) {


                            // Entro nella cartella
                            d2  = opendir(dir->d_name);

                            // Se la cartella è valida
                            if(d2) {

                                printf("\t\t'%s' SubDirectory correctly opened!\n", dir->d_name);

                                while ((dir2 = readdir(d2)) != NULL) {

                                    printf("\t\t\t'%s' File\n", dir2->d_name);

                                    // Questo if sembra non funzionare molto bene...
                                    if(dir2->d_name[0] != '.') {

                                        // Invio della stringa tramite Socket TCP
                                        if(write(sdTcp, dir2->d_name, sizeof(dir2->d_name)) < 0) {
                                            perror("Write failed: ");

                                            continue;
                                        }
                                        printf("\t\t\t\tFile '%s' has been sent to the Client!\n", dir2->d_name);
                                    }
                                }//fine while dir 2

                                // Chiudo la SubDirectory
                                closedir(d2);
                            }

                            else { // fine if(dir->d_type == DT_DIR)
                                continue;
                            }
                        }



                    } // fine while dir 1

                    // Chiudo la directory iniziale
                    closedir(d);

                    // Chiudo lo stream di scrittura
                    shutdown(sdTcp, SHUT_WR);

                } else { // Se la Main Directory non è stata aperta correttamente
                    perror("Main Directory open error: ");

                    // Invio messaggio d'errore al client
                    write(sdTcp, errorString, strlen(errorString));

                    close(sdTcp);
                    exit(EXIT_FAILURE);

                }


                // Chiusura comunicazione
                close(sdTcp);

                exit(EXIT_SUCCESS);

            } else if(pid > 0) { // Padre

                continue;

            } else { // Errore fork

                // Codice errore
                perror("Fork error: ");

                continue;
            }
        } // Fine IS_SET
    }

    return 0;
}

// Gestore del segnale per eliminare i processi figli
void childHandler(int signum) {

    int status;

    printf("Executing handler SIGCHLD!\n");
    wait(&status);

}
