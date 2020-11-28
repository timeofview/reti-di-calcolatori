#include <stdio.h>
#include <stdlib.h>
#include <rpc/rpc.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "operations.h"
#define BUFF_DIM 256

file_out *file_scan_1_svc(file_in *file, struct svc_req *rp){
	
	int nread, i,  chars=0, words=0, rows=1;
	char buff[BUFF_DIM];	
	file_out result;
	FILE *fd;

	fd = fopen(file->fileName, "r");

	if(fd == NULL){
		fprintf(stderr, "errore nell'apertura del file");
		result.chars = -1;
		result.strings = -1;
		result.rows = -1;
		return (&result);
	}

	while(!feof(fd)){ //finchè l' I/O Pointer non è arrivato a fine file
		
		nread = fread(buff, sizeof(char), sizeof(buff), fd);
		for( i=0 ; i<nread-1; i++){
			chars++;
			if((buff[i+1]=='\n' || buff[i+1] =='\t' || buff[i+1]==' ') && 
					(buff[i]!='\n' && buff[i] !='\t' && buff[i]!=' ')){
				/*entro in questo if se trovo un carattere
					 seguito da un carattere "separatore" */
				words++;
			}
			
			if(buff[i] == '\n')
				rows++;

		}
	}

	result.chars = chars;
	result.strings = words;
	result.rows = rows;

	return (&result);
}



/* //METODO 1 uso di LSEEK : controllo solo i Regularfile
int *dir_scan_1_svc( dir_in *dirIn, struct svc_req *rp){
	
	DIR *d;
	struct dirent *dir;
	char path[BUFF_DIM];
	static int count=0;
	int fd;
	int size;
	d = opendir( dirIn->dirName);
	if(!d){
		fprintf(stderr, "Cannot open directory %s", dirIn->dirName);
		return -1;
	}
	
	printf("\t'%s' Directory correctly opened!\n", dirIn->dirName);
	
	while((dir = readdir(d)) !=NULL){
		if(dir->d_type == DT_REG){// se è un file (regolare)
			sprintf(path, "%s/%s", dirIn->dirName, dir->d_name);
			fd = fopen(path, "r");
			size = lseek(fd, 0, SEEK_END); // seek to end of file
			if(size >= dirIn->num)
				count++;
			close(fd);
		} 
	}

	return (&count);
}




*/
/*METODO 2: uso di stat()--> controllo tutto
richiede:
#include <sys/stat.h>
*/
int *dir_scan_1_svc( dir_in *dirIn, struct svc_req *rp){
	
	DIR *d;
	struct dirent *dir;
	struct stat st;
	static int count=0;
	char path[BUFF_DIM];

	d = opendir( dirIn->dirName);
	if(!d){
		fprintf(stderr, "Cannot open directory %s", dirIn->dirName);
		return -1;
	}
	
	printf("\t'%s' Directory correctly opened!\n", dirIn->dirName);
	while((dir = readdir(d)) !=NULL){
		if(dir->d_name[0] != '.'){
			sprintf(path, "%s/%s", dirIn->dirName, dir->d_name); // creo PATH = DirIn/NomeFile
			if(stat(path, &st)==0){ //prendo le info del file di PATH path
				if(&st.st_size > dirIn->num)
					count++;
			}else{
				perror("Impossibile aprire File");			
			}
		}	
	}
	return (&count);
}
