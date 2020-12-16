#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "VotaFattoreX.h"

#define N 15
#define CANDIDATE "candidate"
#define JUDGE "judge"

static Table table;
static Output output;
static int created = 0;

/*
insert 2 value in table
*/
void fill(){
    int i;
    for (i=0; i<2; i++){
        char *new_str ;
        char *str1;
        char *str2=".txt";
        if(i==0){
            str1="Luca";
            if((new_str = malloc(strlen(str1)+strlen(str2)+1)) != NULL){
                new_str[0] = '\0';
                strcat(new_str,str1);
                strcat(new_str,str2);
            } else {
                fprintf(STDERR,"malloc failed!\n");
            }
            table.candidate[i].candidate_name=str1;
            table.candidate[i].judge_name="Luca_Judge";
            table.candidate[i].category="U";
            table.candidate[i].fileName=new_str;
            table.candidate[i].phase="A";
            table.candidate[i].score=50;
        }
        else{
            str1="Lucia";
            if((new_str = malloc(strlen(str1)+strlen(str2)+1)) != NULL){
                new_str[0] = '\0';
                strcat(new_str,str1);
                strcat(new_str,str2);
            } else {
                fprintf(STDERR,"malloc failed!\n");
            }
            table.candidate[i].candidate_name=str1;
            table.candidate[i].judge_name="Lucia_Judge";
            table.candidate[i].category="D";
            table.candidate[i].fileName="L";
            table.candidate[i].phase="S";
            table.candidate[i].score=55;
        }
    }
}

void init(){ //just 1 time
    int i;
    if(created == 1) return;

    //init table
    for(i = 0; i < N; i++){
        table.candidate[i].candidate_name="L";
        table.candidate[i].judge_name="L";
        table.candidate[i].category="L";
        table.candidate[i].fileName="L";
        table.candidate[i].phase="L";
        table.candidate[i].score=-1;
    }

    //fill value
    fill();

    created = 1;
    printf("Inited!\n");
}

/*
accept candidate and operation,
update table,
return result
*/
int *vote_1_svc(Input *in,struct svc_req *rp){
    int result = 1;

    //if it's not initialized create
    if (!created){
        init();
        return result;
    }

    int i;
    int exist=-1;
    Candidate tmp;
    tmp.candidate_name=in->candidate_name;
    int op;
    if (in->operation_name=="add")
        op=1;
    if (in->operation_name=="sub")
        op=2;

    //foreach entry in table
    for (i = 0; i < N; i++){
        if(strcmp(table.candidate[i].candidate_name,tmp.candidate_name)==0)
            exist=i;
    }  

    if(exist>=0){
        if(op==1)
            table.candidate[exist].score++;
        else
            table.candidate[exist].score--;
        if (table.candidate[exist].score<0>||table.candidate[exist].score>100=
            result=-1;
    }
    else{//da implementare?}
    
    return result;
}

/*
Return Output struct,
sorted by candidate rank
*/
Output *ranking_1_svc(struct svc_req *rp){
    //if it's not initialized create
    if (!created){
        init();
    }

    //var init
    Output output;
    int arr[N];
    int count = 0;
    int notFound = 1;

    //foreach entry in arr
    for (int i = 0; i < N; i++){
        notFound = 1;
        //count rank foreach judge
        for (int j = 0; j < count; j++){
            if (!strcmp(table.candidate[i].judge_name, output.judges[j].judge_name)){
                arr[j] += table.candidate[i].score;
                notFound = 0;
            }
        }

        //if notFound add an entry
        if (notFound){
            output.judges[count].judge_name = (char *)malloc(sizeof(table.candidate[i].judge_name));
            strcpy(output.judges[count].judge_name, table.candidate[i].judge_name);
            arr[count++] = table.candidate[i].score;
        }
    }

    //var init for sorting
    int tmp_int;
    Judge tmp_judge;
    int max;

    //finding the max
    for (int i = 0; i < count; i++){
        max = i;
        for (int j = i; j < count; j++){
            if (arr[max] < arr[j]){
                max = j;
            }
        }

        //if not max -> swap
        if (max != i){
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
    return (&output);
}
