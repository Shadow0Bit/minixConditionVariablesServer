#include "inc.h"

// add_to_suspend_list - add a process to the suspend list of a condition variable, 
// given the condition variable, the mutex and the process

//unsuspend_all - unsuspend all processes in the suspend list of a condition variable and give them the mutex 
// or add them to the queue

int do_wait(message *m) 
{
    endpoint_t owner;
    int mutex_pos = search_taken_mutex_list(m->m1_i2, &owner);
    if (owner == m->m_source && mutex_pos != -1) {
    	endpoint_t new_owner = remove_taken_mutex(mutex_pos);
	if (new_owner != 0) {
	    m->m_type = 0;
	    sendnb(new_owner, m);
	}
        add_to_suspend_list(m->m1_i3, m->m1_i2, m->m_source);
    } else {
    	errno = EPERM;
	m->m_type = EPERM;
	sendnb(m->m_source, m);
	return -1;
    }
    return OK;
}

int do_broadcast(message *m) 
{
    unsuspend_cond(m->m1_i3);
    m->m_type = 0;
    sendnb(m->m_source, m);
    return OK;
}

