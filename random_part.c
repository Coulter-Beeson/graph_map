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
#include <time.h>

int main(int argc, char* argv[]){
	
	
	if(argc != 3){
		perror("expects: my_graph number_of_shuffles");
		exit(EXIT_FAILURE);
	}

	//makes stuff print right awawy
	setvbuf (stdout, NULL, _IONBF, 0);

	//The number of times to repeat the shuffle
	int K = atoi(argv[2]);

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
	srand(1);	


	int N = g->N;

	int v;

	printf("shuffling %d times\n",K);

	for(int i=0; i<K; i++){
		printf("shuffle %d of %d\n",i,K);
		for(int j=1; j<=N ;j++){

			v = 1 + rand() % N;
			printf("swapping %d and %d\n",j, v);
			swap_nodes(g, j, v);
		}
	}

	printf("closing graph");
	close_graph(g);

	return 0;
}
