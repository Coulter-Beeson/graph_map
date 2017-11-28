#ifndef GRAPH_H_   
#define GRAPH_H_

#include <stdbool.h>

struct graph{
	char* g; //The underlying mmap'd file
	int N;
	int M;
	int D;
};

struct graph* Graph(int fd);

bool get_edge(struct graph* g, int u, int v);
void add_edge(struct graph* g, int u, int v); 
int get_off(struct graph* g, int u);
int get_deg(struct graph* g, int u);
void inc_deg(struct graph* g, int u);

#endif // GRAPH_H_
