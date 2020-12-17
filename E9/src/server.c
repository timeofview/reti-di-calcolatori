#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <rpc/rpc.h>
#include <sys/dir.h>

#include "VotaFattoreX.h"

static Table table;
static int created = 0;

void fill() {
    int i;

    for (i = 0; i < 2; i++) {
        char new_str[MAXSTRLEN];
        char str1[MAXSTRLEN];
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

void init() {
    if (created == 1)
        return;

    int i;

    for (i = 0; i < N; i++) {
        strcpy(table.candidate[i].candidate_name, "L");
        strcpy(table.candidate[i].judge_name, "L");
        table.candidate[i].category = 'L';
        strcpy(table.candidate[i].fileName, "L");
        table.candidate[i].phase = 'L';
        table.candidate[i].score = -1;
    }
    fill();
    created = 1;
    printf("Inited!\n");
}

int* vote_1_svc(Input* in, struct svc_req *rqstp) {
    init();
    static int result = 1;
    int i, op, exist = -1;
    
    if (strcmp(in->operation_name, "add") == 0)
        op = 1;
    if (strcmp(in->operation_name, "sub") == 0)
        op = 2;

    for (i = 0; i < N; i++)
        if (strcmp(table.candidate[i].candidate_name, in->candidate_name) == 0)
            exist = i;

    if (exist >= 0) {
        if (op == 1)
            table.candidate[exist].score++;
        else
            table.candidate[exist].score--;

        if (table.candidate[exist].score < 0 || table.candidate[exist].score > 100) 
            result = -1;
    }
    else 
        result = -1;
    printf("Sending Result\n");
    return (&result);
}

Output* ranking_1_svc(void *in, struct svc_req *rqstp) {
	init();    
	static Output output;
    int count = 0, notFound = 1;
    int i, j, arr[N];
        
    for (i = 0; i < N; i++) {
        notFound = 1;
        
        for (j = 0; j < count; j++) {
            if (!strcmp(table.candidate[i].judge_name, output.judges[j].judge_name)) {
                arr[j] += table.candidate[i].score;
                notFound = 0;
            }
        }
        if (notFound) {
            strcpy(output.judges[count].judge_name, table.candidate[i].judge_name);
            arr[count++] = table.candidate[i].score;
        }
    }

    int tmp_int, max;
    Judge tmp_judge;
		
    for (i = 0; i < count; i++) {
        max = i;
        for (int j = i; j < count; j++) {
            if (arr[max] < arr[j]) {
                max = j;
            }
        }

        if (max != i) {
            strcpy(tmp_judge.judge_name, output.judges[i].judge_name);
            strcpy(output.judges[i].judge_name, output.judges[max].judge_name);
            strcpy(output.judges[max].judge_name, tmp_judge.judge_name);

            tmp_int = arr[i];
            arr[i] = arr[max];
            arr[max] = tmp_int;
        }
    }
    printf("Sending Judges Ranking\n");
    return (&output);
}
