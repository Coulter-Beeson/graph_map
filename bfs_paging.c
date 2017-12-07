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
#include "bft.h"


#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
} while (0)

struct handler_arg{
	struct graph* g;
	long uffd;
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

	//force output
	setvbuf(stdout, NULL, _IOLBF, 0);

	G = hargs->g;
	uffd = hargs->uffd;

	printf("uffd number: %d\n", uffd);
	//print_graph(G);

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
		unsigned long b_addr = msg.arg.pagefault.address;
		unsigned long offset = b_addr-( (unsigned long) G->map)-(sizeof(unsigned long)*G->off);
		offset = (offset/sizeof(unsigned long)) % G->D;

		unsigned long faulting_node = get_node_from_off(G,offset);

		printf("faulting_node: %d". faulting_node);
		//TODO This ittself shoud fault
		unsigned long* nbrs = get_nbrs(G,faulting_node);
		

		memset(page, 'A' + fault_cnt % 20, page_size);


		fault_cnt++;

		uffdio_copy.src = (unsigned long) page;

		/* We need to handle page faults in units of pages(!).
		So, round faulting address down to page boundary */

		uffdio_copy.dst = (unsigned long) msg.arg.pagefault.address &
					~(page_size - 1);
		uffdio_copy.len = page_size;
		uffdio_copy.mode = 0;
		uffdio_copy.copy = 0;
		if (ioctl(uffd, UFFDIO_COPY, &uffdio_copy) == -1)
			errExit("ioctl-UFFDIO_COPY");

		printf("        (uffdio_copy.copy returned %lld)\n",
			uffdio_copy.copy);
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
	unsigned long len;  /* Length of region handled by userfaultfd */
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
	
	struct graph* G = create_graphf(argv[1]);
	len = get_len(G);

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

	printf("the length of the graph is %d\n", len);

	addr = mmap(NULL, len, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	
	//fucky copying over to register region
	memcpy(addr,G->map,len);
	G->map = (unsigned long*) addr;

	uffdio_register.range.start = (unsigned long) addr+sizeof(unsigned long)*G->off;
	uffdio_register.range.len = len - sizeof(unsigned long)*G->off;
	uffdio_register.mode = UFFDIO_REGISTER_MODE_MISSING;
	if (ioctl(uffd, UFFDIO_REGISTER, &uffdio_register) == -1)
		errExit("ioctl-UFFDIO_REGISTER");

	/* Create a thread that will process the userfaultfd events */

	struct handler_arg* hargs = malloc(sizeof(struct handler_arg));

	hargs->g = G;
	hargs->uffd = uffd;
	pthread_mutex_t mxq;
	pthread_mutex_init(&mxq,NULL);
	pthread_mutex_lock(&mxq);	
	hargs->lock = &mxq;

	s = pthread_create(&thr, NULL, fault_handler_thread, (void *) hargs);
	if (s != 0) {
		errno = s;
		errExit("pthread_create");
	}

	//Application code goes here
	bfs(G,1);

	pthread_mutex_unlock(&mxq);
	pthread_join(thr,NULL);
	
	close_graph(G);	

	if(munmap(addr,len) == -1){
		perror("error unmapping addr");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
