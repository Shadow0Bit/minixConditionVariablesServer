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

size_t cs_lock(int mutex_id)
{
    endpoint_t who;
    message m;

    while (1) {
        if (get_cvs_endpt(&who) != OK) {
            errno = ENOSYS;
            return -1;
        }
        
        m.m1_i1 = 0;
        m.m1_i2 = mutex_id;
        int r = _syscall(who, 1, &m);
        
        if (r == -1 && errno == EINTR) {
           continue;
	    } 

    	return r;
    } 
}

size_t cs_unlock(int mutex_id) 
{
    endpoint_t who;
    message m;
    
    if (get_cvs_endpt(&who) != OK) {
        errno = ENOSYS;
        return -1;
    }
    
    m.m1_i1 = 1;
    m.m1_i2 = mutex_id;
    int r = _syscall(who, 1, &m);
    
    return r;    
}
