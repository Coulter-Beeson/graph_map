#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <math.h>
#include "graph.h"

unsigned long* map;

int main(int argc, char* argv[]){

	if(argc!=3){
		perror("degree # (min 1) AND star = 1, K-complete = 2");
		exit(EXIT_FAILURE);
	}

	char strlist[20];
	char strgraph[20];
	strcpy(strlist,"edgeLists/" );
	strcpy(strgraph,"Gfiles/" );
	strcat(strlist, argv[1]);
	strcat(strgraph, argv[1]);
	//printf("%s\n", strlist);
	int mode = atoi(argv[2]);
	int D = atoi(argv[1]);
	int N = D+1;
	int M=0;
	//printf("%s\n", str);
		
	if (mode == 1) {
		strcat(strlist, "-star.txt");
		strcat(strgraph, "-star.g");
		printf("%s\n", strlist);
	}
	if (mode == 2){
		strcat(strlist, "-complete.txt");
		strcat(strgraph, "-complete.g");
		printf("%s\n", strlist);
	}
	
	FILE *fg;
	fg = fopen(strlist, "w+"); 
	if (fg == NULL){
		perror("error on opening file");
		return(-1);
	}
	//write STAR graph
	if (mode == 1) {
		for (int i = 2; i < N+1; i++) {
			fprintf(fg, "%d %d\n", 1, i);
			M++;
		}
	}
	//write COMPLETE graph
	if (mode == 2){
		
		for (int j = 1; j < N; j++) {
		
			for (int k = j+1; k < N+1; k++) {
			
				if (k != j) {
					fprintf(fg, "%d %d\n", j, k);
					M++;
				}
			
			}
		
		}
	}
	
	fclose(fg);
	
	int fd;
	fd = open(strgraph, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
	
	if (fd == -1){
		perror("error on opening file");
		exit(EXIT_FAILURE);
	}

	int PAGE_SIZE = sysconf(_SC_PAGESIZE);

	unsigned long header_size = sizeof(int)*(3 + 3*N); //N, M, D(3) [o,d]*N in 
	//printf("The header is %d bytes\n", N, header_size);
	//printf("The pages are %d bytes\n", PAGE_SIZE);
	
	//printf("There are %f header pages\n", (double)header_size/PAGE_SIZE);
	
	double hpages = (double)header_size/PAGE_SIZE;
	//printf("There are %f float header pages\n", hpages);

	unsigned long num_hpages = ceil(hpages);
	//printf("Which is %d integer header pages\n", num_hpages);
	
	//unsigned long padding = (num_hpages * PAGE_SIZE) - header_size; //in bytes
	
	
	unsigned long D_up;
	double junk = pow(2,ceil(log2(D)));
	D_up = junk;

	//printf("The upper bound of D is %d\n", D_up);
	
	unsigned long node_data_size = sizeof(int)*N*D_up; //in bytes

	
	//stretch the file in bytes
	size_t length = num_hpages*PAGE_SIZE + node_data_size; // N, M, D; [n,o]*N + padding; + N*D
	if (lseek(fd, length+1, SEEK_SET) == -1){
		close(fd);
		perror("Error on lseek call to stretch the file");
		exit(EXIT_FAILURE);
	} //position at end of file
	
	double dpages = ((sizeof(int)*D_up + node_data_size))/(double)PAGE_SIZE;
	//printf("There are %f double node pages\n", dpages);
	
	unsigned long num_dpages = ceil(dpages);
	//printf("Which is %d int node pages\n", num_dpages);
	
	unsigned long num_tpages = num_hpages + num_dpages;

	//printf("The length of this file is based on header %d + padding %d + node data %d + burned row %d = %d\n", header_size, padding, node_data_size, sizeof(int)*D_up, length);
	
	//printf("The length of the file is %d, requiring %d pages\n", length, num_tpages);

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
	
	int buf[3]={(int)N, (int)0, (int)D_up};
	
	fwrite(buf, sizeof(int), 3, fp);
	
	
	rewind(fp);
	
	
	int c;
	c=fread(buf, sizeof(int), 1, fp);
	
	c=fread(buf, sizeof(int), 1, fp);
		
	c=fread(buf, sizeof(int), 1, fp);
	
	struct graph* my_graph = Graph(fd);
	
	FILE *fe;
	fe = fopen(strlist, "r"); 
	if (fe == NULL){
		perror("error on opening file");
		return(-1);
	}
	
	//initialize nodes with offset and degree 0
	for(int i=0; i<my_graph->N ;i++){
		my_graph->map[3+2*i]=i;
		my_graph->map[3+2*i+1]=0;
		my_graph->map[3+2*my_graph->N+i]=i+1;
	}

	//print_graph(my_graph);
	
	int u,v;
	while(fscanf(fe, "%d %d", &u, &v)==2){
		//printf("Adding edge %d %d in e2g\n", u, v);
		add_edge(my_graph, u, v);
		//print_graph(my_graph);	
	}

	
	//print_graph(my_graph);

	close_graph(my_graph);

	//struct graph* g2 = Graph(fd);

	//print_graph(g2);
	//close_graph(g2);	


	fclose(fe);
	close(fd);
	fclose(fp);

	
	exit(EXIT_SUCCESS);
}
