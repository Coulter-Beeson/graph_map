/**
* This class runs a Depth first Traversal from a given starting node.
* It uses the Graph API to get a list of neighbors given a node.
* Vertex struct is implemented to provide a linked-list datastructure to store the node in a stack.
* 
* N is the number of nodes in the graph. We need this to create an array of boolean called 'visited'
*/

#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define N 4039 //max num of nodes

struct vertex{
	unsigned long num;
	struct vertex* next;
};

struct stack
{
  struct vertex* head;
};


bool stack_push( struct stack*, const unsigned long);
struct vertex* stack_pop( struct stack*);
struct stack* stack_new(void);
struct stack* stack_free( struct stack* );
void stack_print( const struct stack* );
void stack_print_element(const struct vertex* );
 
int main(int argc, char* argv[]){
	
	if(argc != 3){
		perror("takes a file pointer and initial node to traverse graph from as args");
		exit(EXIT_FAILURE);
	}

	int fd;
	fd = open(argv[1], O_RDWR , (mode_t)0600);

	if (fd == -1){
		perror("error on opening file");
		exit(EXIT_FAILURE);
	} 

	//create test graph
	struct graph* G = Graph(fd);	 //actually get graph object
	struct stack* st = stack_new();
	
	unsigned long curr_node= strtoul(argv[2],NULL,10); 
	printf("Traversing from node %lu \n", curr_node);
	//TODO: check if node is valid node..ie  it actually exists
	unsigned long i; 
	unsigned long degree;
	unsigned long* nbrs;
	struct vertex* popped;
	
	int count = 0; //for testing, to see how manny nodes were visited
	bool visited[N]= {false};
	stack_push(st, curr_node);//add initial node to stack. 
	
	printf("BEGIN TRAVERSAL : \n");
	
	while(st->head != NULL){
			// Pop a vertex from stack and print it
			popped= stack_pop(st);
			curr_node = popped->num;
			
			
			if(!visited[curr_node-1]){
				printf(" %d \n", curr_node); 
				visited[curr_node-1]=true;
				count++;
			}
			
			//Get adjacent vertices and push it to stack if it's not visited
			
			nbrs = get_nbrs(G,curr_node);
			degree = get_deg(G, curr_node);
			for(i=0;i< degree;i=i+1){
				if(!visited[nbrs[i]-1]){
					stack_push(st, nbrs[i]);
					
				}
			}
	}
		 		
	   
	printf("---- END TRAVERSAL ---- \n");
	printf("Total nodes visited = %d\n", count);

	//queue_print(visited);
 	close_graph(G);
	stack_free(st);   // always remember to free() the malloc()ed memory 
	free(st);        //free() if list is kept separate from free()ing the structure, I think its a good design 
	st = NULL;      // after free() always set that pointer to NULL, prevents dangling pointer
	return 0;
}

bool stack_push(struct stack* s, const unsigned long i)
{
  struct vertex* p = malloc( 1 * sizeof(*p) );
 
  if( NULL == p )
    {
      fprintf(stderr, "IN %s, %s: malloc() failed\n", __FILE__, "list_add");
      return false; 
    }
 

  if( NULL == s )
    {
      printf("Stack not initialized\n");
      free(p);
      return false;
    }
    
  p->num = i;
  p->next = s->head;
  s->head = p;
  return true;
}
 
/* This is a stack and it is LIFO, so we will always remove the first element */
struct vertex* stack_pop(struct stack* s )
{
  struct vertex* h = NULL;
  struct vertex* p = NULL;
 
  if( NULL == s )
    {
      printf("stack is null\n");
      return NULL;
    }
    
    
  else if( NULL == s->head)
    {
      printf("stack is empty\n");
      return NULL;
    }
    
  h = s->head;
  p = h->next;
  //free(h);
  s->head = p;
    
  return h;
}

/* -------small helper fucntions ---------- */
struct stack* stack_free( struct stack* s )
{
  while( s->head != NULL)
    {
      struct vertex* x= stack_pop(s);
      free(x);
    }
 
  return s;
}
 
struct stack* stack_new(void)
{
  struct stack* p = malloc( 1 * sizeof(*p));
 
  if( NULL == p )
    {
      fprintf(stderr, "LINE: %d, malloc() failed\n", __LINE__);
    }
 
  p->head  = NULL;
   
  return p;
}
 
 

