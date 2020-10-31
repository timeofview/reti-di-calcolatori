#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

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
    int wordLen, readed, count, i, j = 0;

    struct sockaddr_in servaddr, cliaddr;
    struct hostent *clienthost;
    request req;
    int port = 54321;
    int sd_udp;
    const int reuse = 1;

    if (argc == 2)
    {
        port = atoi(argv[1]);
    }
    //set socket ecc
    memset((char *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    if ((sd_udp = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Errore apertura socket UDP.");
        exit(2);
    }
    if (setsockopt(sd_udp, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
    {
        perror("Set opzioni socker UDP");
        exit(9);
    }
    if (bind(sd_udp, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Bind socket UDP");
        exit(10);
    }
    if (recvfrom(sd_udp, &req, sizeof(request), 0, (struct sockaddr *)&cliaddr, sizeof(struct sockaddr_in)) < 0)
    {
        perror("Recvfrom error ");
    }
    clienthost = gethostbyaddr((char *)&cliaddr.sin_addr, sizeof(cliaddr.sin_addr), AF_INET);
    clienthost->h_name, (unsigned)ntohs(cliaddr.sin_port);
    int fd_in = open(req.file_in, O_RDONLY);
    if (fd_in < 0)
    {
        count = -1;
    }
    else
    {
        count = 0;
        wordLen = strlen(req.word);
        char tmp_buff[wordLen];

        char *file_out[strlen(req.file_in) + 4];
        strcpy(file_out, req.file_in);
        strcat(file_out, ".tmp");
        int fd_out = open(file_out, O_WRONLY | O_CREAT, 0777);

        while ((readed = read(fd_in, buff, DIM)) > 0)
        {
            for (i = 0; i < readed; i++)
            {
                if (req.word[j] == buff[i])
                {
                    tmp_buff[j] = buff[i];
                    for (; j < wordLen && i < readed; ++j)
                    {
                        if (req.word[j] == buff[i])
                        {
                            tmp_buff[j] = buff[i++];
                        }
                        else
                        {
                            write(fd_out, tmp_buff, strlen(tmp_buff));
                            j = 0;
                            break;
                        }
                    }
                    if (j == wordLen)
                    {
                        count++;
                        j = 0;
                    }
                }
                write(fd_out, &buff[i], 1);
            }
        }

        close(fd_in);
        close(fd_out);
        rename(file_out, req.file_in);
        sendto(sd_udp, &count, sizeof(count), 0, (struct sockaddr *)&cliaddr, sizeof(struct sockaddr));
    }
    return 0;
}