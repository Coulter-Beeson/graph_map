/**
* This class runs a Breadth first Traversal from a given starting node.
* It uses the Graph API to get a list of neighbors given a node.
* Vertex struct is implemented to provide a linked-list datastructure to store the node in a queue.
* Queue operations include creating/deleting queue, adding/removing nodes in FIFO, printing nodes in queue
* 
*/

#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "traversals.h"


/*
int main(int argc, char* argv[])
{
	printf("starting BFS");

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
  //printf("bft:: Print graph \n");

	struct queue* q = queue_new();

	print_graph(G);

	int start_node= strtoul(argv[2],NULL,10); 

	printf("calling BFS\n");

	bfs(G,start_node);

	return 0;
}*/

////////////// BFT Stuff //////////////////////////

void bfs(struct graph* G, int u){
	//TODO: check if node is valid node..ie  it actually exists
	
	printf("Traversing from node %d \n", u);
	struct queue* q = queue_new();
	queue_add_element(q, u); 

	int* nbrs;
	int  curr_node,degree;
	int  count = 0; //for testing, to see how manny nodes were visited
	bool* visited = malloc(sizeof(bool)*(G->N+1));
	memset(visited, false, sizeof visited);
	
	printf("BEGIN TRAVERSAL : \n");

	while(!queue_is_empty(q)){
		curr_node = pop(q);
		if(visited[curr_node]){continue;}		
		printf("curr_node %d\n",curr_node);
		nbrs = get_nbrs(G,curr_node);
		degree=get_deg(G,curr_node);
		for(int i=0; i<degree; i++){
			//printf("adding nbr %d\n", nbrs[i]);
			queue_add_element(q,nbrs[i]);
		}
		visited[curr_node]=true;
		count++;
	}

	printf("---- END TRAVERSAL ---- \n");
	printf("Total nodes visited = %d\n", count);

	//Clean Up
	queue_free(q); 
	free(q);  
}

bool queue_is_empty(struct queue* q){
	return (q->head == NULL);
}

bool queue_add_element(struct queue* s, const int i)
{
  struct vertex* p = malloc( 1 * sizeof(*p) );
 
  if( NULL == p )
    {
      fprintf(stderr, "IN %s, %s: malloc() failed\n", __FILE__, "list_add");
      return false; 
    }
 
  p->num = i;
  p->next = NULL;
  
  if( NULL == s )
    {
      printf("Queue not initialized\n");
      free(p);
      return false;
    }
  else if( NULL == s->head && NULL == s->tail )
    {
      /* printf("Empty list, adding p->num: %d\n\n", p->num);  */
      s->head = s->tail = p;
      return true;
    }
  else if( NULL == s->head || NULL == s->tail )
    {
      fprintf(stderr, "There is something seriously wrong with your assignment of head/tail to the list\n");
      free(p);
      return false;
    }
  else
    {
      /* printf("List not empty, adding element to tail\n"); */
      s->tail->next = p;
      s->tail = p;
    }
 
  return true;
}

int pop(struct queue* q){
	int v = q->head->num;
	queue_remove_element(q);
	return v;
}

/* This is a queue and it is FIFO, so we will always remove the first element */
bool queue_remove_element( struct queue* s )
{
  struct vertex* h = NULL;
  struct vertex* p = NULL;
 
  if( NULL == s )
    {
      printf("Queue is null\n");
      return false;
    }
  else if( NULL == s->head && NULL == s->tail )
    {
      printf("Queue is empty\n");
      return false;
    }
  else if( NULL == s->head || NULL == s->tail )
    {
      printf("There is something seriously wrong with your list\n");
      printf("One of the head/tail is empty while other is not \n");
      return false;
    }
 
  h = s->head;
  p = h->next;
  free(h);
  s->head = p;
  if( NULL == s->head )  s->tail = s->head;   /* The element tail was pointing to is free(), so we need an update */
 
  return true;
}

/* -------small helper fucntions ---------- */
struct queue* queue_free( struct queue* s )
{
  while( s->head != NULL)
    {
      queue_remove_element(s);
    }
 
  return s;
}
 
struct queue* queue_new(void)
{
  struct queue* p = malloc( 1 * sizeof(*p));
 
  if( NULL == p )
    {
      fprintf(stderr, "LINE: %d, malloc() failed\n", __LINE__);
    }
 
  p->head = p->tail = NULL;
   
  return p;
}
 
 
void queue_print( const struct queue* ps )
{
  struct vertex* p = NULL;
 
  if( ps !=NULL )
    {
      for( p = ps->head; p; p = p->next )
    {
      queue_print_element(p);
    }
    }
 
  printf("\n");
}
 
 
void queue_print_element(const struct vertex* p )
{
  if( p ) 
    {
      printf("%d ", p->num);
    }
  else
    {
      printf("Can not print NULL struct \n");
    }
}

///////////////////// DFT Stuff /////////////////////////////////////////

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
 
void dft(struct graph* G,int u){

	printf("Traversing from node %lu \n", u);
	struct stack* st = stack_new();
	stack_push(st, u);//add initial node to stack. 
		
	//TODO: check if node is valid node..ie  it actually exists
	int i, degree, curr_node;
	int nbrs;
	
	int count = 0; //for testing, to see how manny nodes were visited
	bool* visited = malloc(sizeof(bool)*(G->N+1));
	memset(visited, false, sizeof visited);
	
	
	printf("BEGIN TRAVERSAL : \n");
	
	while(st->head != NULL){
			// Pop a vertex from stack and print it
			curr_node = stack_pop(st)->num;
			
			
			if(!visited[curr_node]){
				printf(" %d \n", curr_node); 
				visited[curr_node]=true;
				count++;
			}
			
			//Get adjacent vertices and push it to stack if it's not visited
			
			nbrs = get_nbrs(G,curr_node);
			degree = get_deg(G, curr_node);
			for(i=0;i< degree;i=i+1){
				if(!visited[nbrs[i]]){
					stack_push(st, nbrs[i]);
					
				}
			}
	}
		 		
	   
	printf("---- END TRAVERSAL ---- \n");
	printf("Total nodes visited = %d\n", count);

	//cleanup
	stack_free(st);   // always remember to free() the malloc()ed memory 
	free(st); 
}	

///////////////////////// Triangle Counting Stuff ///////////////////////////////////////