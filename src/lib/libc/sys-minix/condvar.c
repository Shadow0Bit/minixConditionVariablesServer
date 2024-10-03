#include <sys/cdefs.h>
#include <lib.h>
#include "namespace.h"
#include <cv.h>
#include <unistd.h>

#define OK 0

static int get_cvs_endpt(endpoint_t *pt)
{
    return minix_rs_lookup("cvs", pt);  
}

size_t cs_wait(int cond_var_id, int mutex_id) 
{
    endpoint_t who;
    message m;

    while (1) {
        if (get_cvs_endpt(&who) != OK) {
            errno = ENOSYS;
            return -1;
        }
        m.m1_i1 = 2;
        m.m1_i2 = mutex_id;
        m.m1_i3 = cond_var_id;

        int r = _syscall(who, 1, &m);
        
        if (r == -1 && errno == EINTR) {
            return cs_lock(mutex_id);
        }
	
	return r;
    }
}

size_t cs_broadcast(int cond_var_id) 
{
    endpoint_t who;
    message m;

    while (1) {
        if (get_cvs_endpt(&who) != OK) {
            errno = ENOSYS;
            return -1;
        }
        m.m1_i1 = 3;
        m.m1_i3 = cond_var_id;
        int r = _syscall(who, 1, &m);
        
        if (r == -1 && errno == EINTR) {
            continue;
        }

        return r;
    }

}
