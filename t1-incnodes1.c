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
	if( argc!=3){
		perror("input undirected edge list and N");
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
	FILE *fw = fopen("edgelistonly.txt", "w");
	if (fw == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}
	
	//reprints edgelist for no reason
	//finds N and M
	int u, v;
	int m = 0;
	int N = atoi(argv[2]);
	printf("N = %d\n", N);
	
	
	while(fscanf(fd, "%d %d", &u, &v)==2){
		
		for (int i = 1; i<=N; i++) {
			
			int j
		u+=1;
		v+=1;
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
	//opens file to write to for final format
	FILE *fq = fopen("nmd_2n_el.txt", "w");
	if(fq == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}
	//writes N M
	fprintf(fq, "%d ", n);
	fprintf(fq, "%d ", m);
	
	int nodeArray[n+1];
	int maxD = 0;
	//initializes 0'd out nodeArray
	int j=0;
	for(j=0; j<=n+1; j++) {
		nodeArray[j]=0;
	}
	
	rewind(fd); //returns to beginning of edgelist input
	int a, b;
	//increases degree value per node
	while(fscanf(fd, "%d %d", &a, &b)==2) {
		//printf("Before ++ Node %d has degree %d\n", a, nodeArray[a]);
		//printf("Before ++ Node %d has degree %d\n", b, nodeArray[b]);
		nodeArray[a]++;
		nodeArray[b]++;
		//printf("After ++ Node %d has degree %d\n", a, nodeArray[a]);
		//printf("After ++ Node %d has degree %d\n", b, nodeArray[b]);
	
	}
	
	//finds D
	int i=0;
	for(i = 1; i < n+1; i++) {
		
		if(nodeArray[i]>maxD){
			maxD = nodeArray[i];
		}	
		
		//fprintf(fw, "%d,%d ", i, nodeArray[i]);
	
	}
	
	//writes D to output file
	fprintf(fq, "%d\n", maxD);
	for(i = 1; i < n+1; i++) {
		
		fprintf(fq, "%d,%d ", i, nodeArray[i]);
	
	}
	
	//writes edgelist again to end of file
	fprintf(fq, "\n");
	rewind(fd);
	
	while(fscanf(fd, "%d %d", &u, &v)==2){
		u+=1;
		v+=1;
		fprintf(fq, "%d %d\n", u, v);
	}
	
	fclose(fd);
	fclose(fw);
	fclose(fq);

	exit(EXIT_SUCCESS);
	
}
