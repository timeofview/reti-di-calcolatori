#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "VotaFattoreX.h"

#define MAX_STR 255
#define N 15
#define CANDIDATE "candidate"
#define JUDGE "judge"

// Da ritornare
static Table table;

static int created = 0;

// Puts 2 values in the table
void fill() {

    int i;

    for (i = 0; i < 2; i++) {
        char new_str[MAX_STR];
        char str1[MAX_STR];
        
        if (i == 0) {

            strcpy(str1, "Luca");
            sprintf(new_str, "%s.txt", str1); 

            
            strcpy(table.candidate[i].candidate_name, str1);
            strcpy(table.candidate[i].judge_name, "Luca_Judge");
            table.candidate[i].category = 'U';
            strcpy(table.candidate[i].fileName, new_str);
            table.candidate[i].phase = 'A';
            table.candidate[i].score = 50;
        }
        else {
			
            strcpy(str1, "Lucia");
           
            sprintf(new_str, "%s.txt", str1);

            strcpy(table.candidate[i].candidate_name, str1);
            strcpy(table.candidate[i].judge_name,"Lucia_Judge");
            table.candidate[i].category = 'D';
            strcpy(table.candidate[i].fileName, new_str);
            table.candidate[i].phase = 'S';
            table.candidate[i].score = 55;
        }

    }
    
}

// Executed once
void init() {
	
    int i;
    if (created == 1)
        return;

    //init table
    for (i = 0; i < N; i++) {
        strcpy(table.candidate[i].candidate_name, "L");
        strcpy(table.candidate[i].judge_name, "L");
        table.candidate[i].category = 'L';
        strcpy(table.candidate[i].fileName, "L");
        table.candidate[i].phase = 'L';
        table.candidate[i].score = -1;
    }

    //fill value
    fill();

    created = 1;
    printf("Inited!\n");
}

// Accept candidate and operation,
// Update table,
// Returns result
int* vote_1_svc(Input* in, struct svc_req* rp) {

    int result = 1;

    //if it's not initialized create
    if (!created) {
        init();
        return result;
    }

    int i;
    int exist = -1;
    Candidate tmp;
    strcpy(tmp.candidate_name,  in->candidate_name);
    
    int op;
    if (strcmp(in->operation_name, "add") == 0)
        op = 1;
    if (strcmp(in->operation_name, "sub") == 0)
        op = 2;

    // For each entry in table
    for (i = 0; i < N; i++) {
        if (strcmp(table.candidate[i].candidate_name, tmp.candidate_name) == 0)
            exist = i;
    }

    if (exist >= 0) {
		
        if (op == 1) {
            table.candidate[exist].score++;
        }else {
            table.candidate[exist].score--;
        }

        if (table.candidate[exist].score < 0 || table.candidate[exist].score > 100) {
            result = -1;
        }
    }
    else { // Da implementare?
    }

    return result;
}

// Returns Output struct sorted by candidate rank
Output *ranking_1_svc(void *p, struct svc_req *rp) {
	
	static Output output;
	
	
    // If it's not initialized, then create one
    if (!created) {
        init();
    }

    // Var init
    int arr[N];
    int count = 0;
    int notFound = 1;
    int i;
	
	printf("Sono prima del primo ciclo\n");
	
	// Inizializzo boh
	for(i=0; i<N; i++){
		strcpy(output.judges[i].judge_name, "L");
	}
	
	printf("Sono prima del secondo ciclo\n");
	
    // Foreach entry in array
    for (i = 0; i < N; i++) {
		
        notFound = 1;
        
        // Count rank foreach judge
        for (int j = 0; j < count; j++) {
            if (!strcmp(table.candidate[i].judge_name, output.judges[j].judge_name)) {
                arr[j] += table.candidate[i].score;
                notFound = 0;
            }
        }

        // If notFound add an entry
        if (notFound) {
            strcpy(output.judges[count].judge_name, table.candidate[i].judge_name);
            arr[count++] = table.candidate[i].score;
        }
    }

    // Var init for sorting
    int tmp_int;
    Judge tmp_judge;
    int max;
	
	printf("Sono prima del terzo ciclo!\n");
	
    // Finding the max
    for (i = 0; i < count; i++) {
        max = i;
        for (int j = i; j < count; j++) {
            if (arr[max] < arr[j]) {
                max = j;
            }
        }

        // If not max -> swap
        if (max != i) {
            strcpy(tmp_judge.judge_name, output.judges[i].judge_name);
            strcpy(output.judges[i].judge_name, output.judges[max].judge_name);
            strcpy(output.judges[max].judge_name, tmp_judge.judge_name);

            tmp_int = arr[i];
            arr[i] = arr[max];
            arr[max] = tmp_int;
        }
    }
    
    printf("Sono prima del quarto ciclo!\n");
    
    // Ora stampo a video tutti i giudici
	for(i = 0; i<N; i++){
		printf("Sono nel ciclo %d\n", i);
		printf("Nome giudice: %s\n", output.judges[i].judge_name);
	}
    
    printf("Sono prima del return!\n");
    
    return (&output);
}
