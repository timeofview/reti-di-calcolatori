const MAXSTRLEN = 255;
const N = 15;

/* Struttura di Input */
struct Input{ 
	char candidate_name[MAXSTRLEN]; 
	char operation_name[MAXSTRLEN]; 
};

/* Struttura dei giudici */
struct Judge{
	char judge_name[MAXSTRLEN];
};

/* Struttura di Output */
struct Output{ 
	Judge judges[N];
};

/* Struttura dei candidati */
struct Candidate{
	char candidate_name[MAXSTRLEN]; 
	char judge_name[MAXSTRLEN];
	char category;
	char fileName[MAXSTRLEN];
	char phase;
	int score;
};

/* Struttura che indica la tabella di candidati */
struct Table{
	Candidate candidate[N];
};

/* Definizione programma remoto */
program VOTAFATTORE { /* Nome programma remoto */
	version VOTAFATTOREVERS { /* Versione programma remoto */
		Output RANKING(void) = 1; /* Funzione remota numero 1 */
		int VOTE(Input) = 2; /* Funzione remota numero 2 */
	} = 1; /* Numero versione del programma remoto */
} = 0x20000013; /* Program number */
