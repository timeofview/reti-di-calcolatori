#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <rpc/rpc.h>

#include "VotaFattoreX.h"

int main(int argc, char *argv[]) {
	
	CLIENT *cl; char *server;
	Input input; Output *output;
	char scelta_voto, op;
	int i, *result; void *in;

    if (argc != 2) {
        printf("Usage: %s serverAddress\n", argv[0]); exit(1);
    }
	
    server = argv[1];
    cl = clnt_create(server, VOTAFATTORE, VOTAFATTOREVERS, "udp");

    if(cl == NULL) {
		clnt_pcreateerror(server); exit(1);
    }

	printf("Inserire:\t[R] per il Ranking\t[V] per Votare\t[EOF] per Terminare: ");
	while(scanf("%c", &op) != EOF) {
		
		switch(op) {
			case 'r': case 'R': {

				printf("\nLista Ordinata Giudici:\n");
				output = ranking_1(in, cl);
								
				if(output == NULL) {
					clnt_perror(cl,server); exit(1);
				}
				
				for(i=0; i<N; i++)
					printf("Nome giudice: %s\n", output->judges[i].judge_name);
				
				break;
			}
			case 'v': case 'V': {
				
				printf("\nInserisci il candidato che vuoi votare: ");
				scanf("%s", input.candidate_name);
				getchar();
				printf("Inserire:\t[A] per Aggiungere\t[S] per Sottrarre un voto: ");
				scanf("%c", &scelta_voto);
				getchar();
				if(scelta_voto == 'A' || scelta_voto == 'a'){
					strcpy(input.operation_name, "add");
				}else if(scelta_voto == 'S' || scelta_voto == 's'){
					strcpy(input.operation_name, "sub");
				}else{
					printf("\nNon hai inserito un'operazione valida!\n");
					break;
				}
				
				result = vote_1(&input, cl);
				
				if(result == NULL) {
					perror("\nErrore durante la ricezione del risultato"); exit(1);
				}
				
				if(*result > 0){
					printf("\nIl tuo voto e' stato correttamente aggiunto/sottratto!\n");
				}else{
					printf("\nErrore: La procedura remota e' fallita!\n"); exit(1);
				}
				break;
			}
			default: {
				printf("\nError: Operazione non trovata! Riprova!\n");	
				break;
			}		
		}
		printf("Inserire:\t[R] per il Ranking\t[V] per Votare\t[EOF] per Terminare: ");
	}
	clnt_destroy(cl);
	printf("\nEnd\n");
	return 0;
}
