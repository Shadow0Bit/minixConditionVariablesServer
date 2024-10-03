#include "inc.h"

#define MUTEXES_MAX 1024
#define PROC_NR NR_PROCS

static struct {
    int mutex;
    endpoint_t owner;
} taken_mutexes[MUTEXES_MAX];

static struct {
    endpoint_t proc;
    int mutex;
} queue[PROC_NR];

int queue_top = 0; 
int queue_back = 0; 

static struct {
    int mutex;
    int cond;
    endpoint_t proc;
} suspended_procs[PROC_NR];

int next_available = 0;

int init_mutex_list() 
{
    for (int i = 0; i < MUTEXES_MAX; i++) {
        taken_mutexes[i].mutex = -1;
        taken_mutexes[i].owner = -1;
    }

    for (int i = 0; i < PROC_NR; i++) {
        queue[i].proc = -1;
        queue[i].mutex = -1;
    }

    for (int i = 0; i < PROC_NR; i++) {
        suspended_procs[i].mutex = -1;
        suspended_procs[i].proc = -1;
        suspended_procs[i].cond = -1;
    }

    return 0;
}

int clear_queues(int proc) 
{
    int res = -1;
    for (int i = 0; i < queue_back; i++) {
        if (queue[i].proc == proc) {
            for (int j = i; j < queue_back - 1; j++) {
                queue[j].mutex = queue[j + 1].mutex;
                queue[j].proc = queue[j + 1].proc;
            }
            queue_back--;
	    res = 0;
            break;
        }
    }

    for (int i = 0; i < PROC_NR; i++) {
        if (suspended_procs[i].proc == proc) {
            suspended_procs[i].mutex = -1;
            suspended_procs[i].proc = -1;
            suspended_procs[i].cond = -1;
	    res = 0;
        }
    }

    return res;
}

int clear_all_structures(int proc) {
   int r = clear_queues(proc);
   for (int i = 0; i < MUTEXES_MAX; i++) {
       if (taken_mutexes[i].owner == proc) {
           int new_owner = remove_taken_mutex(i);	    
	   if (new_owner != 0) {
	       message m;
	       m.m_type = 0;
	       sendnb(new_owner, &m);
	   }
	   r = 0;
       }
   }  
   return r;
}

int add_to_suspend_list(int cond, int mutex, endpoint_t proc) 
{
    for (int i = 0; i < PROC_NR; i++) {
        if (suspended_procs[i].proc == -1) {
            suspended_procs[i].mutex = mutex;
            suspended_procs[i].proc = proc;
            suspended_procs[i].cond = cond;
            return 0;
        }
    }

    return -1;
}

int unsuspend_cond(int cond) 
{
    for (int i = 0; i < PROC_NR; i++) {
        if (suspended_procs[i].cond == cond) {
            int owner = -1;
            int search_mutex = search_taken_mutex_list(suspended_procs[i].mutex, &owner);
            if (search_mutex != -1) {
                add_to_queue(suspended_procs[i].mutex, suspended_procs[i].proc);
            } else {
                take_mutex(suspended_procs[i].mutex, suspended_procs[i].proc);
		message m;
		m.m_type = 0;
                sendnb(suspended_procs[i].proc, &m);
            }
            suspended_procs[i].mutex = -1;
            suspended_procs[i].proc = -1;
            suspended_procs[i].cond = -1;
        }
    }
    return 0;
    
}

int search_taken_mutex_list(int mutex, endpoint_t *owner) 
{
    for (int i = 0; i < MUTEXES_MAX; i++) {
        if (taken_mutexes[i].mutex == mutex) {
            *owner = taken_mutexes[i].owner;
            return i;
        } else if (taken_mutexes[i].mutex == -1) {
            if (i < next_available || next_available == -1) next_available = i;
        }
    }

    *owner = 0;
    return -1;
}

endpoint_t remove_taken_mutex(int mutex_pos) 
{
    endpoint_t new_owner = -1;
    if (queue_top != queue_back) {
        new_owner = dequeue(taken_mutexes[mutex_pos].mutex);
	printf("new_owner %d", new_owner);
        if (new_owner != 0) { 
            taken_mutexes[mutex_pos].owner = new_owner;
            return new_owner;
        }
    }

    taken_mutexes[mutex_pos].mutex = -1;
    taken_mutexes[mutex_pos].owner = -1;
    if (mutex_pos < next_available || next_available == -1) next_available = mutex_pos;
    return 0;
}

int take_mutex(int mutex, endpoint_t owner) 
{
    if (next_available == -1) {
        for (int i = 0; i < MUTEXES_MAX; i++) {
            if (taken_mutexes[i].mutex == -1) {
                next_available = i;
                break;
            }
        }
    }
    
    taken_mutexes[next_available].mutex = mutex;
    taken_mutexes[next_available].owner = owner;
    next_available = -1;
    
    return 0;
}

endpoint_t dequeue(int mutex) 
{
    for (int top = 0; top < queue_back; top++) {
	printf("mutex: %d %d\n", queue[top].mutex, mutex);
        if (queue[top].mutex == mutex) {
            endpoint_t proc = queue[top].proc;
            for (int i = top; i < queue_back; i++) {
                queue[i].mutex = queue[i + 1].mutex;
                queue[i].proc = queue[i + 1].proc;
            }
            queue_back--;
            return proc;
        }
    } 
    return 0;
}

int add_to_queue(int mutex, endpoint_t proc) 
{
    queue[queue_back].mutex = mutex;
    queue[queue_back].proc = proc;
    queue_back++;
    return 0;
}

