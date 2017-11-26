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

	FILE *fd;
	fd = fopen(argv[1], "r");
	//int length;
	
	
	if (fd == NULL){
		perror("error on opening file");
		return(-1);
	}
	//rewind(fd);
	int u, v;
	while(fscanf(fd, "%d %d", &u, &v)==2){
		printf("%d %d\n", u, v);
		printf("%d %d\n", v, u); //prints the reverse	
	}
		
	fclose(fd);

	exit(EXIT_SUCCESS);
	
}