#include <stdio.h>
#include <stdlib.h>
#include <rpc/rpc.h>
#include "operations.h"

#define MAX_STR 128


/*
La procedura "file_scan" accetta come parametro d'ingresso il nome del file e restituisce una struttura dati che contiene tre interi
	1) numero di caratteri
	2) parole e linee nel file
	3) oppure un opportuno codice di errore (in caso ad esempio il file sia vuoto oppure non sia presente sul server)
	
La procedura "dir_scan" accetta come parametro d'ingresso il nome del direttorio remoto e una soglia numerica.
	1) In caso di successo, restituisce un intero positivo con il numero di file la cui dimensione supera la soglia inserita
	2) -1 altrimenti 
 
 
*/



// Usage: programName host 
int main(int argc, char *argv[]) {
	
	
	// Gestore di trasporto
	CLIENT *cl; 
	
	// Strutture definite in "operations.h"
	file_in file_input; 
	file_out *file_output;
	dir_in dir_input;
	
	// Variabili del programma
	char *server;
	char op;
	int *result;
	char temp[MAX_STR];


	
	// Controllo argomenti 
    if (argc != 2) {
        fprintf(stderr, "Usage: %s host\n", argv[0]);
        
        exit(EXIT_FAILURE);
    }
	
	// Assegnazione stringa Server
    server = argv[1];
	
	// Creazione gestore di trasporto
    cl = clnt_create(server, OPERATIONSPROG, OPERATIONSVERS, "udp");

	// Se non sono riuscito a creare il gestore di trasporto
    if(cl == NULL) {
		clnt_pcreateerror(server);
		
		exit(EXIT_FAILURE);
    }

	
	// Ciclo di richiesta del servizio da eseguire
	printf("Scrivi il nome dell'operazione che vuoi eseguire:\n\n[F]ile Scan - restituisce informazioni su un certo file\n[D]ir Scan - restituisce il numero di file la cui dimensione supera la soglia inserita\n\n[EOF per terminare]: ");
	while(scanf("%c", &op) != EOF) {
		
		switch(op) {
			
			// Label multiple, nel caso l'utente preme "f" oppure "F"
			case 'f': case 'F': {
				
				printf("Inserisci il nome del file: ");
				scanf("%s", file_input.fileName);
				
				printf("\nscanf eseguita!\n");
				
				file_output = file_scan_1(&file_input, cl);
				
				printf("\nprocedura eseguita!\n");
				
				// Se c'è stato un errore nella ricezione del risultato
				if(file_output == NULL){
					fprintf(stderr, "\nErrore durante la ricezione del risultato!\n");

					exit(EXIT_FAILURE);
				}
				
				
				printf("\nRisultato ottenuto:\nNumero caratteri: %d\nParole del file: %d\nRighe del file: %d\n", file_output->chars, file_output->strings, file_output->rows);
				
				
				break;
			}
			
			case 'd': case 'D': {
				
				printf("Inserisci il nome della directory remota: ");
				scanf("%s", dir_input.dirName);

				printf("\nInserisci la soglia desiderata: ");
				scanf("%d", &dir_input.num);
	
	
				printf("Valori inseriti:\nNome: %s\nSoglia: %d", dir_input.dirName, dir_input.num);
				
				printf("\nPrima della chiamata!\n");

				// Invocazione metodo remoto
				result = dir_scan_1(&dir_input, cl);
				
				printf("\nDopo della chiamata!\n");
				
				// Se c'è stato un errore nella ricezione del risultato
				if(result == NULL){
					perror("\nErrore durante la ricezione del risultato!\n");
					
					exit(EXIT_FAILURE);
				}
				
				// Controllo se la procedura remota è fallita
				if(*result == -1){
					fprintf(stderr, "\nErrore: la procedura remota 'Dir Scan' è fallita!\n");
				}else{
					printf("\nNumero di file la cui dimensione supera la soglia inserita: %d", *result);
				}
				
				
				break;
			}
			
			default: {
				
				printf("\nErrore: operazione non valida! Riprova!\n");
				
				break;
			}
			
		}

		printf("Scrivi il nome dell'operazione che vuoi eseguire:\n\n[F]ile Scan - restituisce informazioni su un certo file\n[D]ir Scan - restituisce il numero di file la cui dimensione supera la soglia inserita\n\n[EOF per terminare]: ");
	}
	
	
	
	printf("\nFine programma :]\n");
	
	return 0;
	
	
}