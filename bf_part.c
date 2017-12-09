/**
* sets nodes in bft order
* 
*/

#include "graph_part.h"
#include "bft.h"
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
	unsigned long K = atoi(argv[2]);

	printf("Opening file\n");

	int fd;
	fd = open(argv[1], O_RDWR, (mode_t)0600);

	if (fd == -1){
		perror("error on opening file");
		exit(EXIT_FAILURE);
	}

	printf("creating graph\n");
	struct graph* g = Graph(fd);

	//Seed the RNG with the current time
	//srand(time(NULL));
	//TODO Seeded with a constant for testing
	
	unsigned long N = g->N;

	bool isVisited[N];
	
	//all nodes are unvisited
	for (int i = 0; i < N; i++){
		isVisited[i] = false;
	}
	//order of nodes to visit
	struct queue* q = queue_new();
	queue_add_element(q, K);
	
	
	unsigned long* nbrs;
	unsigned long curr_node,degree;
	unsigned long order = 0; //for testing, to see how manny nodes were visited
	
	printf("BEGIN TRAVERSAL : \n");

	while(!is_empty(q)){
		queue_print(q);
		curr_node = pop(q);
			
		if(!isVisited[curr_node]) {
			unsigned long o = get_off(g, curr_node);
			unsigned long n = get_node_from_off(g, order);
			printf("in node %d in offset %d needs to go to %d\n", curr_node, o, order); 
			printf("node %d is currently in offset %d\n", n, order);
			isVisited[curr_node] = true;
			nbrs = get_nbrs(g,curr_node);
			degree=get_deg(g,curr_node);
			for(unsigned long i=0; i<degree; i++){
				queue_add_element(q,nbrs[i]);
			}
			
			swap_nodes(g, curr_node, n);
			order++;
		}
	}

	printf("---- END TRAVERSAL ---- \n");
	printf("Total nodes visited = %d\n", order);

	//Clean Up
	queue_free(q); 
	free(q);  
	
	print_graph(g);
	
	printf("closing graph");
	close_graph(g);
	close(fd);

	return 0;
}
