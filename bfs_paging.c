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
};

static int page_size;
static void * fault_handler_thread(void *arg){
	static struct uffd_msg msg;	/* Data read from userfaultfd */
	static int fault_cnt = 0;	/* Number of faults so far handled */
	long uffd;       		/* userfaultfd file descriptor */
	struct graph* G;
	static char *page = NULL;
	struct uffdio_copy uffdio_copy;
	ssize_t nread;

	struct handler_arg* hargs = (struct handler_arg*) arg;

	G = hargs->g;
	uffd = hargs->uffd;

	printf("uffd number: %d", uffd);
	print_graph(G);

	/* Create a page that will be copied into the faulting region */

	if (page == NULL) {
		page = mmap(NULL, page_size, PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (page == MAP_FAILED)
			errExit("mmap");
	}

	/* Loop, handling incoming events on the userfaultfd
	file descriptor */

	for (;;) {

		/* See what poll() tells us about the userfaultfd */

		struct pollfd pollfd;
		int nready;
		pollfd.fd = uffd;
		pollfd.events = POLLIN;
		nready = poll(&pollfd, 1, -1);
		if (nready == -1)
			errExit("poll");

		printf("\nfault_handler_thread():\n");
		printf("    poll() returns: nready = %d; "
			"POLLIN = %d; POLLERR = %d\n", nready,
		(pollfd.revents & POLLIN) != 0,
		(pollfd.revents & POLLERR) != 0);

		/* Read an event from the userfaultfd */

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
}

int main(int argc, char *argv[]){
	long uffd;          /* userfaultfd file descriptor */
	char *addr;         /* Start of region handled by userfaultfd */
	unsigned long len;  /* Length of region handled by userfaultfd */
	pthread_t thr;      /* ID of thread that handles page faults */
	struct uffdio_api uffdio_api;
	struct uffdio_register uffdio_register;
	int s;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s my_graph\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	page_size = sysconf(_SC_PAGE_SIZE);
	printf("Page size is %d\n", page_size);


	printf("creating graph from %s \n",argv[1]);
	struct graph* G = create_graphf(argv[1]);

	print_graph(G);
	len = get_len(G);
	printf("length of graph file is %d \n",len);

	/* Create and enable userfaultfd object */

	uffd = syscall(__NR_userfaultfd, O_CLOEXEC | O_NONBLOCK);
	if (uffd == -1)
		errExit("userfaultfd");

	uffdio_api.api = UFFD_API;
	uffdio_api.features = 0;
	if (ioctl(uffd, UFFDIO_API, &uffdio_api) == -1)
		errExit("ioctl-UFFDIO_API");

	/* Create a private anonymous mapping. The memory will be
	demand-zero paged--that is, not yet allocated. When we
	actually touch the memory, it will be allocated via
	the userfaultfd. */

	//addr = 	G->map;
	printf("Address returned by mmap() = %p\n", G->map);

	/* Register the memory range of the mapping we just created for
	handling by the userfaultfd object. In mode, we request to track
	missing pages (i.e., pages that have not yet been faulted in). */

	printf("the length of the graph is %d\n", len);

	addr = mmap(NULL, len, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	printf("copying from G->map to addr\n");
	memcpy(addr,G->map,len);

	G->map = (unsigned long*) addr;

	printf("address of addr = %p\n",addr);

	uffdio_register.range.start = (unsigned long) addr;
	uffdio_register.range.len = len;
	uffdio_register.mode = UFFDIO_REGISTER_MODE_MISSING;
	if (ioctl(uffd, UFFDIO_REGISTER, &uffdio_register) == -1)
		errExit("ioctl-UFFDIO_REGISTER");

	/* Create a thread that will process the userfaultfd events */

	s = pthread_create(&thr, NULL, fault_handler_thread, (void *) uffd);
	if (s != 0) {
		errno = s;
		errExit("pthread_create");
	}

	//Application code goes here
	bfs(G,1);

	exit(EXIT_SUCCESS);
}
