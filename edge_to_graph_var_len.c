#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>


int N=4029, M=88234, D=4096;
char* map;

void add_edge(int u, int v, int offset){
	map[u+1]++;
	map[2*N+offset+map[u+1]] = v;
}


//TODO make circular
struct queue{
	int front;
	int end;
	int* array;
};

void push(struct queue *Q, int u, int v){
	Q->end += 2;
	Q->array[Q->end] = u;
	Q->array[Q->end+1] = v;
}

int pop(struct queue *Q){
	int r = Q->array[Q->front+1];
	Q->front += 2;
}


int head(struct queue *Q){
	return Q->array[0];
}



int main(int argc, char *argv[]) {

	//Expects a name of an file
	if( argc!=2){
		perror("wrong number of arguments");
		exit(EXIT_FAILURE);
	}

	int fd;
	fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);

	if (fd == -1){
		perror("error on opening file");
		exit(EXIT_FAILURE);
	}

	//stretch the file
	size_t length = N*(2+D);
	if (lseek(fd, length+1, SEEK_SET) == -1){
		close(fd);
		perror("Error on lseek call to stretch the file");
		exit(EXIT_FAILURE);
	}

	//Needs to have something written so add an empty string 
	if (write(fd, "", 1) == -1){
		close(fd);
		perror("Error on writing last byte of file");
		exit(EXIT_FAILURE);
	}
 
	//Open the map
	map = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if(map == MAP_FAILED){
		close(fd);
		perror("Error mapping file");
		exit(EXIT_FAILURE);
	}
	
	int c,u,v,count=0;

	struct queue *Q;

	int* arr = malloc(N*sizeof(int));

	Q->array = arr;

	for(int i=0; i<N; i++){
		scanf("%d", &u);
		scanf("%d", &v);

		push(Q,v,u);
	
		if(c != u){

			//add all the back edges
			while(head(Q) == c){
				add_edge(c,pop(Q),count);
				count++;	
			} 
			map[u]=i; // set the offset for u's list
			c=u;

		} 

		add_edge(u,v,count);
		count++;	
	}

	if (msync(map, length, MS_SYNC) == -1){
		perror("couldn't sync to disk");
	}

	if (munmap(map, length) == -1){
		close(fd);
		perror("Error unmappin");
		exit(EXIT_FAILURE);
	}

	close(fd);

	exit(EXIT_SUCCESS);
}
