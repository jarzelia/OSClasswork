//Eric Wang
//eric_wang1@student.uml.edu
//code taken from the call help assign 3 file.
/*** need a  .h  file with content as shown below         ***/

#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/signal.h>
#include <stdio.h>
#include <stdlib.h>

#define		SEMKEY			(key_t)200000000
#define		MEMKEY			(key_t)200000000
#define		NUMFLAVORS	 	4
#define		NUMSLOTS       	50
#define		NUMSEMIDS	 	3
#define		PROD		 	0
#define		CONSUMER	 	1
#define		OUTPTR		 	2

struct	donut_ring{
	int	flavor  [NUMFLAVORS]  [NUMSLOTS];
	int	outptr  [NUMFLAVORS];
};

extern int		p (int, int);
extern int		v (int, int);
extern int		semsetall (int, int, int);
