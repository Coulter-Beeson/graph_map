#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

// List_of_offset_deg_pairs [2N]
// N lists of D vertices

int N,M,D;
char* map;

void add_edge(int u, int v){
	//adds the edges to u and v's lists
	map[ 2*N + D*map[2*u] + map[2*u+1] ] = v+1;
	//increments the degrees
	map[ 2*u+1 ]++;
}

int main(int argc, char *argv[]) {

	//Expects a name of an file
	if( argc!=5){
		perror("fd N M D");
		exit(EXIT_FAILURE);
	}

	int fd;
	fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
	N = atoi(argv[2]);
	M = atoi(argv[3]);
	D = atoi(argv[4]);

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
	
	int c,u,v, count=0;

	for(int i=0; i<N; i++){
		map[2*i]=i;
		map[2*i+1]=0;
	}
	for(int i=0; i<N*D; i++){
		map[2*N + i - 1] = 0;
	}

	for(int i=0; i<M; i++){
		scanf("%d", &u);
		scanf("%d", &v);

		add_edge(u,v);	
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
