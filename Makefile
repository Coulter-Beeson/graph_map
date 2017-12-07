
bfsp: graph.c bft.c bfs_paging.c
	gcc -lm -pthread -std=c99 graph.c bft.c bfs_paging.c -o bfsp

bft: graph.c bft.c
	gcc -lm -std=c99 graph.c bft.c -o bft

print: graph.c print_graph.c
	gcc -lm -std=c99 graph.c print_graph.c -o print

rand: graph.c random_part.c
	gcc -lm -std=c99 graph.c random_part.c -o rand

e2g: graph.c edge_to_graph.c
	gcc -lm -std=c99 graph.c edge_to_graph.c -o e2g

dft: graph.c dft.c
	gcc -lm -std=c99 graph.c dft.c -o dft
