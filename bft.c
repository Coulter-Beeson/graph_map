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

#define N 4039 //num of nodes

struct vertex{
	unsigned long num;
	struct vertex* next;
};

struct queue
{
  struct vertex* head;
  struct vertex* tail;
};


bool queue_add_element( struct queue*, const unsigned long);
bool queue_remove_element( struct queue*);
struct queue* queue_new(void);
struct queue* queue_free( struct queue* );
void queue_print( const struct queue* );
void queue_print_element(const struct vertex* );


 
int main(int argc, char* argv[])
{
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
    struct queue* q = queue_new();
	
	unsigned long curr_node= strtoul(argv[2],NULL,10); 
	printf("traversing from node %lu \n", curr_node);
	//TODO: check if node is valid node..ie  it actually exists
	queue_add_element(q, curr_node);//add node to q. 
	int i, degree;
	unsigned long* nbrs;
	int count = 0; //for testing, to see how manny nodes were visited
	bool visited[N]= {false};
	
	printf("BEGIN TRAVERSAL : \n");
	   //get first element of queue. get its neighbors n add to queue if not already visited. add that node to visited
	   while(q->head != NULL)
	   {
			curr_node  = q->head->num;
			if(!visited[(int)curr_node-1]){
				queue_print_element(q->head);
				nbrs = get_nbrs(G, (unsigned long)curr_node);
				printf("here2");
				if(nbrs != NULL){
					degree = (int)get_deg(G, curr_node);
					printf("The neighbors of %d are ", curr_node);
					for(i=0;i< degree;i=i+1)
					{
						queue_add_element(q, (int)nbrs[i]);
						printf(" %d",(int)nbrs[i]);
						//TODO: watch out for array index out of bound
					}
				}
				visited[curr_node-1]=true;
				count++;
				printf("\n");
			}
			queue_remove_element(q); //removes the head
	   }
	printf("---- END TRAVERSAL ---- \n");
	printf("Total nodes visited = %d\n", count);
	printf(" Size of int  %d", sizeof(int));
	//queue_print(visited);
 	queue_free(q);   // always remember to free() the malloc()ed memory 
	free(q);        //free() if list is kept separate from free()ing the structure, I think its a good design 
	q = NULL;      // after free() always set that pointer to NULL, prevents dangling pointer
	return 0;
}

bool queue_add_element(struct queue* s, const unsigned long i)
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
 
  printf("------------------\n");
}
 
 
void queue_print_element(const struct vertex* p )
{
  if( p ) 
    {
      printf("%d\n", p->num);
    }
  else
    {
      printf("Can not print NULL struct \n");
    }
}

