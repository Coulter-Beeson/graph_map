
bfs: graph.c bft.c
	gcc -lm -std=c99 graph.c bft.c -o bfs

print: graph.c print_graph.c
	gcc -lm -std=c99 graph.c print_graph.c -o print

rand: graph.c random_part.c
	gcc -lm -std=c99 graph.c random_part.c -o rand

e2g: graph.c edge_to_graph.c
	gcc -lm -std=gnu99 graph.c edge_to_graph.c -o e2g
