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
	
	//reprints edgelist for no reason
	//finds N and M
	int u, v;
	int m = 0;
	int n = 0;
	while(fscanf(fd, "%d %d", &u, &v)==2){
		
		m++;
		if(u > n) {
			n = u;
		} 
		if(v > n) {
			n = v;
		}
	}
	
	printf("M = %d\n", m);
	printf("N = %d\n", n);
	
		
	short nodeArray[n];
	printf("initialize nodeArray\n");
	int maxD = 0;
	//initializes 0'd out nodeArray
	int j=0;
	for(j=0; j<n; j++) {
		nodeArray[j]=0;
	}
	printf("zero out nodeArray\n");
	
	
	rewind(fd); //returns to beginning of edgelist input
	int a, b;
	//increases degree value per node
	while(fscanf(fd, "%d %d", &a, &b)==2) {
		//printf("Before ++ Node %d has degree %d\n", a, nodeArray[a-1]);
		//printf("Before ++ Node %d has degree %d\n", b, nodeArray[b-1]);
		nodeArray[a-1]++;
		nodeArray[b-1]++;
		//printf("After ++ Node %d has degree %d\n", a, nodeArray[a-1]);
		//printf("After ++ Node %d has degree %d\n", b, nodeArray[b-1]);
	
	}
	
	//finds D
	int i=0;
	for(i = 0; i < n; i++) {
		
		if(nodeArray[i]>maxD){
			maxD = nodeArray[i];
		}	
		
	}
	
	//prints D 
	printf("D = %d\n", maxD);
	
	
	fclose(fd);
	

	exit(EXIT_SUCCESS);
	
}