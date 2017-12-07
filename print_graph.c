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

	print_graph(G);

	close_graph(G);

	exit(EXIT_SUCCESS);
}
