const MAXSTRLEN = 15;
const N = 15;

struct Input{ 
	char candidate_len[MAXSTRLEN]; 
	char operation_name[MAXSTRLEN]; 
};
struct Judge{
	char judge_name[MAXSTRLEN];
};
struct Output{ 
	Judge judges[N];
};
program VOTAFATTORE {
	version VOTAFATTOREVERS{
		Output CLASSIFICA_GIUDICI(void) = 1;
		int ESPRIMI_VOTO(Input) = 2;
	} = 1;
} = 0x20000013;