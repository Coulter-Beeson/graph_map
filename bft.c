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

#define waiting 1
#define visited 2

struct vertex{
	int num;
	int state;
	struct vertex* next;
};

struct queue
{
  struct vertex* head;
  struct vertex* tail;
};

struct visitedList{
	
}

bool queue_add_element( struct queue*, const int);
bool queue_remove_element( struct queue*);
struct queue* queue_new(void);
struct queue* queue_free( struct queue* );
void queue_print( const struct queue* );
void queue_print_element(const struct vertex* );
 
 
int main(int argc, char* argv[])
{
	if(argc != 2){
		perror("takes a file pointer to a graph");
		exit(EXIT_FAILURE);
	}

	int fd;
	fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);

	if (fd == -1){
		perror("error on opening file");
		exit(EXIT_FAILURE);
	}

	//create test graph
	struct graph* G = Graph(fd);	
    struct queue*  q = NULL;
	int initial_node= 0; //say for now, later take it from cmd line
  
	q = queue_new();
	//setup initial node as vertex object
	
    struct vertex* v = malloc( 1 * sizeof(*v) );
	v->num= initial_node;
	v->state= waiting;
	int* node = get_neighbors(initial_node);
    while( node != NULL && v->state != visited){
		int degree = getdegree(G, node)
		v->state= visited
		for(int i=0;i< degree;i=i+1){
		  //add elements to Q. 
		  queue_add_element(q,&node)
		}
		node=queue(getfirstelement)
		int* = get_neighbors(node)
	}
    
  */
  
  list_add_element(mt, 1);
  list_add_element(mt, 2);
  list_add_element(mt, 3);
  list_add_element(mt, 4); 
   
  list_print(mt);
 
  list_remove_element(mt);
  list_print(mt);
 
  list_free(mt);   /* always remember to free() the malloc()ed memory */
  free(mt);        /* free() if list is kept separate from free()ing the structure, I think its a good design */
  mt = NULL;      /* after free() always set that pointer to NULL, C will run havon on you if you try to use a dangling pointer */
 
  list_print(mt);
 
  return 0;
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
 
/* This is a queue and it is FIFO, so we will always remove the first element */
bool list_remove_element( struct queue* s )
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
struct queue* queue_free( struct my_list* s )
{
  while( s->head )
    {
      list_remove_element(s);
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
 
  if( ps )
    {
      for( p = ps->head; p; p = p->next )
    {
      queue_print_element(p);
    }
    }
 
  printf("------------------\n");
}
 
 
void queue_print_element(const struct vertex* p )
{
  if( p ) 
    {
      printf("Num = %d\n", p->num);
    }
  else
    {
      printf("Can not print NULL struct \n");
    }
}