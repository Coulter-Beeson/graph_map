#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include "graph.h"


int main(int argc, char* argv[]){

	if(argc!=3){
		perror("Expects a graph and a numer 1-3");
		exit(EXIT_FAILURE);
	}

	int fd = open(argv[1],O_RDWR,(mode_t)0600);
	int mode = atoi(argv[2]);
	struct graph* G = Graph(fd);

	if(mode==0) print_graph(G);
	if(mode==1) print_offset(G);

	close_graph(G);

	exit(EXIT_SUCCESS);
}
