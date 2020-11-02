#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define DIM 256
#define PORT_MIN 1024
#define PORT_MAX 65535

typedef struct {
    char fileName[DIM];
    char wordName[DIM];
} toSend;

int main(int argc, char *argv[]) {

    // Variables
    int i, port, serverSize, reply;
    // Socket descriptors
    int sd;
    // File Name to send
    toSend to_send;
    // HostEntity serverHost
    struct hostent *serverHost;
    // Socket Address
    struct sockaddr_in clientAddr, serverAddr;

    // Struct timeout
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    // Arguments check
    if (argc != 3) {
        printf("Error: incorrect argument number!\nProgram usage: %s ServerAddress ServerPort\n", argv[0]);
        exit(1);
    }

    // Port Check for Integer value
    for(i=0; i<strlen(argv[2]); i++) {
        if (argv[2][i] < '0' || argv[2][i] > '9') {
            fprintf(stderr, "Error: port argument '%s' is not an only-digit!\n", argv[2]);
            exit(EXIT_FAILURE);
        }
    }

    // atoi and checks on port
    port = atoi(argv[2]);
    if(port < PORT_MIN || port > PORT_MAX) {
        fprintf(stderr, "Error: port is exceeding the 65K limit!\n");
    }

    // clientAddr -> 0
    memset((char*) &clientAddr, 0, sizeof(struct sockaddr_in));

    // Socket initialisation
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = INADDR_ANY;
    clientAddr.sin_port = 0;

    // serverAddr -> 0
    memset((char*) &serverAddr, 0, sizeof(struct sockaddr_in));

    // gethostbyname -> serverHost
    serverHost = gethostbyname(argv[1]);
    if(serverHost == NULL) {
        fprintf(stderr, "Error: cannot resolve host %s!\n", argv[1]);

        exit(EXIT_FAILURE);
    }

    // Socket address settings
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ((struct in_addr*) (serverHost->h_addr))->s_addr;
    serverAddr.sin_port = htons(port);

    // Socket Creat
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0) {
        fprintf(stderr, "Error: cannot create socket!\n");
        exit(EXIT_FAILURE);
    }

    // Binding Socket to clientAddr
    if(bind(sd, (struct sockaddr*) &clientAddr, sizeof(struct sockaddr_in)) < 0) {
        fprintf(stderr, "Error: cannot bind socket!\n");
        exit(EXIT_FAILURE);
    }

    // Timeout Setting
    if (setsockopt (sd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval)) < 0) {
        fprintf(stderr, "Error: setsockopt failed\n");
    }
    if (setsockopt (sd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval)) < 0) {
        fprintf(stderr, "Error: setsockopt failed\n");
    }

    printf("Enter the file name[EOF to end]: ");
    
    // MainLoop()
    while(scanf("%s", to_send.fileName) != EOF) {

        printf("Enter the Word to remove from file: ");
        scanf("%s", to_send.wordName);

        // SendStruct
        if ((sendto(sd, &to_send, sizeof(toSend), 0, (struct sockaddr*) &serverAddr, sizeof(struct sockaddr_in))) < 0) {
            printf("Error: cannot send file %s\n", to_send.fileName);
            continue;
        }

        printf("FileName '%s' and wordName '%s' have been sent to the Server!\n", to_send.fileName, to_send.wordName);
		
		
		
		
        // ReadServerSize()
        serverSize = sizeof(serverAddr);
		
		// Ripulisco buffer per contenere poi la risposta del Server
		memset((char*) &reply, 0, sizeof(int));
		
        // ReadDeletedWord()
        if (recvfrom(sd, &reply, sizeof(int), 0, (struct sockaddr*) &serverAddr, &serverSize) < 0) {
            fprintf(stderr, "Error: cannot receive the number of deleted word from Server!\n");
            continue;
        }

        // Read Reply from Server
        //reply = (int)ntohl(reply);
        if(reply < 0)
            fprintf(stderr, "Error: file '%s' not found in the Server's FileSystem!\n", to_send.fileName);
        else
            printf("Number of removed words from '%s' is %d\n", to_send.fileName, reply);
            
        printf("Enter file name [EOF to end]: ");
    }

    // Communication Close
    close(sd);
    printf("End of comunication!\n");

    return 0;
}
