#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <math.h>
#include "graph.h"

// List_of_offset_deg_pairs [2N]
// N lists of D vertices

int N,M,D;
char* map;

/*void add_edge(int u, int v){
	//adds the edges to u and v's lists
	map[ 2*N + D*map[2*u] + map[2*u+1] ] = v+1;
	//increments the degrees
	map[ 2*u+1 ]++;
}*/

/*double log_two(int x) {
	
	double y = log(x)/log(2);
		
	return(y);
	
}*/

int main(int argc, char *argv[]) {

	//Expects a name of an file
	if(argc!=6){
		perror("fe N M D fd");
		exit(EXIT_FAILURE);
	}

	int fd;
	fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
	N = atoi(argv[2]);
	printf("N = %d\n", N);
	M = atoi(argv[3]);
	printf("M = %d\n", M);
	D = atoi(argv[4]);
	printf("D = %d\n", D);

	if (fd == -1){
		perror("error on opening file");
		exit(EXIT_FAILURE);
	}

	int PAGE_SIZE = 4096;
	int header_size = 20 + 8*N; //N, M, D(3) burn (2) [o,d]*N in 
	printf("The header is (%d x 2 + 5) X 4 = %d bytes\n", N, header_size);
	printf("The pages are %d bytes\n", PAGE_SIZE);
	
	printf("There are %f header pages\n", (double)header_size/PAGE_SIZE);
	
	double hpages = (double)header_size/PAGE_SIZE;
	printf("There are %f float header pages\n", hpages);

	int num_hpages = ceil(hpages);
	printf("Which is %d integer header pages\n", num_hpages);
	
	int padding = (num_hpages * 4096) - header_size; //in bytes
	
	
	int D_up;
	double junk = pow(2,ceil(log2(D)));
	D_up = junk;

	printf("The upper bound of D is %d\n", D_up);
	
	int node_data_size = 4*N*D_up; //in bytes

	
	//stretch the file in bytes
	size_t length = num_hpages*4096 + 4*D_up + node_data_size; // N, M, D; [n,o]*N + padding; burn row [N][D]
	if (lseek(fd, length+1, SEEK_SET) == -1){
		close(fd);
		perror("Error on lseek call to stretch the file");
		exit(EXIT_FAILURE);
	} //position at end of file
	
	double dpages = ((4*D_up + node_data_size)/(double)PAGE_SIZE);
	printf("There are %f double node pages\n", dpages);
	
	int num_dpages = ceil(dpages);
	printf("Which is %d int node pages\n", num_dpages);
	
	int num_tpages = num_hpages + num_dpages;

	printf("The length of this file is based on header %d + padding %d + node data %d + burned row %d = %d\n", header_size, padding, node_data_size, 4*D_up, length);
	
	printf("The length of the file is %d, requiring %d pages\n", length, num_tpages);

	//Needs to have something written so add an empty string 
	if (write(fd, "", 1) == -1){
		close(fd);
		perror("Error on writing last byte of file");
		exit(EXIT_FAILURE);
	}
 
	/*Open the map
	map = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if(map == MAP_FAILED){
		close(fd);
		perror("Error mapping file");
		exit(EXIT_FAILURE);
	}*/
	
	//int c,u,v, count=0;
	
	struct graph eGraph = {fd, N, M, D_up, num_hpages};
	
	FILE *fe;
	fe = fopen(argv[1], "r"); 
	if (fe == NULL){
		perror("error on opening file");
		return(-1);
	}
	
	int u,v;
	while(fscanf(fe, "%d %d", &u, &v)==2){
		
		add_edge(*eGraph, u, v);
		
	}
	
	
	fclose(fe);
	close(fd);

	exit(EXIT_SUCCESS);
}
