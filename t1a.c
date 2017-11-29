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
	
	//open file to write to
	FILE *fw = fopen("nmd_2n_g.txt", "w");
	if (fw == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}
	
	int u, v;
	int m = 0;
	int n = 0;
	while(fscanf(fd, "%d %d", &u, &v)==2){
		fprintf(fw, "%d %d\n", u, v);
		//fprintf(fw, "%d %d\n", v, u); //prints the reverse	
		m++;
		if(u > n) {
			n = u;
		} 
		if(v > n) {
			n = v;
		}
	}
	fprintf(fw, "N = %d\n", n);
	fprintf(fw, "M = %d\n", m);
	fclose(fd);
	fclose(fw);

	exit(EXIT_SUCCESS);
	
}