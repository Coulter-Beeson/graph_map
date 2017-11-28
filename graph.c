#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "graph.h"


//TODO ASSUMES that the file is already a created graph
//Should add capabilities to use Graph(int N, int D)
//that creates its own empty file and turns it into an empty graph
//of the correct size`
struct graph* Graph(int fd){

	//TODO add error checking to see if the file is an existing
	int length = 0;
	//Open the map

	char* map = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if(map == MAP_FAILED){
		close(fd);
		perror("Error mapping file");
		exit(EXIT_FAILURE);
	}

	struct graph *g = malloc(sizeof(struct graph));

	g->g = map;
	g->N = map[0];
	g->M = map[1];
	g->D = map[2];

	return g;
}

bool get_edge(struct graph* g, int u, int v){
	int d = get_deg(g,u);
	int o = get_off(g,u);

	for(int i=0; i<d; i++){
		if(g->g[3 + 2*g->N + g->D*o + i] == v){
			return true;
		}
	}

	return false;
}

void add_edge(struct graph* g, int u, int v){
	
	if(get_edge(g,u,v)) return;

	inc_deg(g,u);
	
	int n = g->N;
	int d = g->D;

	g->g[3 + 2*n + d*get_off(g,u) + get_deg(g,u)] = v;
}

int get_off(struct graph* g, int u){
	return g->g[3 + 2*u];
}

int get_deg(struct graph* g, int u){
	return g->g[3 + 2*u+1];
}

void inc_deg(struct graph* g, int u){
	g->g[3 + 2*u+1]++;
}



int main(int argc, char* argv[]){

	//Expects a name of an file
	if(argc != 2){
		perror("takes a file pointer to a graph");
		exit(EXIT_FAILURE);
	}

	int fd;
	fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);

	if (fd == -1){
		perror("error on opening file");
		exit(EXIT_FAILURE);
	}

	//create test graph
	struct graph* G = Graph(fd);	
	int test_case_count = 0;

	//=======================Test Cases========================

	printf("Beginging test case %n", test_case_count); 

	//test add_edge

	//TODO add a graph clsoe function
	//TODO figure out length 
	int length = 0;
	if (msync(G->g, length, MS_SYNC) == -1){
		perror("couldn't sync to disk");
	}

	if (munmap(G->g, length) == -1){
		close(fd);
		perror("Error unmappin");
		exit(EXIT_FAILURE);
	}

	close(fd);

	exit(EXIT_SUCCESS);	
}
