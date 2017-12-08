#ifndef BFT_H_
#define BFT_H_

struct vertex{
	unsigned long num;
	struct vertex* next;
};

struct queue
{
  struct vertex* head;
  struct vertex* tail;
};

void bfs(struct graph*,unsigned long);

bool is_empty(struct queue*);
bool queue_add_element( struct queue*, const unsigned long);
unsigned long pop(struct queue*);
bool queue_remove_element( struct queue*);
struct queue* queue_new(void);
struct queue* queue_free( struct queue* );
void queue_print( const struct queue* );
void queue_print_element(const struct vertex* );

#endif
