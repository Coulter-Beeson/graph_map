#ifndef GRAPH_H_   
#define GRAPH_H_

#include <stdbool.h>

struct graph{
	unsigned long* g; //The underlying mmap'd file
	unsigned long N; // The number of nodes
	unsigned long M; // The number of edges
	unsigned long D; // Upper bound on maximum degree
	unsigned long off; // number of pages in header
};

struct graph* Graph(int fd);

bool get_edge(struct graph* g, unsigned long u, unsigned long v);
void add_edge(struct graph* g, unsigned long u, unsigned long v);
unsigned long* get_nbrs(struct graph*, unsigned long u);
unsigned long get_off(struct graph* g, unsigned long u);
unsigned long get_deg(struct graph* g, unsigned long u);
void inc_deg(struct graph* g, unsigned long u);
void inc_edge_count(struct graph* g, unsigned long u, unsigned long v);

void print_edge_list(struct graph* g, unsigned long u);
void print_node(struct graph* g, unsigned long u);
void print_graph(struct graph* g);
void close_graph(struct graph* g);
void swap_nodes(struct graph* g, unsigned long u, unsigned long v);

#endif // GRAPH_H_
