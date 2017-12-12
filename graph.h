#ifndef GRAPH_H_   
#define GRAPH_H_

#include <stdbool.h>

typedef unsigned long ul;

struct graph{
	int* map; //The underlying mmap'd file
	int N; // The number of nodes
	int M; // The number of edges
	int D; // Upper bound on maximum degree
	ul off; // number of pages in header
};

struct graph* Graph(int fd);
struct graph* create_graphf(char* file);

bool get_edge(struct graph* g, int u, int v);
void add_edge(struct graph* g, int u, int v);
int* get_nbrs(struct graph* g, int u);
int get_node_from_off(struct graph*, int off);
int get_off(struct graph* g, int u);
void set_off(struct graph* g, int u, int off);
int get_deg(struct graph* g, int u);
ul get_len(struct graph* g);
void inc_deg(struct graph* g, int u);
void inc_edge_count(struct graph* g, int u, int v);

void print_arr(int* arr, int size);

void print_nbrs(struct graph* g, int u);
void print_node(struct graph* g, int u);
void print_graph(struct graph* g);
void print_offset(struct graph* g);
void print_degrees(struct graph* g);

void close_graph(struct graph* g);
void swap_nodes(struct graph* g, int u, int v);

#endif // GRAPH_H_
