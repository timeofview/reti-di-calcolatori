#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/rpc.h>
#include "VotaFattoreX.h"

#define MAX_STR 255
#define MAX_JUDGES 15


/*
Il Client filtro realizza l’interazione con l’utente, proponendo ciclicamente i servizi che utilizzano le due procedure remote e stampa a 
video gli esiti delle chiamate, fino alla fine del file di input da tastiera
 
 
1) La procedura "classifica_giudici" non richiede parametri d'ingresso
   e restituisce la struttura Output che contiene i nomi di tutti i
   giudici, ordinati secondo il punteggio totale dei rispettivi candidati
   
2) La procedura "esprimi_voto" accetta come parametro d’ingresso due stringhe,
   la prima rappresenta il candidato mentre la seconda specifica il tipo di 
   operazione (aggiunta/sottrazione) e aggiorna la struttura dati aggiungendo
   o sottraendo un voto al candidato scelto 


// Struct di input
struct Input{ 
	string candidate_name<MAXSTRLEN>; 
	string operation_name<MAXSTRLEN>; 
};

// Struct che indica il giudice
struct Judge{
	string judge_name<MAXSTRLEN>;
};

// Struct di output
struct Output{ 
	Judge judges[N];
};

*/


// Usage: programName host 
int main(int argc, char *argv[]) {
	
	
	// Gestore di trasporto
	CLIENT *cl; 
	
	// Strutture definite in "VotaFattoreX.x"
	Input input; // Da inviare, quindi senza puntatore
	Output *output;// Come risultato, quindi ci va il puntatore
	
	
	// Variabili del programma
	char *server;
	char candidato[MAX_STR];
	char scelta_voto;
	char op;
	int i;
	int *result;


	// Controllo argomenti 
    if (argc != 2) {
        fprintf(stderr, "Usage: %s serverAddress\n", argv[0]);
        
        exit(EXIT_FAILURE);
    }
	
	// Assegnazione stringa Server
    server = argv[1];
	
	// Creazione gestore di trasporto
    cl = clnt_create(server, VOTAFATTORE, VOTAFATTOREVERS, "udp");

	// Se non sono riuscito a creare il gestore di trasporto
    if(cl == NULL) {
		clnt_pcreateerror(server);
		
		exit(EXIT_FAILURE);
    }

	
	// Ciclo di richiesta del servizio da eseguire
	printf("Scrivi il nome dell'operazione che vuoi eseguire:\n\n[R]anking -  mostra i nomi di tutti i giudici, ordinati secondo il punteggio totale dei rispettivi candidati\n[V]ote - aggiunge o sottrae un voto al candidato scelto\n\n[EOF per terminare]: ");
	while(scanf("%c", &op) != EOF) {
		
		//Output ranking(void)
		switch(op) {
			
			// Label multiple, nel caso l'utente preme "f" oppure "F"
			case 'r': case 'R': {
				
				printf("\nVengono mostrati sotto i nomi di tutti i giudici ordinati:\n");
				output = ranking_1(NULL, cl);
				
				printf("Sono dopo la chiamata!\n");
				
				// Se c'è stato un errore nella chiamata di procedura remota
				if(output == NULL) {
					perror("\nErrore durante la ricezione del risultato");

					exit(EXIT_FAILURE);
				}
				
				printf("Sono prima del ciclo\n");
				
				// Ora stampo a video tutti i giudici
				for(i=0; i<MAX_JUDGES; i++){
					printf("Sono nel ciclo %d\n", i);
					printf("Nome giudice: %s\n", output->judges[i].judge_name);
				}
				
				
				break;
			}
			
			case 'v': case 'V': {
				
				do{
					printf("\nInserisci il candidato che vuoi votare: ");
					scanf("%s", input.candidate_name);
					printf("\n[A]ggiungi voto\n[S]ottrai voto\nLa tua scelta: ");
					scanf("%c", &scelta_voto);
					
					if(scelta_voto == 'A' || scelta_voto == 'a'){
						strcpy(input.operation_name, "add");
					}else if(scelta_voto == 'S' || scelta_voto == 's'){
						strcpy(input.operation_name, "sub");
					}else{
						printf("\nNon hai inserito un'operazione valida!");
					}
					
					result = vote_1(&input, cl);
					
					// Se c'è stato un errore nella ricezione del risultato
					if(result == NULL) {
						perror("\nErrore durante la ricezione del risultato");

						exit(EXIT_FAILURE);
					}
					
					// Se la procedura remota è riuscita ad aggiungere/sottrarre il voto
					if(*result > 0){
						printf("\nIl tuo voto e' stato correttamente aggiunto/sottratto!\n");
					}else{
						fprintf(stderr, "\nErrore: e' la procedura remota e' fallita! Non e' stato aggiunto/sottratto il tuo voto!\n");
						
						
						exit(EXIT_FAILURE);
					}
					
					
				}while(scelta_voto != 'A' || scelta_voto != 'a' || scelta_voto != 'S' || scelta_voto != 's');

				
				break;
			}
			
			default: {
				
				printf("\nErrore: operazione non valida! Riprova!\n");
				
				break;
			}
			
		}

		printf("Scrivi il nome dell'operazione che vuoi eseguire:\n\n[R]anking -  mostra i nomi di tutti i giudici, ordinati secondo il punteggio totale dei rispettivi candidati\n[V]ote - aggiunge o sottrae un voto al candidato scelto\n\n[EOF per terminare]: ");
	}
	
	
	
	printf("\nFine programma :]\n");
	
	return 0;
	
}
