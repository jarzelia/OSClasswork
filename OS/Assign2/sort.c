//Eric Wang
//eric_wang1@student.uml.edu
//Some code taken from ~bill/cs305 assignment 7 help file
//Some code taken from ~bill/cs308 assignment 1 help file

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
    int     childOut[2], childIn[2];

    //creates the pipe, if fails, then it will say so.
    if(pipe(childOut) == -1 || pipe(childIn) == -1)
    {
        perror("Failed in pipe creation.\n");
        exit(1);
    }

    //process ID not needed
    //creation of children starts here
    switch(fork())
    {

    case -1: //call failed
        perror("Failed in child creation.\n");
        exit(8);

    case 0 : //child process

        //Child process to close stdout
        //and use parent's stdout/in
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

        //start sort program
        //first field is area code, then last name, then first name
        execlp("sort", "sort", "-k", "3.3n", "-k", "1.1", "-k", "2.2", NULL);

        //in case of execlp failure
        perror("Sort has failed to run correctly.\n");
        exit(4);

    default : //parent code
        //closing unneeded pipes, no more need for read in/write out
        if(close(childOut[0]) == -1 || close(childIn[1]) == -1)
        {
            perror("Parent could not close pipes.\n");
            exit(3);
        }

        int file;
        file = open("cs308a2_sort_data", O_RDONLY, 0);

        //Read data into the pipe
        //Data then goes into the child pipe
        char buffer[512];
        int numIn;

        while((numIn = read(file, buffer, 80)))
        {
            if((write(childOut[1], buffer, numIn)) == -1)
            {
                printf("Parent could not write to child.\n");
                exit(2);
            }

        }

        //close more unneeded pipes
        if(close(childOut[1]) == -1)
        {
            perror("Parent could not close pipe.\n");
            exit(3);
        }

        //open child's output
        FILE* fp = fdopen(childIn[0], "r");

        //number of people in the area code
        int aCodeCount = 0;

        //old area code
        int oldACode = -1;

        char firstName[20], lastName[20];
        int aCode = 0;
        int prefix, tel;


        while(fscanf(fp, "%s %s %d %d %d\n", lastName, firstName, &aCode, &prefix, &tel) != EOF)
        {

            if (oldACode == aCode) //match found, increment counter
            {
                aCodeCount++;
            }
            else if (oldACode == -1)  //starting run, increment counter
            {
                oldACode = aCode;
                aCodeCount++;
            }
            else        //new area code
            {
                //print area code info, reset counter
                printf("Area code %d appears %d times.\n", oldACode, aCodeCount);
                oldACode = aCode;
                aCodeCount = 1;
            }

        }


        //print last area code, since the loop doesn't run it again.
        printf("Area code %d appears %d times.\n", oldACode, aCodeCount);

        //close last child.
        if ((close(childIn[0])) == -1)
        {
            perror("Could not close last pipe.\n");
            exit(10);
        }

        fclose(fp);


    } //switch end


    return 0;

}
