#ifndef _CV_H
#define _CV_H

#include <stddef.h>

size_t cs_lock(int);
size_t cs_unlock(int);

size_t cs_wait(int, int);
size_t cs_broadcast(int);

#endif /* !_CV_H_ */