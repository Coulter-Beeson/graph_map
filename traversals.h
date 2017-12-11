#ifndef TRAVERSALS_H
#define TRAVERSALS_H_

struct vertex{
	int num;
	struct vertex* next;
};

struct queue
{
  struct vertex* head;
  struct vertex* tail;
};


struct stack
{
  struct vertex* head;
};

/* For DFT */
bool stack_push( struct stack*, const unsigned long);
struct vertex* stack_pop( struct stack*);
struct stack* stack_new(void);
struct stack* stack_free( struct stack* );
void stack_print( const struct stack* );
void stack_print_element(const struct vertex* );
void dft(struct graph*,int);

/*For BFT */
bool queue_is_empty(struct queue*);
bool queue_add_element( struct queue*, const int);
int pop(struct queue*);
bool queue_remove_element( struct queue*);
struct queue* queue_new(void);
struct queue* queue_free( struct queue* );
void queue_print( const struct queue* );
void queue_print_element(const struct vertex* );
void bfs(struct graph*,int);
#endif
