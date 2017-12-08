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
	if( argc!=2){
		perror("input undirected edge list");
		exit(EXIT_FAILURE);
	}

	//open file to read from
	FILE *fd;
	fd = fopen(argv[1], "r"); 
	if (fd == NULL){
		perror("error on opening file");
		return(-1);
	}
	
	int u,v;
	int n=0;
	while(fscanf(fd, "%d %d", &u, &v)==2){
		
		if(u > n) {
			n = u;
		} 
		if(v > n) {
			n = v;
		}
	}
	
	printf("Max N is %d\n", n);
	
	fclose(fd);
	

	exit(EXIT_SUCCESS);
	
}
