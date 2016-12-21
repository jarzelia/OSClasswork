//Eric Wang
//eric_wang1@student.uml.edu
//code taken from the call help assign 3 file.

#include "donuts.h"


int main()
{

    int i, j, rand;

    int flavors[NUMFLAVORS][10];
    //time struct creation
    struct tm* aTime;

    //shared memory
    int shmid, semid[3];

    //consumers
    int c1, c2, c3, c4, c5;
    struct donut_ring* shared_ring;

    struct timeval randtime;
    char myTime[40];

    //connect to donut ring
    if((shmid = shmget(MEMKEY, 0, 0)) == -1)
    {
        perror("Consumer could not get shared memory.\n");
        exit(1);
    }

    //attach to shared memory.
    if((shared_ring = (struct donut_ring *) shmat(shmid, NULL, 0)) == -1)
    {
        perror("Consumer could not attach to memory.\n");
        exit(2);
    }

    //semaphore allocation
    for( i = 0; i < NUMSEMIDS; i++ )
    {
        if((semid[i] = semget(SEMKEY + i, NUMFLAVORS, IPC_CREAT | 0600)) == -1)
        {
            perror("Consumer failed to attach to create semaphores.\n");
            exit(3);
        }
    }


    //consumer section trial
    for(i = 0; i < 10; i++)
    {
        //clear
        c1 = 0;
        c2 = 0;
        c3 = 0;
        c4 = 0;
        c5 = 0;

        //get donuts dozen
        for(j = 0; j < 12; j++)
        {
            //get random num for donut
            gettimeofday(&randtime, (struct timezone *)0);

            unsigned short xsub1[3];
            xsub1[0] = (ushort) randtime.tv_usec;
            xsub1[1] = (ushort)(randtime.tv_usec >> 16);
            xsub1[2] = (ushort)(getpid());
            rand = nrand48(xsub1) & 3;

            //lock data, collect donut
            p(semid[OUTPTR], rand);
            p(semid[CONSUMER], rand);

            int taste = shared_ring -> flavor[rand][shared_ring -> outptr[rand]];
            switch(rand)
            {
            case 0:
            {
                flavors[rand][c1] = taste;
                c1++;
                break;
            }

            case 1:
            {
                flavors[rand][c2] = taste;
                c2++;
                break;
            }


            case 2:
            {
                flavors[rand][c3] = taste;
                c3++;
                break;
            }

            case 3:
            {
                flavors[rand][c4] = taste;
                c4++;
                break;
            }

            } //end switch

            //consume donut, watch out for too many donuts
            if (shared_ring -> outptr[rand] >= NUMSLOTS)
            {
                shared_ring -> outptr[rand] = 0;
            }
            else
            {
                shared_ring -> outptr[rand] = shared_ring -> outptr[rand] + 1;
            }

            //unlock semaphores, producer can make more
            v(semid[PROD], rand);
            v(semid[OUTPTR], rand);

        }

        //START PRINT BLOCK
        //get more time
        aTime = localtime(&randtime.tv_sec);

        //format time
        strftime(myTime, sizeof(myTime), "%H:%M:%S", aTime);
        long milli = randtime.tv_usec / 1000;

        printf("\nProcess PID: %d", getpid());
        printf("\nTime: %s", myTime, milli);
        printf("\nDozen: %d\n", i + 1);

        //flavor selection
        printf("Glaze\tChoc\tCream\tPlain\n");

        //print info
        int a;
        for(a = 0; a < 12; a++)
        {
            printf("%d\t%d\t%d\t%d\n",
                   flavors[0][a], flavors[1][a], flavors[2][a], flavors[3][a]);
        }

        //reset
        for(a = 0; a < NUMFLAVORS; a++)
        {
            for(j = 0; j < 12; j++)
            {
                flavors[a][j] = 0;
            }
        }

    } //end loop

    return 0;

}
