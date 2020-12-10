#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <rpc/rpc.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "operations.h"

#define BUFF_DIM 256


file_out *file_scan_1_svc(file_in *file, struct svc_req *rp){
	
	
	int fd, nread, i,  chars=0, words=0, rows=1;
	char buff[BUFF_DIM];	
	static file_out result;



	printf("Metodo file_scan lanciato!\n");
	
	fd = open(file->fileName, O_RDONLY);
	
	if(fd < 0){
		fprintf(stderr, "Errore: apertura del file!");
		result.chars = -1;
		result.strings = -1;
		result.rows = -1;
		return (&result);
	}
	
	
	printf("File aperto con successo!\n");
	
	// Finché l' I/O Pointer non è arrivato a fine file
	while((nread = read(fd, buff, sizeof(char) * BUFF_DIM)) > 0){ 

		for(i=0 ; i<nread-1; i++){
			
			chars++;
			
			// Entro in questo if se trovo un carattere seguito da un carattere "separatore"
			if((buff[i+1]=='\n' || buff[i+1] == '\t' || buff[i+1] == ' ') && (buff[i] != '\n' && buff[i] != '\t' && buff[i] !=' ')) {	
				words++;
			}
			
			if(buff[i] == '\n')
				rows++;

		}
	}

	result.chars = chars;
	result.strings = words;
	result.rows = rows;

	close(fd);
	
	printf("Sono prima del return!\n");
	
	return (&result);
}


/*METODO 2: uso di stat()--> controllo tutto
richiede:
#include <sys/stat.h>
*/
int *dir_scan_1_svc(dir_in *dirIn, struct svc_req *rp){
	
	DIR *d;
	struct dirent *dir;
	struct stat st;
	static int count = 0;
	static int error = -1;
	char path[BUFF_DIM];
	
	
	printf("Metodo dir_scan lanciato!\n");
	
	d = opendir(dirIn->dirName);
	
	if(!d){
		fprintf(stderr, "Errore: impossibile aprire %s!", dirIn->dirName);
		return (&error);
	}
	count=0;
	printf("'%s' Directory aperta correttamente!\n", dirIn->dirName);
	while((dir = readdir(d)) != NULL){
		
		if(dir->d_name[0] != '.'){
			
			// creo PATH = DirIn/NomeFile
			sprintf(path, "%s/%s", dirIn->dirName, dir->d_name); 
			
			if(stat(path, &st) == 0){ //prendo le info del file di PATH path
				
				if(st.st_size > dirIn->num)
					count++;
			}else{
				perror("Impossibile aprire il file!");			
			}
		}	
	}
	
	printf("Fine metodo dir_scan!\n");
	
	closedir(d);
	
	return (&count);
}