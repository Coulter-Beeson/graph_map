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


//TODO ASSUMES that the file is already a created graph
//Should add capabilities to use Graph(int N, int D)
//that creates its own empty file and turns it into an empty graph
//of the correct size`
struct graph* Graph(int fd){

	//printf("creating graph\n");

	struct stat st;
	fstat(fd, &st);

	//TODO add error checking to see if the file is an existing
	unsigned long length = st.st_size;

	//printf("the length of the file is %d \n",length);
	//Open the map

	if(length == 0){
		perror("empty file, requires an existing graph");
		exit(EXIT_FAILURE);
	}

	printf("length of file being mapped: %d\n",length);

	//printf("mapping file\n");
	unsigned long* map = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if(map == MAP_FAILED){
		close(fd);
		perror("Error mapping file");
		exit(EXIT_FAILURE);
	}

	//printf("allocating graph object\n");
	struct graph *g = malloc(sizeof(struct graph));

	//printf("assigning member variables\n");
	g->map = map;
	g->N = map[0];	//Number of nodes
	g->M = map[1];	//Number of edges
	g->D = map[2];	//Upper bound on max degree
	

	//printf("N = %d M = %d D = %d\n", g->N, g->M, g->D);

	unsigned long page_size = sysconf(_SC_PAGESIZE);
	
	//printf("page size is : %d now calculate offset\n",page_size);

	int num_pages = ceil((sizeof(unsigned long)*(3+3*g->N)/(double)page_size));

	//printf("the number of pages for the header is %d\n",num_pages);

	//The off set from which the adjacency lists start
	g->off = (num_pages*page_size)/sizeof(unsigned long);
	//printf("The offset is: %d\n", g->off);

	//printf("returning graph\n");
	return g;
}

//Takes the name of an existing graph file
struct graph* create_graphf(char* file){
	int fd = open(file, O_RDWR, (mode_t)0600);
	return Graph(fd);
}

//Closes the graph file and syncs the memory
//This is required otherwise you're file will not be correct!
void close_graph(struct graph* g){
	//printf("Closing graph\n");
	unsigned long length = g->off + g->N*g->D;

	if (msync(g->map, length, MS_SYNC) == -1){
		perror("couldn't sync to disk");
	}

	if (munmap(g->map, length) == -1){
		perror("Error unmappin");
		exit(EXIT_FAILURE);
	}
	
	//printf("Graph closed.\n");
}

//Linearly scans the adjacency list looking for the entry
bool get_edge(struct graph* g, unsigned long u, unsigned long v){
	unsigned long d = get_deg(g,u);
	unsigned long o = get_off(g,u);

	unsigned long* edges = get_nbrs(g,u);
	
	for(unsigned long i=0; i<d; i++){
		if(edges[i] == v){
			return true;
		}
	}

	return false;
}

//Icreases the edge count in both the object and the file
void inc_edge_count(struct graph* g, unsigned long u, unsigned long v){
	g->M++;
	g->map[1]++;
}

//Returns an adjacency list for the given node u
unsigned long* get_nbrs(struct graph* g, unsigned long u){

	unsigned long o = get_off(g,u);

	return &g->map[ g->off + g->D*o ];
}

//Adds the edge u,v to g
void add_edge(struct graph* g, unsigned long u, unsigned long v){
	//printf("Adding edge %d %d in add_edge\n", u,v);
	//This makes it safe, but slow
	//if(get_edge(g,u,v)) return;

	unsigned long* edges_u = get_nbrs(g,u);
	unsigned long* edges_v = get_nbrs(g,v);

	edges_u[get_deg(g,u)] = v;
	edges_v[get_deg(g,v)] = u;

	inc_deg(g,u);
	inc_deg(g,v);

	inc_edge_count(g,u,v);

}

//Returns the size of the underlying file in bytes
unsigned long get_len(struct graph* g){
	unsigned long page_size = sysconf(_SC_PAGESIZE);
	unsigned long size_header = g->off*(sizeof(unsigned long));
	unsigned long num_pages_mat = (g->N*g->D*(sizeof(unsigned long))/page_size)+1  ;

	return size_header+page_size*num_pages_mat;
}

unsigned long get_off(struct graph* g, unsigned long u){
	//printf("offset is being calculated\n");
	return g->map[3 + 2*(u-1)];
}

void set_off(struct graph* g, unsigned long u, unsigned long off){
	g->map[3+2*(u-1)]=off;
	g->map[3+2*g->N+off]=u;
}

unsigned long get_node_from_off(struct graph* g,unsigned long off){
	return g->map[3+2*g->N+off]+1;
} 
/*
unsigned long get_node_from_off(struct graph* g,unsigned long off){
	return g->map[3+2*g->N+off];
} */

void set_node_at_off(struct graph* g, unsigned long u, unsigned long off){
	g->map[3+2*g->N+off]=u;
	g->map[3+2*(u-1)]=off;
}

unsigned long get_deg(struct graph* g, unsigned long u){
	//printf("in get_deg\n");
	return g->map[3 + 2*(u-1)+1];
}

