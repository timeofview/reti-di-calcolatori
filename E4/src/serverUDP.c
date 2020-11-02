#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define DIM 256

typedef struct
{
    char file_in[DIM];
    char word[DIM];
} request;

int main(int argc, char **argv)
{
    char buff[DIM];
    char buff_out[DIM];
    int len, wordLen, readed, count, fd_in, i, j = 0;

    struct sockaddr_in servaddr, cliaddr;
    struct hostent *clienthost;
    request req;
    int port = 54321;
    int sd_udp;
    const int reuse = 1;

    // Arguments Number Check
  	if (argc != 2) {
  		printf("Error Usage : %s port \n", argv[0]);
  		exit(1);
  	}

    // Is port an integer?
    len = 0;
  	while (argv[1][len] != '\0') {
  		if (argv[1][len] < '0' || argv[1][len] > '9') {
  			printf("Argument passed is not a integer");
  			exit(2);
  		}
  		len++;
  	}

    // 1024<=port<=65535
    port = atoi(argv[1]);
    if (port < 1024 || port > 65535) {
		    printf("Error: incorrect number port ");
		    exit(3);
    }

    printf("Checks completed\n");

    // Setting socket
    memset((char *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    // Socket creat
    if ((sd_udp = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error creating socket");
        exit(4);
    }

    // Socket options
    if (setsockopt(sd_udp, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("Set options socket UDP");
        exit(5);
    }

    // Socket bindings
    if (bind(sd_udp, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind socket UDP");
        exit(6);
    }

    printf("Settings and Bindings completed\n");

    while(1){

        len = sizeof(struct sockaddr_in);
        // Socket receive
        if (recvfrom(sd_udp, &req, sizeof(req), 0,
                    (struct sockaddr *) &cliaddr, &len) < 0){
            perror("Recvfrom error ");
            continue;
        }

        clienthost = gethostbyaddr((char *)&cliaddr.sin_addr,
                                                sizeof(cliaddr.sin_addr), AF_INET);
        if (clienthost == NULL){
                printf("Client host information not found\n");
        }
            else{
                printf("Operation required from: %s %i\nI will work on word %s of file %s\n",
        clienthost->h_name, (unsigned) ntohs(cliaddr.sin_port), req.file_in, req.word);
        }

        // Open file and separting FAILURE from SUCCESS
        if ((fd_in = open(req.file_in, O_RDONLY)) < 0) {
            printf("Bad fileName\n");
            count = -1;
        }
        else {
            count = 0;
            wordLen = strlen(req.word);
            char tmp_buff[wordLen];
            char *file_out[strlen(req.file_in) + 4];
            strcpy(file_out, req.file_in);
            strcat(file_out, ".tmp");
            int fd_out = open(file_out, O_WRONLY | O_CREAT, 0777);

            while ((readed = read(fd_in, buff, DIM)) > 0){
                for (i = 0; i < readed; i++){
                    if (req.word[j] == buff[i]){
                        tmp_buff[j] = buff[i];
                        for (; j < wordLen && i < readed; ++j){
                            if (req.word[j] == buff[i]){
                                tmp_buff[j] = buff[i++];
                            }
                            else{
                                write(fd_out, tmp_buff, strlen(tmp_buff));
                                j = 0;
                                break;
                            }
                        }
                        if (j == wordLen){
                            count++;
                            j = 0;
                        }
                    }
                    write(fd_out, &buff[i], 1);
                }
            }

            // Closing file out and overwriting
            close(fd_out);
            remove(req.file_in);
            rename(file_out, req.file_in);
        }

        // Closing file in
        close(fd_in);
        printf("Sending result\n");

        // Sending result to Client
        if(sendto(sd_udp, &count, sizeof(count), 0,
                    (struct sockaddr *)&cliaddr, sizeof(struct sockaddr))<0){
            perror("Error final sendto");
            continue;
        }
        printf("Completed an Operation\n------------------\n");
    }
    return EXIT_SUCCESS;
}
