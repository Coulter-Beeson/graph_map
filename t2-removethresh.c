#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>



int main(int argc, char *argv[]) {

	//Expects a name of an file
	if( argc!=5){
		perror("input: edge list, threshold #, over list, new list");
		exit(EXIT_FAILURE);
	}
	
	//open file to read from
	FILE *fd;
	fd = fopen(argv[1], "r"); 
	if (fd == NULL){
		perror("error on opening file");
		return(-1);
	}
	
	FILE *ft;
	ft = fopen(argv[3], "r"); 
	if (ft == NULL){
		perror("error on opening file");
		return(-1);
	}
	
	FILE *fw;
	fw = fopen(argv[4], "w"); 
	if (fw == NULL){
		perror("error on opening file");
		return(-1);
	}
	
	
	int n = atoi(argv[2]);
	int u,v,t;
	int T[n];
	int toPrint = 1;
	
	for (int i = 0; i<n; i++) {
		if (fscanf(ft, "%d", &t)==1){
			printf("tracking %d\n", t);
			T[i]=t;
		}
	}
	
	while(fscanf(fd, "%d %d", &u, &v)==2){
		toPrint = 1;
		for (int i = 0; i<n; i++) {
			if ((u == T[i]) || (v == T[i])) {
				toPrint = 0;
			}
			
		}
		if (toPrint == 1) {
			fprintf(fw, "%d %d\n", u, v);
		}
	}
	printf("# of nodes that exceed the threshold = %d\n", n);
	fclose(fd);
	fclose(ft);
	fclose(fw);
	

	exit(EXIT_SUCCESS);
	
}
