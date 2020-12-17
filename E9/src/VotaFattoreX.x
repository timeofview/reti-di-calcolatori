const MAXSTRLEN = 255;
const N = 15;

struct Input{ 
	char candidate_name[MAXSTRLEN]; 
	char operation_name[MAXSTRLEN]; 
};

struct Judge{
	char judge_name[MAXSTRLEN];
};

struct Output{ 
	Judge judges[N];
};

struct Candidate{
	char candidate_name[MAXSTRLEN]; 
	char judge_name[MAXSTRLEN];
	char category;
	char fileName[MAXSTRLEN];
	char phase;
	int score;
};

struct Table{
	Candidate candidate[N];
};

program VOTAFATTORE {
	version VOTAFATTOREVERS {
		int VOTE(Input) = 1;
		Output RANKING(void) = 2;
	} = 1;
} = 0x20000013;
