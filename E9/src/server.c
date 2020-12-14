#include "VotaFattoreX.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define N 15
#define CANDIDATE "candidate"
#define JUDGE "judge"

static Table table;
static Output output;
static int created = 0;

Table getTable()
{

    if (created)
    {
        return table;
    }
    char num[255];
    char candidate[strlen(CANDIDATE) + 3];
    char judge[strlen(JUDGE) + 3];
    for (int i = 0; i < N; i++)
    {
        /* char candidate []= "candidate";
        sprintf(num, "%d", i);
        strcat(num, "candidate");
        strcpy(table.candidate[i].candidate_name, num);
        strcpy(table.candidate[i].judge_name, strcat("judge", candidate));
        table.candidate[i].category = 'F';
        table.candidate[i].phase = 'F';
        table.candidate[i].fileName = "orco.dio";
        table.candidate[i].score = i;*/
        sprintf(num, "%d", i);
        sprintf(candidate, "%s%d", CANDIDATE, i);
        sprintf(judge, "%s%d", JUDGE, i);
        Candidate tmp;
        table.candidate[i].candidate_name = (char *)malloc(strlen(candidate));
        table.candidate[i].judge_name = (char *)malloc(strlen(judge));
        strcpy(table.candidate[i].candidate_name, candidate);
        strcpy(table.candidate[i].judge_name, judge);
        table.candidate[i].category = 'f';
        table.candidate[i].fileName = "orco.dio";
        table.candidate[i].phase = 'f';
        table.candidate[i].score = i;
    }
    created = 1;
    return table;
}
