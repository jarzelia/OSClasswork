//Eric Wang
//Assignment 6
//Code taken from assignment 6 file and in class

#define _FILE_OFFSET_BITS       64

#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h> // file type/prot macros
#include<sys/sysmacros.h> // major/minor macros
#include<stdio.h>
#include<stdlib.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>
#include<errno.h>

extern int errno;
/* the things in struct for reference
 *
 * struct stat {
 *      dev_t st_dev;  device inode resides on  //unneeded
 *      ino_t st_ino;  this inode's number
 *      mode_t st_mode;  protection
 *      nlink_t st_nlink;  number or hard links to the file
 *      uid_t st_uid;  user-id of owner
 *      gid_t st_gid;  group-id of owner
 *      dev_t st_rdev;  dev type, for inode that is dev
 *      off_t st_size;  total size of file
 *      time_t st_atime;  file last access time //unneeded
 *      time_t st_mtime;  file last modify time
 *      time_t st_ctime;  file last status change time
 *      uint_t st_blksize;  opt blocksize for file sys i/o ops //unneeded
 *      int st_blocks;  actual number of blocks allocated //unneeded
 * ……….. etc………..
 *  };
 *
 */


/*
Need to print
FILE_TYPE
PERMISSIONS
OWNER_NAME
GROUP_NAME
DATE_OF_LAST_MODIFICATION
LINK_COUNT
SIZE_IN_BYTES OR DEV INFO
INODE_NUMBER
*/
void printDetails(struct stat statIn)
{
    char permissions[10];
    struct passwd* pwd;
    struct group* group;
    int i;

    permissions[9] = '\0';

    //start testing file types
    if(S_ISDIR(statIn.st_mode))
    {
        printf("TYPE:\t\t\tdirectory\n");
    }

    if(S_ISCHR(statIn.st_mode))
    {
        printf("TYPE:\t\t\tchar device\n");
    }

    if(S_ISBLK(statIn.st_mode))
    {
        printf("TYPE:\t\t\tblock device\n");
    }

    if(S_ISREG(statIn.st_mode))
    {
        printf("TYPE:\t\t\tregular file\n");
    }

    if(S_ISFIFO(statIn.st_mode))
    {
        printf("TYPE:\t\t\tpipe\n");
    }

    if(S_ISLNK(statIn.st_mode))
    {
        printf("TYPE:\t\t\tsym link\n");
    }

    //permission check
    //set defaults
    for(i = 0; i < 9; i++)
    {
        permissions[i] = '-';
    }

    if(statIn.st_mode & S_IRUSR)
    {
        permissions[0] = 'r';
    }

    if(statIn.st_mode & S_IWUSR)
    {
        permissions[1] = 'w';
    }

    if(statIn.st_mode & S_IXUSR)
    {
        permissions[2] = 'x';
    }

    if(statIn.st_mode & S_IRGRP)
    {
        permissions[3] = 'r';
    }

    if(statIn.st_mode & S_IWGRP)
    {
        permissions[4] = 'w';
    }

    if(statIn.st_mode & S_IXGRP)
    {
        permissions[5] = 'w';
    }

    if(statIn.st_mode & S_IROTH)
    {
        permissions[6] = 'r';
    }

    if(statIn.st_mode & S_IWOTH)
    {
        permissions[7] = 'w';
    }
    if(statIn.st_mode & S_IXOTH)
    {
        permissions[8] = 'x';
    }

    printf("PERMISSIONS:\t\t%s\n", permissions);

    //Owner
    pwd = getpwuid(statIn.st_uid);
    if( pwd == NULL)
    {
        printf("OWNER:\t\t\t%d\n", statIn.st_uid);
    }
    else
    {
        printf("OWNER:\t\t\t%s\n", pwd -> pw_name);
    }

    //group info
    group = getgrgid(statIn.st_gid);
    if( group == NULL)
    {
        printf("GROUP:\t\t\t%d\n", statIn.st_gid);
    }
    else
    {
        printf("GROUP:\t\t\t%s\n", group -> gr_name);
    }

    //Date info
    printf("LAST MODIFIED:\t\t%s", ctime(&(statIn.st_mtime)));

    //link info
    printf("LINKS:\t\t\t%d\n", statIn.st_nlink);

    //device type or size
    if((S_ISCHR(statIn.st_mode))|| S_ISBLK(statIn.st_mode))
    {
        printf("MAJOR, MINOR:\t\t%d , %d\n", major(statIn.st_rdev), minor(statIn.st_rdev));
    }
    else
    {
        printf("FILE SIZE:\t\t%d\n", statIn.st_size);
    }

    //inode
    printf("INODE:\t\t\t%llu\n\n", statIn.st_ino);

    return;
}

int main(int argc, char* argv[])
{

    struct stat statIn;
    struct dirent* entptr;
    DIR* dir;
    int i, j;

    if (argc > 1) //check for how many files in
    {
        //start opening files and printing info
        for(i = 1; i < argc; i++)
        {
            if(lstat(argv[i], &statIn) == -1)
            {
                //open failed
                perror("File could not be located.\n");
                exit(10);
            }
            printf("\nFILENAME:\t\t%s\n",argv[i]);
            printDetails(statIn);

        }
    }
    else  //no input, do .
    {
        //check for current directory
        if((dir = opendir(".")) == NULL)
        {
            perror("Could not open current directory.\n");
            exit(9);
        }

        //read pointer, get start of all files
        entptr = readdir(dir);
        //start looping all files
        while (entptr != NULL)
        {
            if(lstat(entptr -> d_name, &statIn) == -1)
            {
                //open failed
                perror("File could not be located.\n");
                exit(10);
            }
            printf("\nFILENAME:\t\t%s\n",entptr -> d_name);
            printDetails(statIn);
            entptr = readdir(dir);
        }

    }
    return 0;
}


