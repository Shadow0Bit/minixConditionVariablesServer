#include "inc.h"

#define PROC_STOPPED 0
#define PROC_INTERRUPTED 1

/* handle the process interruption */
int handle_proc_intr(message *m) {
    int r;
    switch (m->m1_i1)
    {
    case PROC_STOPPED:
	    r = clear_all_structures(m->m1_i2);
	    if (r == -1) return -1;
        break;
    
    case PROC_INTERRUPTED:
        r = clear_queues(m->m1_i2);
	    if (r == -1) return -1;
        m->m_type = EINTR;
        sendnb(m->m1_i2, m);
        break;

    default:
        break;
    }

    return OK;
}

