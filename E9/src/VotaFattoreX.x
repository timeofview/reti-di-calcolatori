const MAXSTRLEN = 255;
const N = 15;

struct Input{ 
	string candidate_name<MAXSTRLEN>; 
	string operation_name<MAXSTRLEN>; 
};
struct Judge{
	string judge_name<MAXSTRLEN>;
};
struct Output{ 
	Judge judges[N];
};
struct Candidate{
string candidate_name<MAXSTRLEN>; 
string judge_name<MAXSTRLEN>;
char category;
string fileName<MAXSTRLEN>;
char phase;
int vote;
};
struct Table{
	Candidate candidate [N];
};
program VOTAFATTORE {
	version VOTAFATTOREVERS{
		Output ranking(void) = 1;
		int vote(Input) = 2;
	} = 1;
} = 0x20000013;