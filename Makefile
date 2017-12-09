
bfsp: graph.c bft.c bfs_paging.c
	gcc -pthread -std=c99 graph.c bft.c bfs_paging.c -o bfsp -lm

bft: graph.c bft.c
	gcc -std=c99 graph.c bft.c -o bft -lm

print: graph.c print_graph.c
	gcc -std=c99 graph.c print_graph.c -o print -lm

rand: graph.c random_part.c
	gcc -std=c99 graph.c random_part.c -o rand -lm

e2g: graph.c edge_to_graph.c
	gcc -std=c99 graph.c edge_to_graph.c -o e2g -lm

dft: graph.c dft.c
	gcc -std=c99 graph.c dft.c -o dft -lm
	
bfpart: graph.c bft.c bf_part.c
	gcc -std=gnu99 graph.c bft.c bf_part.c -o bfpart -lm
