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
	if( argc!=4){
		perror("input: degree list, threshold, outlist");
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
	ft = fopen(argv[3], "w"); 
	if (ft == NULL){
		perror("error on opening file");
		return(-1);
	}
	
	int threshold = atoi(argv[2]);
	int u,v;
	int n=0;
	while(fscanf(fd, "%d %d", &u, &v)==2){
		
		if(v >= threshold) {
			printf("%d: %d\n", u, v);
			fprintf(ft, "%d\n", u);
			n++;
		}
	}
	printf("# of nodes that exceed the threshold = %d\n", n);
	fclose(fd);
	fclose(ft);

	exit(EXIT_SUCCESS);
	
}
