#include <stdio.h>
#include "graph.h"


struct graph{
	char* g;
	int N;
	int M;
	int D;
};


graph* Graph(char* map, int n, int m, int d){

	graph *g = malloc(sizeof(graph));

	g->g = map;
	g->N = n;
	g->M = m;
	g->D = d;

	return g;
}

bool get_edge(struct graph* g, int u, int v){
	int d = get_deg(g,u);
	int o = get_off(g,u);

	for(int i=0; i<d; i+){
		if(g->g[2*g->N +g->D*o + i] == v){
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

	g->g[2*n + d*get_off(g,u) + get_deg(g,u)] = v;
}

int get_off(struct graph* g, int u){
	return g->g[2*u];
}

int get_deg(struct graph* g, int u){
	return g->g[2*u+1];
}

void inc_deg(struct graph* g, int u){
	g->g[2*u+1]++;
}



int main(int argc, char** argv[]){

	//Expects a name of an file
	if( argc!=5){
		perror("fd N M D");
		exit(EXIT_FAILURE);
	}


	//TODO should file opening be pushed into the
	//graph construction?
	int fd;
	fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
	N = atoi(argv[2]);
	M = atoi(argv[3]);
	D = atoi(argv[4]);

	if (fd == -1){
		perror("error on opening file");
		exit(EXIT_FAILURE);
	}

	//Open the map
	map = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if(map == MAP_FAILED){
		close(fd);
		perror("Error mapping file");
		exit(EXIT_FAILURE);
	}


	//create test graph
	graph* G = create_graph(map,N,M,D);	
	int test_case_count = 0;

	//=======================Test Cases========================


	printf("Beginging test case %n", test_case_count); 

	//test add_edge

	if (msync(map, length, MS_SYNC) == -1){
		perror("couldn't sync to disk");
	}

	if (munmap(map, length) == -1){
		close(fd);
		perror("Error unmappin");
		exit(EXIT_FAILURE);
	}

	close(fd);

	exit(EXIT_SUCCESS);	
}
