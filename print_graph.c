#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>


int main(int argc, char *argv[]){

	if(argc != 3){
		perror("Expects two arguments N and D");
	}

	int N = atoi(argv[1]);
	int D = atoi(argv[2]);

	//printf("%d",N);
	//printf("%d",D);

	int o,d;

	//Print Offset Degree pairs
	for(int i=0; i<N ; i++){
		printf("%d",i+1);
		printf("(");
		o = getchar();
		printf("%d", o+1);
		printf(",");
		d = getchar();
		printf("%d",d);
		printf(") ");
	}

	printf("\n");

	int v;

	//Print the Adjacency lists
	for(int i=0; i<N*D ; i++){
		if( i%D==0 ){
			printf("%d [", i/D+1);
		}

		v = getchar();
		if(v == 0){
			 printf(" ");
		}
		else{
			 printf("%d", v);
		}

		if( i%D==D-1 ){
			printf("]");
			printf("\n");
			continue;
		}

		printf(",");

	}

	exit(EXIT_SUCCESS);

}
