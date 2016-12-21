//Eric Wang
//eric_wang1@student.uml.edu
//Some code taken from ~bill/cs305 assignment 7 help file
//Some code taken from ~bill/cs308 assignment 1 help file
//This is the second grep file.


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
    int     childOut[2], childIn[2];
    int     file, numIn;
    char    buffer[80];

    //creates the pipe, if fails, then it will say so.
    if(pipe(childOut) == -1 || pipe(childIn) == -1)
    {
        perror("Failed in pipe creation.\n");
        exit(7);
    }

    //process ID not needed
    //creation of children starts here
    switch(fork())
    {

    case -1: // call failed
        perror("Failed in child creation.\n");
        exit(8);

    case 0 :
    {
        //Child process to close stdout
        if (close(0) == -1)
        {
            perror("Child could not close stdout.\n");
            exit(6);
        }

        //child to open parents stdout as its own
        if (dup(childOut[0]) != 0)
        {
            perror("Child can not use parents stdout.\n");
            exit(6);
        }

        //child to close stdin
        if (close(1) == -1)
        {
            perror("Child could not close stdin.\n");
            exit(6);
        }

        //parent uses child in
        if (dup(childIn[1]) != 1)
        {
            perror("Child could not use parents in.\n");
            exit(6);
        }

        //close other pipes
        if ( close(childOut[0]) == -1 || close(childOut[1]) == -1 ||
                close(childIn[0]) == -1 || close(childIn[1]) == -1)
        {

            perror("Child could not close excess pipes.\n");
            exit(6);
        }

        //start grep process
        execlp("grep", "grep", "123", NULL);

        //grep fail
        perror("grep has failed to run correctly.\n");
        exit(6);
    }

    default : //parent block

        //closing unneeded pipes, no more need for read in/write out
        if(close(childOut[0]) == -1 || close(childIn[1]) == -1)
        {
            perror("Parent could not close pipes.\n");
            exit(3);
        }

        //open the file.
        if((file = open("cs308a2_grep_data_2", O_RDONLY, 0)) == -1)
        {
            perror("Parent could not open file.\n");
            exit(2);
        }

        //Read the data from the file
        //Automatically goes to
        while((numIn = read(file, buffer, 80)) != 0)
        {
            if(numIn == -1)
            {
                printf("write error.\n");
                exit(3);
            }

            if(write(childOut[1], buffer, numIn) == -1)
            {
                perror("Parent could not write to child.\n");
                exit(3);
            }
        }

        //close more unneeded pipes
        if(close(childOut[1]) == -1)
        {
            perror("Parent could not close pipe.\n");
            exit(3);
        }

        //close file
        if(close(file) == -1)
        {
            perror("Parent could not close file.\n");
            exit(3);
        }

        //create a file for grep output for line counting
        FILE* fp = fopen("count", "w+");

        file = open("count", O_RDWR);

        //put grep info into the file and count the lines.
        int count = 0;
        while((numIn = read(childIn[0], buffer, sizeof(buffer))) != 0)
        {
            write(file, buffer, numIn);
        }

        //count the lines, since each line  has one hit
        while(fgets(buffer, sizeof(buffer), fp) != NULL)
        {
            count++;
        }

        printf("grep found it %d times.\n", count);
        if ((close(childIn[0])) == -1)
        {
            perror("Child could not close last pipe.\n");
            exit(10);
        }
        break;

    } // switch


    return 0;

}
