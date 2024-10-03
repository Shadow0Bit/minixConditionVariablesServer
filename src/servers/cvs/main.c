#include "inc.h"

endpoint_t who_e;
int call_type;
endpoint_t SELF_E;

static struct {
    int (*func)(message *m);
} calls[] = {
    { do_lock },
    { do_unlock },
    { do_wait },
    { do_broadcast },
};

static void sef_local_startup();
static int sef_fresh(int type, sef_init_info_t *info);
static void sef_handler(int signo);

/* move this elsewhere */
#define CVS_CALL 1
#define PROC_INT 0 

int main(int argc, char *argv[]) 
{
    message m;
    init_mutex_list();
    sef_local_startup();

    while(TRUE) {
        int r;

        if ((r = sef_receive(ANY, &m)) != OK) { 
            printf("CVS sef_receive error: %d\n", r);
            continue;
        }

        who_e = m.m_source;
        call_type = m.m_type;

        int cvs_numb;
        switch (call_type)
        {
        case PROC_INT:
            handle_proc_intr(&m);
            break;
        case CVS_CALL:
            cvs_numb = m.m1_i1;
            calls[cvs_numb].func(&m);
            break;
        default:
            break;
        }

    }

    /* can't get here */
    return -1;
}

static void sef_local_startup()
{
    sef_startup();
}