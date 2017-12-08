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
	if( argc!=6){
		perror("input undirected edge list, holes file, and output file also # of holes, # of edges");
		exit(EXIT_FAILURE);
	}

	//open file to read from
	FILE *fd;
	fd = fopen(argv[1], "r"); 
	if (fd == NULL){
		perror("error on opening file");
		return(-1);
	}
	
	//open tmp file to write to
	FILE *fh = fopen(argv[2], "r");
	if (fh == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}
	
	FILE *fw = fopen(argv[3], "w");
	if (fw == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}
	
	FILE *fw1 = fopen("node1.txt", "w+");
	FILE *fw2 = fopen("node2.txt", "w+");
	
	int a,b;
	
	while(fscanf(fd, "%d %d", &a, &b) == 2){
		fprintf(fw1, "%d\n", a);
		fprintf(fw2, "%d\n", b);
	}
	
	int H = atoi(argv[4]);
	int M = atoi(argv[5]);
	
	int holes[H];
	
	int i = 0;
	int hnode, d;
	
	//find number of holes
	while(fscanf(fh, "%d %d", &hnode, &d)==2){
		holes[i] = hnode;
		i++;
	}
	
	rewind(fh);
	
	int hval = 0;
	int u,v;
	
	rewind(fw1);
	rewind(fw2);
	
	FILE *fwa = fopen("nodea.txt", "w+");
	FILE *fwb = fopen("nodeb.txt", "w+");

	while( fscanf(fw1, "%d", &a) == 1){
			
		int j = 0;
		while (holes[j]<a){
			j++;
		}
		fprintf(fwa, "%d\n", a-j+1);
			
	}
	
	while( fscanf(fw2, "%d", &b) == 1){
		
		int j = 0;
		while (holes[j]<b){
			j++;
		}
		fprintf(fwb, "%d\n", b-j+1);
			
	}
	
	rewind(fwa);
	rewind(fwb);
	
	while((fscanf(fwa, "%d", &a) == 1) && (fscanf(fwb, "%d", &b) == 1)) {
		fprintf(fw, "%d %d\n", a, b);
	}
	
	fclose(fd);
	fclose(fh);
	fclose(fw);
	fclose(fw1);
	fclose(fw2);
	fclose(fwa);
	fclose(fwb);


	exit(EXIT_SUCCESS);
	
}
