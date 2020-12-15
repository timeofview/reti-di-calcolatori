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
Output *ranking_1_svc(struct svc_req *)
{
    if (!created)
    {
        getTable();
    }
    Output output;
    int arr[N];
    int count = 0;
    int notFound = 1;
    for (int i = 0; i < N; i++)
    {
        notFound = 1;
        for (int j = 0; j < count; j++)
        {
            if (!strcmp(table.candidate[i].judge_name, output.judges[j].judge_name))
            {
                arr[j] += table.candidate[i].score;
                notFound = 0;
            }
        }
        if (notFound)
        {
            output.judges[count].judge_name = (char *)malloc(sizeof(table.candidate[i].judge_name));
            strcpy(output.judges[count].judge_name, table.candidate[i].judge_name);
            arr[count++] = table.candidate[i].score;
        }
    }
    int tmp_int;
    Judge tmp_judge;
    int max;
    for (int i = 0; i < count; i++)
    {
        max = i;
        for (int j = i; j < count; j++)
        {
            if (arr[max] < arr[j])
            {
                max = j;
            }
        }
        if (max != i)
        {
            tmp_judge.judge_name = (char *)malloc(sizeof(output.judges[i].judge_name));
            strcpy(tmp_judge.judge_name, output.judges[i].judge_name);
            output.judges[i].judge_name = (char *)malloc(sizeof(output.judges[max]));
            strcpy(output.judges[i].judge_name, output.judges[max].judge_name);
            output.judges[max].judge_name = (char *)malloc(sizeof(tmp_judge.judge_name));
            strcpy(output.judges[max].judge_name, tmp_judge.judge_name);

            tmp_int = arr[i];
            arr[i] = arr[max];
            arr[max] = tmp_int;
        }
    }
    return &output;
}
