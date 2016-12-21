//Eric Wang
//eric_wang1@student.uml.edu
//Some code taken from ~bill/cs305 assignment 7 help file
//Some code taken from ~bill/cs308 assignment 1 help file


// need these include files

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/wait.h>

int counter = 0, counter_2G = 0;

typedef union
{
    int exit_status;

    struct
    {
        unsigned sig_num:7;
        unsigned core_dump:1;
        unsigned exit_num:8;
    } parts;
} LE_Wait_Status;

void sig_handler(int signal)
{

    sigset_t mask;
    sigemptyset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    printf("Starting assignment program.\n");
    execl("./assign1", "assign1", NULL);

    //if execl returns, that means it failed.
    perror("execl for assign1 failed.\n");
    exit(10);

}

// need just a main function, no args

int main(void)
{
    // some local variables
    pid_t   pid, ppid;
    int     ruid, rgid, euid, egid;
    int     priority;
    char    msg_buf[100];
    int     msg_pipe[2];
    struct sigaction newsig;
    sigset_t	mask, proc_mask;

    // use the pipe() system call to create the pipe

    if(pipe(msg_pipe) == -1)
    {
        perror("failed in Parent pipe creation:");
        exit(7);
    }

    // use various system calls to collect and print process details

    printf("\nThis is the Parent process report:\n");
    pid  = getpid();
    ppid = getppid();
    ruid = getuid();
    euid = geteuid();
    rgid = getgid();
    egid = getegid();
    priority = getpriority(PRIO_PROCESS, 0);

    //print credentials
    printf("\nPARENT PROG:  Process ID is:\t\t%d\n\
    PARENT PROC:  Process parent ID is:\t%d\n\
    PARENT PROC:  Real UID is:\t\t%d\n\
    PARENT PROC:  Real GID is:\t\t%d\n\
    PARENT PROC:  Effective UID is:\t\t%d\n\
    PARENT PROC:  Effective GID is:\t\t%d\n\
    PARENT PROC:  Process priority is:\t%d\n",
           pid, ppid, ruid, rgid, euid, egid, priority);


    // now use the fork() call to create the child:
    //
    // format is:
    //
    // 	switch (pid = fork()){
    //        case -1: // if the call failes
    //
    //        default: // this is the parent's case
    //                 // parent must write message to pipe and
    //                 // do a normal exit
    //
    //        case 0:  // this is the child's case
    //		   // child must create and print report
    //                 // child must read pipe message and print
    //		   // a modified version of it to output
    //		   // child must do a normal exit

    switch (pid = fork())
    {
    case -1:        // call failed
        perror("failed in child creation:\n");
        exit(6);

    default:        //parent call
        printf("PARENT PROC: Created child with pid: %d\n", pid);
        break;


    case 0:         //child call
//
//        struct sigaction newsig;
//        sigset_t	mask, proc_mask;

        sigemptyset(&proc_mask);                     /** clear signal mask **/
        sigprocmask(SIG_SETMASK, &proc_mask, NULL);

        sigemptyset(&mask);

        newsig.sa_mask = mask;
        newsig.sa_handler = sig_handler;
        newsig.sa_flags = 0;

        if  (sigaction(SIGTERM, &newsig, NULL) == -1)
        {
            perror("sig_handler setup failed.\n");
            exit(2);
        }


        printf("\nThis is the Child process report:\n");
        pid  = getpid();
        ppid = getppid();
        ruid = getuid();
        euid = geteuid();
        rgid = getgid();
        egid = getegid();
        priority = getpriority(PRIO_PROCESS, 0);


        printf("\nPARENT PROG:  Process ID is:\t\t%d\n\
    CHILD PROC:  Process parent ID is:\t%d\n\
    CHILD PROC:  Real UID is:\t\t%d\n\
    CHILD PROC:  Real GID is:\t\t%d\n\
    CHILD PROC:  Effective UID is:\t\t%d\n\
    CHILD PROC:  Effective GID is:\t\t%d\n\
    CHILD PROC:  Process priority is:\t%d\n",
               pid, ppid, ruid, rgid, euid, egid, priority);

        if((write(msg_pipe[1],"a", 1)) == -1)
        {
            perror("Child write to pipe failed.\n");
            exit(50);
        }

        while (counter_2G < 10)
        {
            counter ++;
            if (counter < 0)
            {
                counter = 0;
                counter_2G++;
            }
        }
        printf("CHILD PROG: timed out after 20 billion iterations\n");
        exit(2);

//        read(msg_pipe[0], msg_buf, 100);
//
//        printf("CHILD PROC: parent's msg is %s\n", msg_buf);
//        printf("CHILD PROC: Process parent ID now is:   %d\n",
//               getppid());
//        printf("CHILD PROC: ### Goodbye ###\n");
//        exit(0);
    } // switch and child end

    //start more parent code
    //wait for read in.
    if((read(msg_pipe[0], msg_buf, 1)) == -1)
    {
        perror("Parent read from pipe failed.\n");
        exit(51);
    }

    //kill child process
    kill(pid, SIGTERM);
    printf("Child to be killed.\n");

    //wait for child death
    LE_Wait_Status wait;
    if((pid = waitpid(-1, &wait.exit_status, 0)) == -1)
    {
        printf("Parent wait failed.\n");
        exit(49);
    }

    //exit
    printf("PARENT PROC: Child with pid %d killed with the following information.\n", pid);
    if (wait.parts.sig_num == 0)
    {
        printf("Exit code: %d\n", wait.parts.exit_num);
    }
    else
    {
        printf("Signal code: %d\n", wait.parts.sig_num);
    }

    return 0;
}
