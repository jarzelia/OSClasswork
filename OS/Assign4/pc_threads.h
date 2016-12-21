//Eric Wang
//eric_wang1@student.uml.edu
//code taken from the call help assign 3 file.
//code taken from the call help assign 4 file.

#define _GNU_SOURCE
#include <sched.h>
#include <utmpx.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <linux/unistd.h>
#include <strings.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <errno.h>

#define NUMFLAVORS       4
#define NUMSLOTS         30000
#define NUMCONSUMERS     50
#define NUMPRODUCERS     30


struct  donut_ring
{
    int     flavor[NUMFLAVORS][NUMSLOTS];
    int     outptr[NUMFLAVORS];
    int     in_ptr[NUMFLAVORS];
    int     serial[NUMFLAVORS];
    int     spaces[NUMFLAVORS];
    int     donuts[NUMFLAVORS];
};

/**********************************************************************/
/* SIGNAL WAITER, PRODUCER AND CONSUMER THREAD FUNCTIONS              */
/**********************************************************************/

void* sig_waiter ( void *arg );
void* producer   ( void *arg );
void* consumer ( void *arg);
void sig_handler ( int );

/**********************    END HEADER FILE         *******************/

