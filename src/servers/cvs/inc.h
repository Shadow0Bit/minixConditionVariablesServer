#define _POSIX_SOURCE      1    /* tell headers to include POSIX stuff */
#define _MINIX             1    /* tell headers to include MINIX stuff */
#define _SYSTEM            1    /* get OK and negative error codes */

#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/config.h>
#include <minix/ipc.h>
#include <minix/endpoint.h>
#include <minix/sysutil.h>
#include <minix/const.h>
#include <minix/type.h>
#include <minix/syslib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <machine/vm.h>
#include <machine/vmparam.h>
#include <sys/vm.h>

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define PROC_NR NR_PROCS

int do_lock(message *);
int do_unlock(message *);
int do_wait(message *);
int do_broadcast(message *);
int handle_proc_intr(message *);
int clear_all_structures(int);
int clear_queues(int);

int init_mutex_list(void);
int search_taken_mutex_list(int, int *);
endpoint_t remove_taken_mutex(int);
int take_mutex(int, endpoint_t);
endpoint_t dequque(int);
int add_to_queue(int, endpoint_t);
int add_to_suspend_list(int, int, int);

int unsuspend_cond(int);
int clear_structures(int);

EXTERN endpoint_t who_e;
EXTERN int call_type;
EXTERN endpoint_t SELF_E;
