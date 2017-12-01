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

	struct stat st;
	fstat(fd, &st);

	//TODO add error checking to see if the file is an existing
	int length = st.st_size;
	//Open the map

	if(length == 0){
		perror("empty file, requires an existing graph");
		exit(EXIT_FAILURE);
	}

	char* map = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if(map == MAP_FAILED){
		close(fd);
		perror("Error mapping file");
		exit(EXIT_FAILURE);
	}

	struct graph *g = malloc(sizeof(struct graph));

	g->g = map;
	g->N = map[0];	//Number of nodes
	g->M = map[1];	//Number of edges
	g->D = map[2];	//Upper bound on max degree

	long page_size = sysconf(_SC_PAGESIZE);
	
	//The off set from which the adjacency lists start
	g->off = ((4*(3+2*g->N)/page_size)+1)*page_size;

	return g;
}

//Closes the graph file and syncs the memory
//This is required otherwise you're file will not be correct!
void close_graph(struct graph* g){
	
	int length = g->off + g->N*g->D;

	if (msync(g->g, length, MS_SYNC) == -1){
		perror("couldn't sync to disk");
	}

	if (munmap(g->g, length) == -1){
		perror("Error unmappin");
		exit(EXIT_FAILURE);
	}
}

bool get_edge(struct graph* g, int u, int v){
	int d = get_deg(g,u);
	int o = get_off(g,u);

	int* edges = get_nbrs(g,u);
	
	for(int i=0; i<d; i++){
		if(edges[i] == v){
			return true;
		}
	}

	return false;
}

//INcreases the edge count in both the object and the file
void inc_edge_count(struct graph* g, int u, int v){
	g->M++;
	g->g[1]++;
}

//Returns an adjacency list for the given node u
int* get_nbrs(struct graph* g, int u){
	int o = get_off(g,u);
	return (int*)g->g[ g->off + g->D*o ];
}

//Adds the edge u,v to g
void add_edge(struct graph* g, int u, int v){
	
	//This makes it safe, but slow
	//if(get_edge(g,u,v)) return;

	int* edges = get_nbrs(g,u);

	edges[get_deg(g,u)] = v;
	inc_deg(g,u);
	inc_edge_count(g,u,v);
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

//prints a single node's offset and degree
void print_node(struct graph* g, int u){
	printf("(%d,%d)",get_off(g,u),get_deg(g,u));
}

//prints a nodes adjacency list
void print_edge_list(struct graph* g, int u){
	int d = get_deg(g,u);
	int* el = get_nbrs(g,u);

	printf("%d: [", u);

	for(int i=0; i<d; i++){
		printf("%d",el[i]);
		if(i != d-1) printf(",");
	}
	
	printf("]");
}

void print_graph(struct graph* g){
	for(int i=0; i<g->N; i++){
		print_node(g,i);
	}
	printf("\n");

	for(int i=0; i<g->N; i++){
		print_edge_list(g,i);
		printf("\n");
	}

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

	//TODO hardcode test graph

	//create test graph
	struct graph* G = Graph(fd);	
	int test_case_count = 0;

	//=======================Test Cases========================

	printf("Beginning test case %n", test_case_count); 

	print_graph(G);

	//test add_edge

	//TODO add a graph clsoe function
	//TODO figure out length 

	close_graph(G);
	close(fd);

	exit(EXIT_SUCCESS);	
}
