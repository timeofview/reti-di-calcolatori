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
#define BUF_MAX 256
#define BACKLOG_MAX 10



// Funzioni
//int deleteWord(char*, char*); --> da finireeeeeeeeeeeeee
void childHandler(int);


int main(int argc, char *argv[]) {

    // RequestUDP, indica una richiesta di tipo UDP
    typedef struct {
        char fileName[STR_MAX];
        char toDelete[STR_MAX];
    } RequestUDP;

    int sdUdp, sdListen, sdTcp, port, clientSize, pid, i, numfds, recurrences;
    const int enable=1;
    struct sockaddr_in clientAddr, serverAddr;
    fd_set rset;
    RequestUDP request;
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
            if (argv[2][i] < '0' || argv[1][i] > '9') {
                fprintf(stderr, "Error: port argument '%s' is not an only-digit!\n", argv[2]);

                exit(EXIT_FAILURE);
            }
        }


        // Controllo che l'argomento "Port" non ecceda il limite massimo di 65K
        // Converto la porta in intero e controllo che non super il limite
        port = atoi(argv[1]);
        if(port < PORT_MIN || port > PORT_MAX) {
            fprintf(stderr, "Error: port must be between %d and %d!\n", PORT_MIN, PORT_MAX);
        }

    } // fine else


    // Setto a zero l'indirizzo del serverAddr
    memset((char *)&serverAddr, 0, sizeof(serverAddr));

    // Inizializzo i parametri dell'indirizzo del Server
    serverAddr.sin_family= AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port= htons(port);


    // Creazione socket udp
    sdUdp = socket(AF_INET, SOCK_DGRAM, 0);
    if (sdUdp < 0) {
        perror("Error creating socket ");

        exit(EXIT_FAILURE);
    }

    // Creazione socket tcp
    sdListen = socket(AF_INET, SOCK_STREAM, 0);
    if(sdListen < 0) {
        perror("TCP Socket creation error: ");

        exit(EXIT_FAILURE);
    }

    // Opzioni Socket UDP
    if (setsockopt(sdUdp, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
        perror("Set Socket options error: ");

        exit(EXIT_FAILURE);
    }

    // Opzioni Socket TCP
    if(setsockopt(sdListen, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
        perror("Socket options error: ");

        exit(EXIT_FAILURE);
    }


    // Binding socket tcp
    if(bind(sdListen,(struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        perror("Socket bind error: ");

        exit(EXIT_FAILURE);
    }

    // Binding socket udp
    if (bind(sdUdp, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0) {
        perror("Socket bind error: ");

        exit(EXIT_FAILURE);
    }

    // listen della socket tcp
    if (listen(sdListen, BACKLOG_MAX) < 0) {
        perror("Listen queue error: ");

        exit(EXIT_FAILURE);
    }

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
        clientSize = sizeof(struct sockaddr_in);

        // Inizio UDP
        // Controllo se la socket sdUdp ha dei dati disponibili
        if(FD_ISSET(sdUdp, &rset)) {

            if(recvfrom(sdUdp, &request, sizeof(RequestUDP), 0, (struct sockaddr *)&clientAddr, &clientSize) < 0) {
                perror("recvFrom error: ");
                continue;
            }

            // Questa funzione mi elimina da fileName la stringa toDelete
            //recurrences = deleteWord(request.fileName, request.toDelete);
            printf("\n!!! UDP PART GOES HERE !!!\n");

            recurrences = 1;

            printf("\nRecurrences found in file: %d\n", recurrences);

            if(sendto(sdUdp, &recurrences, sizeof(int), 0, (struct sockaddr *)&clientAddr, clientSize) < 0) {
                perror("sendTo error: ");
                continue;
            }

        } // FINE UDP

        // Inizio TCP
        if(FD_ISSET(sdListen, &rset)) {
            if((sdTcp = accept(sdListen, (struct sockaddr *)&clientAddr, &clientSize)) < 0 ) {
                if(errno == EINTR) {
                    continue;
                } else {
                    perror("Accept error: ");

                    continue;
                }
            }//fine if TCP

            // Generazione figli: Server parallelo
            if((pid = fork()) == 0) { // Figlio

                printf("Client Connected! A fork has been done!\n");

                // Devo restituire i nomi dei file presenti nei direttori di secondo livello (incluse anche le directory)
                if (recv(sdTcp, &dirName, STR_MAX * sizeof(char), 0) < 0) {
                    perror("Receive error: ");
                    exit(EXIT_FAILURE);
                }

                // Apro la directory che il Client mi ha inviato
                d = opendir(dirName);

                // Se sono riuscito ad aprire la directory
                if(d) {

                    printf("Main directory '%s' opened succesfuly!\n", dirName);

                    // Quando trovo una directory
                    while((dir = readdir(d)) != NULL) {

                        // Se "dir" è una directory
                        if(dir->d_type == DT_DIR) {

                            // Entro nella cartella
                            d2  = opendir(dir->d_name);

                            // Se la cartella è valida
                            if(d2) {

                                printf("A Subdirectory has beeen correctly opened!\n");

                                while((dir2 = readdir(d2)) != NULL) {

                                    printf("File name: %s\n", dir2->d_name);

                                    // Invio della stringa tramite Socket TCP
                                    //if(send(sdTcp, dir2->d_name, strlen(dir2->d_name), 0) != strlen(dir2->d_name)) {
                                    if(write(sdTcp, dir2->d_name, strlen(dir2->d_name)) < 0) {
                                        perror("Send failed: ");

                                        continue;
                                    }
                                    printf("\n++ Subdirectory name '%s' has been sent to the Client! ++\n", dir2->d_name);

                                } // Fine while dir 2

                                // Chiudo la SubDirectory
                                closedir(d2);
                            }

                        } else { // fine if(dir->d_type == DT_DIR)

                            continue;
                        }

                    } // fine while dir 1

                    // Chiudo la directory iniziale
                    closedir(d);
                    shutdown(sdTcp, SHUT_RD);
                    //close(sdTcp); // Forse da mettere???
                    exit(EXIT_FAILURE);
                } else {
                    perror("Directory open error: \n");
                    continue;
                }

                exit(EXIT_SUCCESS);

            } else if(pid > 0) { // Padre

                continue;
            } else { // Errore fork

                // Codice errore
                perror("Fork error: ");

                continue;
            }
        }//fine IS_SET
    }

    return 0;
}


// Gestore eccezioni
void childHandler(int signum) {

    int status;

    printf("ChildHandler!\n");

    wait(&status);
}
