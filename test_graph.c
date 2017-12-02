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
	printf("Closing graph\n");
	int length = g->off + g->N*g->D;

	if (msync(g->g, length, MS_SYNC) == -1){
		perror("couldn't sync to disk");
	}

	if (munmap(g->g, length) == -1){
		perror("Error unmappin");
		exit(EXIT_FAILURE);
	}
	
	printf("Graph closed.\n");
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
	printf("in inc_edge_count\n");
	g->M++;
	printf("m inced\n");
	g->g[1]++;
	printf("finished incrementing edge count\n");
}

//Returns an adjacency list for the given node u
int* get_nbrs(struct graph* g, int u){
	printf("in get_nbrs\n");
	int o = get_off(g,u);
	printf("out of get_off\n");
	return &g->g[ g->off + g->D*o ];
}

//Adds the edge u,v to g
void add_edge(struct graph* g, int u, int v){
	printf("Adding edge %d %d in add_edge\n", u,v);
	//This makes it safe, but slow
	//if(get_edge(g,u,v)) return;

	int* edges = get_nbrs(g,u);
	printf("get_nbrs\n");
	edges[get_deg(g,u)] = v;
	printf("get_deg\n");
	inc_deg(g,u);
	printf("inc_deg\n");
	inc_edge_count(g,u,v);
	printf("edges added\n");
}

int get_off(struct graph* g, int u){
	printf("offset is being calculated\n");
	return g->g[3 + 2*u];
}

int get_deg(struct graph* g, int u){
	printf("in get_deg\n");
	return g->g[3 + 2*u+1];
}

void inc_deg(struct graph* g, int u){
	printf("in inc_deg\n");
	g->g[3 + 2*u+1]++;
	printf("finished increasing deg\n");
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