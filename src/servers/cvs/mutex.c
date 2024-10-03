#include "inc.h"

int do_lock(message *m) 
{
    endpoint_t owner;
    int mutex_pos = search_taken_mutex_list(m->m1_i2, &owner);
    if (mutex_pos == -1) {
        take_mutex(m->m1_i2, m->m_source);
	    m->m_type = 0;
        sendnb(m->m_source, m);
    } else if (owner != m->m_source) {
        add_to_queue(m->m1_i2, m->m_source);
    } else {
    	m->m_type = 0;
        sendnb(m->m_source, m);
    }

    return OK;
}

int do_unlock(message *m) 
{
    endpoint_t owner;
    int mutex_pos = search_taken_mutex_list(m->m1_i2, &owner);    
    if (owner == m->m_source && mutex_pos != -1) {
        endpoint_t new_owner = remove_taken_mutex(mutex_pos);
    	m->m_type = 0;
        if (new_owner > 0) {
            m->m1_i1 = new_owner;
            sendnb(new_owner, m);
        }
    	sendnb(m->m_source, m);
    } else {
    	errno = EPERM;
        m->m_type = EPERM;
        sendnb(m->m_source, m);
        return -1;
    }
    
    return OK;
}
