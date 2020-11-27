typedef struct
{
    char *fileName;
} file_in;
typedef struct
{
    char *diranme;
    int num;
} dir_in;
typedef struct
{
    int chars;
    int strings;
    int rows;
} file_out;

file_out file_scan(file_in in);
int dir_scan(dir_in in);
