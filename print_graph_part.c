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

	//print_map(G);
	
	
	printf("Before a swap, the node at offset 0 is %d and the node from offset 1 is %d\n", 			get_node_from_off_1(G, 0), get_node_from_off_1(G,1));
		
	swap_nodes(G, 1, 2);
	
	printf("\nAfter a swap, the node at offset 0 is %d and the node from offset 1 is %d\n", 		get_node_from_off_2(G, 0), get_node_from_off_2(G,1));
	
	printf("1: "); print_node(G,1);
	printf("2: "); print_node(G,2); printf("\n");

	//print_map(G);
	print_graph(G);
	
	close_graph(G);

	exit(EXIT_SUCCESS);
}
