# Makefile for CVS server
PROG=   cvs
SRCS=   main.c queue.c mutex.c condvar.c utility.c

DPADD+= ${LIBSYS}
LDADD+= -lsys

MAN=

BINDIR?= /usr/sbin
FILES=cvs.conf
FILESNAME=cvs
FILESDIR= /etc/system.conf.d

.include <minix.service.mk>
