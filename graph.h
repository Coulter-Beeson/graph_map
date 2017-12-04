#ifndef GRAPH_H_   
#define GRAPH_H_

#include <stdbool.h>

struct graph{
	unsigned int* g; //The underlying mmap'd file
	int N; // The number of nodes
	int M; // The number of edges
	int D; // Upper bound on maximum degree
	int off; // number of pages in header
};

struct graph* Graph(int fd);

bool get_edge(struct graph* g, int u, int v);
void add_edge(struct graph* g, int u, int v);
unsigned int* get_nbrs(struct graph*, int u);
int get_off(struct graph* g, int u);
int get_deg(struct graph* g, int u);
void inc_deg(struct graph* g, int u);
void inc_edge_count(struct graph* g, int u, int v);

void print_edge_list(struct graph* g, int u);
void print_node(struct graph* g, int u);
void print_graph(struct graph* g);
void close_graph(struct graph* g);
void swap_nodes(struct graph* g, int u, int v);

#endif // GRAPH_H_
