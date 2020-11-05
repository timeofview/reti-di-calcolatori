#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
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


// Function prototypes
void childHandler(int);


// UDP request Struct
typedef struct {
    char file_in[STR_MAX];
    char word[STR_MAX];
} request;


int main(int argc, char *argv[]) {


    int sdUdp, sdListen, sdTcp, port, clientSize, pid, i, j, numfds, cntFound, fdInput, fdOutput, nRead, toRemoveLen;
    const int reuse = 1;
    struct sockaddr_in clientAddr, serverAddr;
    struct hostent *clienthost;
    char buff[STR_MAX], dirName[STR_MAX],  *fileOutName;
    fd_set rset;
    request req;
    DIR *d, *d2;
    struct dirent *dir, *dir2; // DirEntity, represent a Directory-Type Entity


    // Arguments Check
    if(argc == 1) { //Uses default port
        port = DEFAULT_PORT;
    } else if(argc != 2) {
        fprintf(stderr, "Error: invalid argument number!\nProgram usage: %s Port[OPTIONAL]", argv[0]);
        exit(EXIT_FAILURE);
    } else {

        // Check that argv is composed by numbers
        for(i=0; i<strlen(argv[1]); i++) {
            if (argv[1][i] < '0' || argv[1][i] > '9') {
                fprintf(stderr, "Error: port argument '%s' is not an only-digit!\n", argv[1]);
                exit(EXIT_FAILURE);
            }
        }

        // Checks on port (isInteger()&&isBetween(1024,65535))
        port = atoi(argv[1]);
        if(port < PORT_MIN || port > PORT_MAX) {
            fprintf(stderr, "Error: port needs to be between %d and %d!\n", PORT_MIN, PORT_MAX);
            exit(EXIT_FAILURE);
        }
    }

    // Setting Server Address to 0
    memset((char *)&serverAddr, 0, sizeof(serverAddr));

    // Server Address Parameters Initialisation
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    // UDP Socket Creat
    if ((sdUdp = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("UDP Socket creation error: ");
        exit(EXIT_FAILURE);
    }

    // TCP Socket Creat
    sdListen = socket(AF_INET, SOCK_STREAM, 0);
    if(sdListen < 0) {
        perror("TCP Socket creation error: ");
        exit(EXIT_FAILURE);
    }

    // UDP Socket setting
    if (setsockopt(sdUdp, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("Failed setting UDP socket options: ");
        exit(EXIT_FAILURE);
    }

    // TCP Socket setting
    if(setsockopt(sdListen, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("Failed setting TCP socket options: ");
        exit(EXIT_FAILURE);
    }


    // Binding Socket TCP
    if(bind(sdListen, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        perror("TCP Socket bind failed: ");
        exit(EXIT_FAILURE);
    }

    // Binding Socket UDP
    if (bind(sdUdp, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("UDP Socket bind failed: ");
        exit(EXIT_FAILURE);
    }

    // TCP Listening - Marking Passive Socket
    if (listen(sdListen, BACKLOG_MAX) < 0) {
        perror("Listen failed: ");
        exit(EXIT_FAILURE);
    }

    // C
    signal(SIGCHLD, childHandler);

    // Clearing descriptors set
    FD_ZERO(&rset);

    // The numfds argument specifies the range of file descriptors to be tested
    if(sdListen > sdUdp) {
        numfds = sdListen + 1;
    } else {
        numfds = sdUdp + 1;
    }

    printf("++ Server initialized! Waiting for Client connections! ++\n\n");

    // Size of client
    clientSize = sizeof(struct sockaddr_in);

    // mainLoop()
    while(1) {

        // Adding descriptors to set
        FD_SET(sdListen, &rset);
        FD_SET(sdUdp, &rset);

        // Extracts the first request
        if(select(numfds, &rset, NULL, NULL, NULL) < 0) {

            // if Signal occurred during the system call
            if(errno == EINTR) {

                continue;
            } else {

                perror("Select error: ");

                exit(EXIT_FAILURE);
            }
        }

        // UDP

        // Tests to see if a descriptor is part of the set
        if(FD_ISSET(sdUdp, &rset)) {

            // Receiving a request from client
            if (recvfrom(sdUdp, &req, sizeof(req), 0, (struct sockaddr *) &clientAddr, &clientSize) < 0) {

                perror("Recvfrom failed: ");

                continue;
            }

            // isClientValid()
            clienthost = gethostbyaddr((char *)&clientAddr.sin_addr, sizeof(clientAddr.sin_addr), AF_INET);
            if (clienthost == NULL) {

                fprintf(stderr, "Client host information not found!\n");
            } else {

                printf("Deleting word '%s' from file '%s'...\n", req.word, req.file_in);
            }

            // File Open and separating FAILURE from SUCCESS
            if ((fdInput = open(req.file_in, O_RDONLY)) < 0) {

                perror("Cannot open file: ");
                cntFound = -1;
            } else {

                // Creating a new file to substitute to the original
                //char fileOutName[strlen(req.file_in) + 4]; --> this stinks
                if( (fileOutName = (char *) malloc(strlen(req.file_in) * sizeof(char))) == NULL) {

                    perror("String allocation failed: ");
                }

                sprintf(fileOutName, "%s.tmp", req.file_in);
                printf("File Temp: %s\n", fileOutName);
                fdOutput = open(fileOutName, O_WRONLY | O_CREAT, 0777);

                // Length of word to remove and initializing wordCounter
                toRemoveLen = strlen(req.word);
                cntFound = 0;

                // While I can read from file
                do {
                    nRead = read(fdInput, buff, STR_MAX);
                    if(nRead < 0) { // Era <= 0
                        break;
                    }
                    // Checking if the buff contain the word
                    for(i=0; i<nRead; i++) {

                        // Is the first char of the word is found
                        if(buff[i] == req.word[0]) {

                            // Checking other chars toRemoveLen-times
                            for(j=0; j<toRemoveLen; j++) {

                                // If i find different chars I give up
                                if(buff[j+i] != req.word[j]) {
                                    break;
                                }
                            }
                            // If I find the word and it is not a substring -> Increase counter, increase iterator and don't write
                            if(j == toRemoveLen && (buff[j+i] == ' ' || buff[j+i] == '\t' || buff[j+i] == '\n' || i+j == nRead)) {

                                i += toRemoveLen;
                                cntFound++;
                            }
                        }
                        write(fdOutput, &buff[i], sizeof(char));
                    }
                } while(nRead > 0);

                // Closing file out and Overwriting
                close(fdOutput);
                remove(req.file_in);
                rename(fileOutName, req.file_in);
            }

            // Closing file in
            close(fdInput);
            free(fileOutName);
            printf("Sending %d to Client...\n", cntFound);

            // Sending result to Client
            if(sendto(sdUdp, &cntFound, sizeof(cntFound), 0, (struct sockaddr *)&clientAddr, sizeof(struct sockaddr))<0) {
                perror("Last SendTo failed:  ");
                continue;
            }

        }

        // TCP

        // Tests to see if a descriptor is part of the set
        if(FD_ISSET(sdListen, &rset)) {

            // Extracting the request on the queue
            if((sdTcp = accept(sdListen, (struct sockaddr *)&clientAddr, &clientSize)) < 0 ) {

                // if Signal occurred during the system call
                if(errno == EINTR) {

                    continue;
                } else {

                    perror("Accept failed: ");
                    continue;
                }
            }

            // Children fork
            if((pid = fork()) == 0) { // Child
                printf("Client connected and fork() executed!\n\n");

                // Reading directory name from Client
                if(read(sdTcp, &dirName, STR_MAX * sizeof(char)) < 0) {
                    perror("Read failed: ");
                    exit(EXIT_FAILURE);
                }

                printf("Received the directory name: '%s'\n\n" , dirName);

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

                                    // Escludo le Directory nascoste
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
                            }else { // fine if(dir->d_type == DT_DIR)
                                continue;
                            }
                        }

                    } // fine while dir 1

                    // Chiudo la directory iniziale
                    closedir(d);

                    // Chiudo lo stream di scrittura
                    shutdown(sdTcp, SHUT_WR);

                } else { // If directory has not been opened
                    perror("Main Directory open error: ");

                    // Sending error to client
                    write(sdTcp, ".", 1);
                    close(sdTcp);
                    exit(EXIT_FAILURE);
                }

                // Closing communications
                close(sdTcp);
                exit(EXIT_SUCCESS);

            } else if(pid > 0) { // Father

                continue;
            } else { // Error

                perror("Fork failed: ");

                continue;
            }
        }
    }
    return 0;
}


void childHandler(int signum) {

    int status;

    printf("\nExecuting childHandler!\n");
    wait(&status);
}
