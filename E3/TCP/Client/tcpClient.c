#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>

#define DIM_BUFF 1024

int main(int argc,char **argv) {
    int sd,fd_in,fd_out,port,row,nread;
    struct hostent *host;
    char filename[FILENAME_MAX];
    char buff[DIM_BUFF];

    if(argc!=3){
        perror("Numero di argomenti sbagliato.");
        exit(1);
    }
    host = gethostbyname(argv[1]);
    port=atoi(argv[2]);
    if(host==NULL){
        perror("NULL host");
        exit(2);
    }
    struct sockaddr_in socket_add = {.sin_addr.s_addr= ((struct in_addr *)(host->h_addr)) -> s_addr,.sin_port= htons(port),.sin_family=AF_INET};

    printf("Write filename:\n");
    gets(filename);
    printf("Write row's number, to delete:\n");
    scanf("%d",&row);

    if((fd_in=open(filename,O_RDONLY))<0){
        perror("Errore appertura file in input.");
        exit(2);
    }
    char filename_out[FILENAME_MAX];
    strcpy(filename_out,filename);
    strcat(filename_out,"-out");
    if((fd_out=open(filename_out,O_WRONLY|O_CREAT,0777))<0){
        perror("Errore appertura file in output.");
        exit(3);
    }
    if((sd = socket(AF_INET,SOCK_STREAM,0))<0){
        perror("Errore appertura socket.");
        exit(4);
    }
    if(connect(sd,(struct sockaddr *)&socket_add, sizeof(struct sockaddr)) < 0){
        perror("Errore connessione");
        exit(5);
    }
    write(sd,&row,sizeof(int));
    printf("Sending\n");
    while((nread=read(fd_in,buff,DIM_BUFF))>0){
        write(sd,buff,nread);
    }
    shutdown(sd,1);
    close(fd_in);
    printf("Receiving\n");
    while((nread=read(sd,buff,DIM_BUFF))>0){
        write(fd_out,buff,nread);
        //write(0,buff,nread);
        printf("%s",buff);
    }
    shutdown(sd,0);
    close(sd);
    close(fd_out);
    return 0;
}
