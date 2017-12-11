/* userfaultfd_demo.c

Licensed under the GNU General Public License version 2 or later.
*/
#define _GNU_SOURCE
#include <sys/types.h>
#include <stdio.h>
#include <linux/userfaultfd.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <poll.h>

#include "graph.h"
#include "traversals.h"

typedef unsigned long ul;

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
} while (0)

struct handler_arg{
	struct graph* g;
	long uffd;
	void* start;
	pthread_mutex_t* lock;
};

static int page_size;

int needQuit(pthread_mutex_t *mtx){
  switch(pthread_mutex_trylock(mtx)) {
    case 0: /* if we got the lock, unlock and return 1 (true) */
      pthread_mutex_unlock(mtx);
      return 1;
    case EBUSY: /* return 0 (false) if the mutex was locked */
      return 0;
  }
  return 1;
}

static void * fault_handler_thread(void *arg){
	static struct uffd_msg msg;	/* Data read from userfaultfd */
	static int fault_cnt = 0;	/* Number of faults so far handled */
	long uffd;       		/* userfaultfd file descriptor */
	struct graph* G;
	static char *page = NULL;
	struct uffdio_copy uffdio_copy;
	ssize_t nread;

	struct handler_arg* hargs = (struct handler_arg*) arg;
	void* start_addr;


	//force output
	setvbuf(stdout, NULL, _IOLBF, 0);

	G = hargs->g;
	uffd = hargs->uffd;
	start_addr = hargs->start;

	/*
	printf("uffd number: %d\n", uffd);
	print_graph(G);
	printf("start address of region %p\n", start_addr);
	*/

	/* Create a page that will be copied into the faulting region */

	if (page == NULL) {
		page = mmap(NULL, page_size, PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (page == MAP_FAILED)
			errExit("mmap");
	}

	/* Loop, handling incoming events on the userfaultfd
	file descriptor */

	//Trippy lock kill switch lets main signal a kill
	pthread_mutex_t *mx = hargs->lock;
	while( !needQuit(mx) ) {

		/* See what poll() tells us about the userfaultfd */
		struct pollfd pollfd;
		int nready;
		pollfd.fd = uffd;
		pollfd.events = POLLIN;
		nready = poll(&pollfd, 1, 10);
		if (nready == -1)
			errExit("poll");
		if(nready == 0){
			continue;
		}

		nread = read(uffd, &msg, sizeof(msg));
		if (nread == 0) {
			printf("EOF on userfaultfd!\n");
			exit(EXIT_FAILURE);
		}

		if (nread == -1)
			errExit("read");

		/* We expect only one kind of event; verify that assumption */

		if (msg.event != UFFD_EVENT_PAGEFAULT) {
			fprintf(stderr, "Unexpected event on userfaultfd\n");
			exit(EXIT_FAILURE);
		}

		/* Display info about the page-fault event */

		printf("    UFFD_EVENT_PAGEFAULT event: ");
		printf("flags = %llx; ", msg.arg.pagefault.flags);
		printf("address = %llx\n", msg.arg.pagefault.address);

		/* Copy the page pointed to by 'page' into the faulting
		region. Vary the contents that are copied in, so that it
		is more obvious that each fault is handled separately. */

		//TODO this is where we build the page
		//Does this cause fucky recursive faulting?
		//if we fault in here is it just regular fault handling?
		//Even if thats the case we batch all of our faults at once 
		ul b_addr = msg.arg.pagefault.address;
		ul offset = b_addr-( (ul) start_addr)-(sizeof(int)*G->off);
	
		offset = (offset/sizeof(int));
		offset = offset / G->D;

		//printf("offset is %lu\n", offset);

		int faulting_node = get_node_from_off(G,(int) offset);

		if(faulting_node == 0) {
			perror("fault_node is zero");
			exit(EXIT_FAILURE);
		}

		//printf("faulting_node: %d\n", faulting_node);

		int* nbrs = get_nbrs(G,faulting_node);

		int page_num = ((b_addr - ((ul) start_addr))/page_size) ;

		//printf("page_num %d\n", page_num);

		memcpy(page, ((void *)  G->map)+page_num*page_size, page_size);

		fault_cnt++;

		//printf("setting up the copy\n");
		uffdio_copy.src = (ul) page;

		/* We need to handle page faults in units of pages(!).
		So, round faulting address down to page boundary */

		uffdio_copy.dst = (ul) msg.arg.pagefault.address &
					~(page_size - 1);
		uffdio_copy.len = page_size;
		uffdio_copy.mode = 0;
		uffdio_copy.copy = 0;
		//printf("calling ioctl\n");
		if (ioctl(uffd, UFFDIO_COPY, &uffdio_copy) == -1)
			errExit("ioctl-UFFDIO_COPY");

		//printf("        (uffdio_copy.copy returned %lld)\n",
		//	uffdio_copy.copy);
	}

	printf("Faulted a total of %d times \n", fault_cnt);

	if(munmap(page,page_size) == -1){
		perror("error unmapping page");
		exit(EXIT_FAILURE);
	}

	return NULL;	
}

int main(int argc, char *argv[]){
	long uffd;          /* userfaultfd file descriptor */
	char *addr;         /* Start of region handled by userfaultfd */
	ul len;  /* Length of region handled by userfaultfd */
	pthread_t thr;      /* ID of thread that handles page faults */
	struct uffdio_api uffdio_api;
	struct uffdio_register uffdio_register;
	int s;

	//force print everything
	setvbuf(stdout, NULL, _IOLBF, 0);

	if (argc != 2) {
		fprintf(stderr, "Usage: %s my_graph\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	page_size = sysconf(_SC_PAGE_SIZE);
	
	/* Create and enable userfaultfd object */
	uffd = syscall(__NR_userfaultfd, O_CLOEXEC | O_NONBLOCK);
	if (uffd == -1) errExit("userfaultfd");

	uffdio_api.api = UFFD_API;
	uffdio_api.features = 0;
	if (ioctl(uffd, UFFDIO_API, &uffdio_api) == -1)
		errExit("ioctl-UFFDIO_API");

	/* Create a private anonymous mapping. The memory will be
	demand-zero paged--that is, not yet allocated. When we
	actually touch the memory, it will be allocated via
	the userfaultfd. */

	/* Register the memory range of the mapping we just created for
	handling by the userfaultfd object. In mode, we request to track
	missing pages (i.e., pages that have not yet been faulted in). */

	int fd = open(argv[1],O_RDWR, (mode_t)0600);

	struct graph* app_G = Graph(fd);
	len = get_len(app_G);

	ul size_of_offset = sizeof(int)*app_G->off;

	printf("the length of the graph is %d\n", len);

	addr = mmap(NULL, len, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	/*
	Up to Linux kernel 4.11, only private anonymous ranges are compatible
       for registering with UFFDIO_REGISTER.`
	*/
	//copy the header over

	struct graph* handler_G = Graph(fd); 
	memcpy(addr,handler_G->map,size_of_offset);
	app_G->map = (int*) addr;

	//Register Adjacency List Region (empty region of anon file)
	uffdio_register.range.start = (ul) addr + size_of_offset;
	uffdio_register.range.len = len - size_of_offset;
	uffdio_register.mode = UFFDIO_REGISTER_MODE_MISSING;
	if (ioctl(uffd, UFFDIO_REGISTER, &uffdio_register) == -1)
		errExit("ioctl-UFFDIO_REGISTER");

	/* Create a thread that will process the userfaultfd events */

	struct handler_arg* hargs = malloc(sizeof(struct handler_arg));

	hargs->g = handler_G;
	hargs->uffd = uffd;
	hargs->start = (int*) addr;
	pthread_mutex_t mxq;
	pthread_mutex_init(&mxq,NULL);
	pthread_mutex_lock(&mxq);	
	hargs->lock = &mxq;

	s = pthread_create(&thr, NULL, fault_handler_thread, (void *) hargs);
	if (s != 0) {
		errno = s;
		errExit("pthread_create");
	}


	/*
	printf("invalidating addr\n");
	msync(addr,len,MS_SYNC|MS_INVALIDATE);
	msync(G->map,len,MS_SYNC|MS_INVALIDATE);
	*/

	//Application code goes here
	bfs(app_G,1);

	pthread_mutex_unlock(&mxq);
	pthread_join(thr,NULL);
	

	printf("size of page %d\n",page_size);

	printf("size of int %d\n", sizeof(int));
	
	printf("size of D %d\n", app_G->D);

	printf("sie of line: %d\n", sizeof(int)*app_G->D);

	printf("how many pages per line %d\n", 1+(sizeof(int)*app_G->D)/page_size );


	//TODO probably add some proper clean up so the proper regions get resynced with disk
	close_graph(app_G);	

	if(munmap(addr,len) == -1){
		perror("error unmapping addr");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
