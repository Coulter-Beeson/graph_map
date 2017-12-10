#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include "graph.h"


int main(int argc, char* argv[]){

	if(argc!=2){
		perror("Expects a graph");
		exit(EXIT_FAILURE);
	}

	int fd = open(argv[1],O_RDWR,(mode_t)0600);

	struct graph* G = Graph(fd);
	
	print_map(G);
	
	int N = G->N;
	printf("Before a swap, the node at offset 1 is %d and the node from offset 2 is %d\n", get_node_from_off(G, 1), get_node_from_off(G,2));
		
	swap_nodes(G, 1, 2);
	
	printf("After a swap, the node at offset 1 is %d and the node from offset 2 is %d\n", get_node_from_off(G, 1), get_node_from_off(G,2));
	
	print_map(G);
	printf("Before a swap, the node at offset 3 is %d and the node from offset 3 is %d\n", get_node_from_off(G, 3), get_node_from_off(G,3));

	swap_nodes(G, 3, 3);
	
	printf("After a swap, the node at offset 3 is %d and the node from offset 3 is %d\n", get_node_from_off(G, 3), get_node_from_off(G,3));
	
	print_map(G);

	close_graph(G);

	exit(EXIT_SUCCESS);
}
