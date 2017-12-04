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
unsigned int* map;

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
	fd = open(argv[5], O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
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

	//int PAGE_SIZE = 4096;
	
	//TODO Replaced this to get the page size
	long PAGE_SIZE = sysconf(_SC_PAGESIZE);

	int header_size = sizeof(unsigned int)*(3 + 2*N); //N, M, D(3) [o,d]*N in 
	printf("The header is (%d x 2 + 5) X 4 = %d bytes\n", N, header_size);
	printf("The pages are %d bytes\n", PAGE_SIZE);
	
	printf("There are %f header pages\n", (double)header_size/PAGE_SIZE);
	
	double hpages = (double)header_size/PAGE_SIZE;
	printf("There are %f float header pages\n", hpages);

	int num_hpages = ceil(hpages);
	printf("Which is %d integer header pages\n", num_hpages);
	
	int padding = (num_hpages * PAGE_SIZE) - header_size; //in bytes
	
	
	int D_up;
	double junk = pow(2,ceil(log2(D)));
	D_up = junk;

	printf("The upper bound of D is %d\n", D_up);
	
	int node_data_size = sizeof(unsigned int)*N*D_up; //in bytes

	
	//stretch the file in bytes
	size_t length = num_hpages*PAGE_SIZE + sizeof(unsigned int)*D_up + node_data_size; // N, M, D; [n,o]*N + padding; burn row [N][D]
	if (lseek(fd, length+1, SEEK_SET) == -1){
		close(fd);
		perror("Error on lseek call to stretch the file");
		exit(EXIT_FAILURE);
	} //position at end of file
	
	double dpages = ((sizeof(unsigned int)*D_up + node_data_size))/(double)PAGE_SIZE;
	printf("There are %f double node pages\n", dpages);
	
	int num_dpages = ceil(dpages);
	printf("Which is %d int node pages\n", num_dpages);
	
	int num_tpages = num_hpages + num_dpages;

	printf("The length of this file is based on header %d + padding %d + node data %d + burned row %d = %d\n", header_size, padding, node_data_size, sizeof(int)*D_up, length);
	
	printf("The length of the file is %d, requiring %d pages\n", length, num_tpages);

	//Needs to have something written so add an empty string
	
	if (write(fd, "", 1) == -1){
		close(fd);
		perror("Error on writing last byte of file");
		exit(EXIT_FAILURE);
	}
 
	//Open the map
	map = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if(map == MAP_FAILED){
		close(fd);
		perror("Error mapping file");
		exit(EXIT_FAILURE);
	}
	
	//int c,u,v, count=0;

	FILE* fp = fdopen(fd,"r+");

	rewind(fp);
	
	int buf[3]={(unsigned int)N, (unsigned int)0, (unsigned int)D_up};
	
	fwrite(buf, sizeof(unsigned int), 3, fp);
	
	/*
	if (fprintf(fp,"%d",(int)0) < 0 ) {
        	close(fd);
		perror("Error on writing M to file");
		exit(EXIT_FAILURE);
	} //else {printf("wrote 0 to file\n");}
	
	if ( fprintf(fp,"%d",(int)D_up) < 0 ) {
        	close(fd);
		perror("Error on writing D to file");
		exit(EXIT_FAILURE);
	} //else {printf("wrote %d to file\n", D_up);}
	*/
	//TODO print the whole file here and look at it?

	//printf("printing back the file should have NMD\n");

	rewind(fp);
	
	
	/*
	int c;
	c=fread(buf, sizeof(int), 1, fp);
	printf("N:%d\n",c);

	c=fread(buf, sizeof(int), 1, fp);
	printf("M:%d\n",c);
	
	c=fread(buf, sizeof(int), 1, fp);
	printf("D:%d\n",c);*/

	struct graph* my_graph = Graph(fd);
	
	FILE *fe;
	fe = fopen(argv[1], "r"); 
	if (fe == NULL){
		perror("error on opening file");
		return(-1);
	}
	
	//initialize nodes with offset and degree 0
	for(unsigned int i=0; i<my_graph->N ;i++){
		my_graph->g[3+2*i]=i;
		my_graph->g[3+2*i+1]=0;
	}

	print_graph(my_graph);
	
	int u,v;
	while(fscanf(fe, "%d %d", &u, &v)==2){
		printf("Adding edge %d %d in e2g\n", u, v);
		add_edge(my_graph, u, v);
		print_graph(my_graph);	
	}

	
	print_graph(my_graph);

	fclose(fe);
	close_graph(my_graph);
	close(fd);
	fclose(fp);

	exit(EXIT_SUCCESS);
}
