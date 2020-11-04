#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>


#define PORT_MIN 1024
#define PORT_MAX 65535
#define STR_MAX 256


int main(int argc, char * argv[]) {

    int sd, nRead, port, i;
    char dirName[STR_MAX], serverReply[STR_MAX];
    struct sockaddr_in serverAddr;
    struct hostent *serverHost;

    // Timeout struct, used to set a timeout
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    // Arguments Check
    if(argc != 3) {
        fprintf(stderr, "Error: invalid argument number!\nProgram usage: %s Address Port", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Check that argv is composed by numbers
    for(i=0; i<strlen(argv[2]); i++) {
        if (argv[2][i] < '0' || argv[2][i] > '9') {
            fprintf(stderr, "Error: port argument '%s' is not an only-digit!\n", argv[2]);
            exit(EXIT_FAILURE);
        }
    }

    // Checks on port (isInteger()&&isBetween(1024,65535))
    port = atoi(argv[2]);
    if(port < PORT_MIN || port > PORT_MAX) {
		fprintf(stderr, "Error: port needs to be between %d and %d!\n", PORT_MIN, PORT_MAX);
		exit(EXIT_FAILURE);
    }

    serverHost = gethostbyname(argv[1]);
    if(serverHost == NULL) {
        perror("Unknown Server error: ");
    }

    // Setting Address to 0
    memset((char*) &serverAddr, 0, sizeof(struct sockaddr_in));

    // Server Address Parameters Initialisation
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ((struct in_addr*) (serverHost->h_addr))->s_addr;
    serverAddr.sin_port = htons(port);

    // Asking in a loop nameDir
    printf("Enter directory name [EOF to end]: ");
    while(scanf("%s", dirName) != EOF) {
	
		// Socket Open
		if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("Socket error: ");
			exit(EXIT_FAILURE);
		}

		// Setting receiveTime in socket Options
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
		
        // Sending Result
        if(write(sd, dirName, strlen(dirName)) < 0) {
            perror("Send failed: ");
            continue;
        }

        // Receiving String list
        while((nRead = read(sd, serverReply, STR_MAX * sizeof(char))) > 0) {

            // Error receive
            if(nRead < 0) {
                perror("Socket Read failed: ");
                continue;
            } else if(serverReply[0]=='.'){
                printf("Cannot open Directory!\n");
            } else {
                printf("%s\n", serverReply);
            }

            // String flush
            memset((char*) &serverReply, 0, STR_MAX * sizeof(char));
        }
		
		// SocketClose
		close(sd);
        printf("Enter directory name [EOF to end]: ");
    }
    printf("Communication Closed\n");
    return 0;
}
