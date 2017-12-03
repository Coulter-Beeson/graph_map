/**
* Shuffle the order of the edgelist part in partition file
* 
*/

#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define N 4039

int main(int argc, char* argv[]){
		
	if(argc != 2){
		perror("takes a file pointer to a graph");
		exit(EXIT_FAILURE);
	}

	printf("Opening file\n");

	int fd;
	fd = open(argv[1], O_RDWR, (mode_t)0600);

	if (fd == -1){
		perror("error on opening file");
		exit(EXIT_FAILURE);
	}

	//TODO hardcode test graph

	printf("creating graph\n");
	struct graph* g = Graph(fd);
		
	int i,v;
	for(i=0;i< N;i=i+1)
	{
		v= rand() % N;
		swap_edges(g, i, v);
	}
	return 0;
}
