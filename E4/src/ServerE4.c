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
	typedef struct
	{
	    char file_in[STR_MAX];
	    char word[STR_MAX];
	} request;

    int sdUdp, sdListen, sdTcp, port, clientSize, pid, i, numfds, recurrences;
    const int reuse=1;
    int wordLen, readed, j = 0;
    struct sockaddr_in clientAddr, serverAddr;
    struct hostent *clienthost;
    char buff[STR_MAX];
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
    if ((sdUdp = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
    		perror("UDP Socket creation error: ");
            exit(EXIT_FAILURE);
        }

    // Creazione socket tcp
    sdListen = socket(AF_INET, SOCK_STREAM, 0);
    if(sdListen < 0) {
        perror("TCP Socket creation error: ");

        exit(EXIT_FAILURE);
    }

    // Opzioni Socket UDP
    if (setsockopt(sdUdp, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
        {
            perror("Set opzioni socker UDP");
            exit(EXIT_FAILURE);
        }

    // Opzioni Socket TCP
    if(setsockopt(sdListen, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("Socket options error: ");

        exit(EXIT_FAILURE);
    }


    // Binding socket tcp
    if(bind(sdListen,(struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        perror("Socket bind error: ");

        exit(EXIT_FAILURE);
    }

    // Binding socket udp
    if (bind(sdUdp, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
       {
           perror("Socket bind error :");
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

        // Inizio UDP
        // Controllo se la socket sdUdp ha dei dati disponibili
        if(FD_ISSET(sdUdp, &rset)) {

        	 if (recvfrom(sdUdp, &req, sizeof(request), 0, (struct sockaddr *)&clientAddr, sizeof(struct sockaddr_in)) < 0)
        	    {
        	        perror("Recvfrom error ");
        	    }

        	 clienthost = gethostbyaddr((char *)&clientAddr.sin_addr, sizeof(clientAddr.sin_addr), AF_INET);
        	 int fd_in = open(req.file_in, O_RDONLY);
        	 if (fd_in < 0)
        	     {
        	         recurrences = -1;
        	     }
        	     else{
        	    	 recurrences = 0;
        	    	         wordLen = strlen(req.word);
        	    	         char tmp_buff[wordLen];

        	    	         char *file_out[strlen(req.file_in) + 4];
        	    	         strcpy(file_out, req.file_in);
        	    	         strcat(file_out, ".tmp");
        	    	         int fd_out = open(file_out, O_WRONLY | O_CREAT, 0777);
        	    	         while ((readed = read(fd_in, buff, STR_MAX)) > 0)
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
        	    	                                 recurrences++;
        	    	                                 j = 0;
        	    	                             }
        	    	                         }
        	    	                         write(fd_out, &buff[i], 1);
        	    	                     }
        	    	                 }
        	    	         close(fd_in);
							 close(fd_out);
							 rename(file_out, req.file_in);
							 if(sendto(sdUdp, &recurrences, sizeof(int), 0, (struct sockaddr *)&clientAddr, sizeof(struct sockaddr)) < 0) {
							                perror("sendTo error: ");
							                continue;
							 }

        	     }

            printf("\nRecurrences found in file: %d\n", recurrences);

        } // FINE UDP

       // Inizio TCP
               if(FD_ISSET(sdListen, &rset)) {
                   if((sdTcp = accept(sdListen, (struct sockaddr *)&clientAddr, &clientSize)) < 0 ) {
                       if(errno == EINTR) {
                           continue;
                       } else {
                           perror("accept error: ");

                           continue;
                       }
                   }//fine if TCP

                   // Generazione figli: Server parallelo
                   if((pid = fork()) == 0) { // Figlio
                   	printf("CREAZIONE FIGLIO\n");
                       // Devo restituire i nomi dei file presenti nei direttori di secondo livello (incluse anche le directory)
                   	if (recv(sdTcp, &dirName, STR_MAX*sizeof(char), 0) < 0) {
                   	      perror("recv failed");
                   	      exit(EXIT_FAILURE);
       				}
                   	printf("RICEVUTO DAL CLIENT : %s\n" , dirName);
                       // Apro la directory del Client
                   	d = opendir(dirName);
                   	printf("Directory opened\n");
                       // se sono riuscito ad aprire la directory
                       if(d) {

                           // Quando trovo una diretory
                           while ((dir = readdir(d)) != NULL) {
                           	printf("nomeDirectory : %s\n", dir->d_name);
                               // Se "dir" è una directory
                               if(dir->d_type == DT_DIR) {

                                   // Entro nella cartella
                               	d2  = opendir(dir->d_name);
                               	printf("Directory 2.0 opened\n");
                                   // Se la cartella è valida
                                   if(d2) {

                                       while ((dir2 = readdir(d2)) != NULL) {
                                       	printf("nomeFile : %s\n", dir2->d_name);
                                       	if(strcmp(dir2->d_name,"..")!=0 || strcmp(dir2->d_name,".")!=0){
       										// Invio della stringa tramite Socket TCP
       										if(write(sdTcp ,&dir2->d_name, sizeof(dir2->d_name)) < 0) {
       											perror("Send failed: ");

       											continue;
       										}
       										printf("inviato %s\n", dir2->d_name);
                                       	}
                                       }//fine while dir 2

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
                           //close(sdTcp);
                           exit(EXIT_FAILURE);
                       } else {
                           perror("Error: cannot open directory!\n");
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
       void childHandler(int signum){

          int status;

          printf("ChildHandler è in esecuzione!\nPlease wait...\n");
          wait(&status);
       }