void inc_deg(struct graph* g, unsigned long u){
	g->map[3 + 2*(u-1)+1] += 1;
}

//prints a single node's offset and degree
void print_node(struct graph* g, unsigned long u){
	printf("(%d,%d)",get_off(g,u),get_deg(g,u));
}

//prints a nodes adjacency list
void print_nbrs(struct graph* g, unsigned long u){
	unsigned long d = get_deg(g,u);
	unsigned long* el = get_nbrs(g,u);

	printf("%d: [", u);

	for(unsigned long i=0; i<d; i++){
		printf("%d",el[i]);
		if(i != d-1) printf(",");
	}
	
	printf("]");
}

void print_graph(struct graph* g){
	//printf("printing graph \n");

	printf("N:%d,M:%d,D:%d\n",g->N,g->M,g->D);
	
	//printf("printing nodes\n");
	for(unsigned long i=1; i<=g->N; i++){
		printf("%d",i);
		print_node(g,i);
	}
	printf("\n");
	
	//printf("print edge lists\n");
	for(unsigned int i=1; i<=g->N; i++){
		print_nbrs(g,i);
		printf("\n");
	}

}
/*
void print_map(struct graph* g){
	//printf("printing graph \n");

	printf("N:%d,M:%d,D:%d\n",g->N,g->M,g->D);
	
	//printf("printing nodes\n");
	for(unsigned long i=0; i<g->N; i++){
		unsigned long node = get_node_from_off(g, i);
		printf("%d",node);
		print_node(g,node);
	}
	printf("\n");
	
	//printf("print edge lists\n");
	for(unsigned int i=0; i<g->N; i++){
		unsigned long node = get_node_from_off(g, i);
		print_nbrs(g,node);
		printf("\n");
	}

}*/


//Swaps the position of the adjacency lists for u and v
//Effectively can be used to change which page a node resides on. 
void swap_nodes(struct graph* g, unsigned long u, unsigned long v){

	int d_u = get_deg(g,u);
	//printf("degree of %d is %d\n", u, d_u);
	int d_v = get_deg(g,v);
	//printf("degree of %d is %d\n", v, d_v);

	unsigned long* nbr_u = get_nbrs(g,u);
	unsigned long* nbr_v = get_nbrs(g,v);

	//write down the shorter of the two
	//TODO abstract the body of this if to its own method
	
	if(d_u <= d_v){
		unsigned long tmp[d_u]; 						//tmp of length degree_u
		memcpy(tmp,nbr_u,d_u*sizeof(unsigned long));	//copy u's neighbours to tmp
		memcpy(nbr_u,nbr_v,d_v*sizeof(unsigned long));	//copy v's neighbours to u
		memcpy(nbr_v,tmp,d_u*sizeof(unsigned long));	//copy u's neighbours from tmp to v
		//print_map(g);
		printf("%d",u); print_node(g, u);
		printf("%d",v); print_node(g, v); printf("\n");
		//print_graph(g);
		unsigned long off_u = get_off(g,u);
		unsigned long off_v = get_off(g,v);
		//printf("\ntmpNode %d is at offset %d\n", get_node_from_off(g, tmp_o), tmp_o);
		//printf("want to set node %d to be at offset %d\n", u, get_off(g,v)) ;
		set_off(g,u,off_v);
		set_off(g,v,off_u);
		printf("after set, node %d is at offset %d and node %d is at offset %d\n", u, get_off(g,u), v, get_off(g,v)) ;
		//int c
		
		print_graph(g);
		
		printf("now node %d is at offset %d and node %d is at offset %d\n", v, get_off(g,v), u, get_off(g,u));
		//print_graph(g);
		
		printf("%d",u); print_node(g, u);
		printf("%d",v); print_node(g, v); printf("\n");
	}
	else{
		unsigned long tmp[d_v];		
		memcpy(tmp,nbr_v,d_v*sizeof(unsigned long));	
		memcpy(nbr_v,nbr_u,d_u*sizeof(unsigned long));
		memcpy(nbr_u,tmp,d_v*sizeof(unsigned long));

		unsigned long tmp_o = get_off(g,v);
		set_off(g,v,get_off(g,u));
		set_off(g,u,tmp_o);
	}

}
/*
int main(int argc, char* argv[]){

	setvbuf (stdout, NULL, _IONBF, 0);

	printf("start\n");

	//Expects a name of an file
	if(argc != 2){
		perror("takes a file pointer to a graph");
		exit(EXIT_FAILURE);
	}

	printf("Opening file\n");

	int fd;
	fd = open(argv[1], O_RDWR, (mode_t)0600);

	if (fd == -1){
		perror("error on opening file");
		exit(EXIT_FAILURE);
	}

	//TODO hardcode test graph

	printf("creating graph\n");
	//create test graph
	struct graph* G = Graph(fd);	
	int test_case_count = 0;

	//=======================Test Cases========================

	printf("Beginning test case %d\n", test_case_count); 

	print_graph(G);

	//test add_edge

	//TODO add a graph clsoe function
	//TODO figure out length 

	close_graph(G);
	close(fd);

	exit(EXIT_SUCCESS);	
}*/
