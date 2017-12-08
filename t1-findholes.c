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
		perror("input undirected edge list and max node N and output file");
		exit(EXIT_FAILURE);
	}

	//open file to read from
	FILE *fd;
	fd = fopen(argv[1], "r"); 
	if (fd == NULL){
		perror("error on opening file");
		return(-1);
	}
	
	int u, v;
	int N = atoi(argv[2]);
	
	char holes[N];
	//holes[0]=-1;
	printf("1 ");
	for (int i = 0; i < N; i++) {
		holes[i]=0;
	}
	printf("2 ");
	while(fscanf(fd, "%d %d", &u, &v)==2){
		holes[u-1] = 1;
		holes[v-1] = 1;
	}
	printf("3 ");
	FILE *fw;
	fw = fopen(argv[3], "w"); 
	if (fw == NULL){
		perror("error on opening file");
		return(-1);
	}
	printf("4 ");
	for (int i = 0; i<N; i++) {
		if(holes[i]==0){
			fprintf(fw, "%d %d\n", i+1, holes[i]);
			printf("%d %d\n", i+1, holes[i]);
		}
	}
	
	
	fclose(fd);
	fclose(fw);

	exit(EXIT_SUCCESS);
	
}
