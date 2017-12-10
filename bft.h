#ifndef BFT_H_
#define BFT_H_

struct vertex{
	int num;
	struct vertex* next;
};

struct queue
{
  struct vertex* head;
  struct vertex* tail;
};

void bfs(struct graph*,int);

bool is_empty(struct queue*);
bool queue_add_element( struct queue*, const int);
int pop(struct queue*);
bool queue_remove_element( struct queue*);
struct queue* queue_new(void);
struct queue* queue_free( struct queue* );
void queue_print( const struct queue* );
void queue_print_element(const struct vertex* );

#endif
