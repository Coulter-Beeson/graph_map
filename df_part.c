/**
* sets nodes in dft order
* 
*/
#include "graph.h"
//#include "graph_part.h"
#include "traversals.h"
//#include "bft.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

int main(int argc, char* argv[]){
	
	
	if(argc != 3){
		perror("expects: graph object, beginning node");
		exit(EXIT_FAILURE);
	}

	//makes stuff print right awawy
	setvbuf (stdout, NULL, _IONBF, 0);

	//The node to start on
	int K = atoi(argv[2]);

	//printf("Opening file\n");

	int fd;
	fd = open(argv[1], O_RDWR, (mode_t)0600);

	if (fd == -1){
		perror("error on opening file");
		exit(EXIT_FAILURE);
	}

	//printf("creating graph\n");
	struct graph* g = Graph(fd);

	//Seed the RNG with the current time
	//srand(time(NULL));
	//TODO Seeded with a constant for testing
	
	int N = g->N;

	int isVisited[N+1];
	
	//all nodes are unvisited
	for (int i = 0; i <= N; i++){
		isVisited[i] = 0;
	}
	//order of nodes to visit
	struct stack* st = stack_new();
	stack_push(st, K);
	
	
	int* nbrs;
	int curr_node,degree;
	int order = 0; //for testing, to see how manny nodes were visited
	
	//printf("BEGIN TRAVERSAL : \n");

	while(st->head != NULL){
		//queue_print(q);
		curr_node = stack_pop(st)->num;
			
		if(isVisited[curr_node]==0) {
			int o = get_off(g, curr_node);
			int n = get_node_from_off(g, order);
			//printf("in node %d in offset %d needs to go to %d\n", curr_node, o, order); 
			//printf("node %d is currently in offset %d\n", n, order);
			isVisited[curr_node] = 1;
			nbrs = get_nbrs(g,curr_node);
			degree=get_deg(g,curr_node);
			for(int i=0; i<degree; i++){
				stack_push(st,nbrs[i]);
			}
			
			swap_nodes(g, curr_node, n);
			order++;
		}
	}

	//printf("---- END TRAVERSAL ---- \n");
	//printf("Total nodes visited = %d\n", order);

	//Clean Up
	stack_free(st); 
	free(st);  
	
	//print_graph(g);
	
	//printf("closing graph");
	close_graph(g);
	close(fd);

	return 0;
}
