//Eric Wang
//eric_wang1@student.uml.edu
//code taken from the assign 5 data structs file.


#include "main.h"
#include <math.h>

int allocateMemoryb(struct requests* request)
{
    //get the buddy size needed;
    int x;
    //x = ceil(log2(request->size));
    int i;

    //extract smallest size available
    for(i = x; i <= 21; i++)
    {
        //find first open
        if(lst_ary[i].head -> next == NULL)
        {
            continue;
        }
        else if ((lst_ary[i].head -> next) != NULL && i == x)
        {
            //list array next next is null, BUT BLOCK IS FOUND
            if (lst_ary[i].head -> next -> next == NULL)
            {
                request -> is_allocated = TRUE;
                request -> base_adr = lst_ary[i].head -> next -> adr;
                //request -> next_boundary_adr = request -> base_adr + pow(2,i);
                lst_ary[i].head -> next = NULL;
            }
            else //list next next is not null, BUT BLOCK IS FOUND
            {
                lst_ary[i].head -> next -> next -> prev = lst_ary[i].head;
                lst_ary[i].head -> next = lst_ary[i].head -> next -> next;
                request -> is_allocated = TRUE;
                break;
            }
        }
        else if (lst_ary[i].head -> next != NULL)
        {
            break;
        }
    }
}


int update_listc(int index)
{

    //temp free lists declarations
    struct free_list* free;
    struct free_list* newB;
    struct free_list* combine;

    //not allocated, no need to modify
    if(req_arrayb[index].is_allocated == FALSE)
    {
        return 0;
    }

    //block needs to be free/marked done
    req_arrayb[index].is_allocated = DONE;
    total_free += req_arrayb[index].size;

    //scan the whole list for possible combinations
    for(free = list_head.next; free; free = free -> next)
    {

        //free newB does not pair up with checked newB
        if(req_arrayb[index].base_adr > free -> block_adr)
        {
            continue;
        }

        //create a new block to store
        newB = malloc(sizeof(struct free_list));
        newB -> block_size = req_arrayb[index].size;
        newB -> block_adr = req_arrayb[index].base_adr;
        newB -> adjacent_adr = newB -> block_adr + newB -> block_size;

        newB->next = free;
        free -> previous -> next = newB;
        newB -> previous = free -> previous;
        free -> previous = newB;

        //combine with next?
        if(newB -> adjacent_adr == newB -> next -> block_adr)
        {
            combine = newB -> next;
            newB -> block_size = newB -> block_size + combine -> block_size;
            newB -> adjacent_adr = combine -> adjacent_adr;
            newB -> next = combine -> next;

            if(newB -> next)
            {
                newB -> next -> previous = newB;
            }

        }

        //combine with previous?
        newB = newB -> previous;
        //check that you aren't at the beginning already
        if((newB != NULL) && (newB -> adjacent_adr != 0))
        {
            if(newB -> adjacent_adr == newB -> next -> block_adr)
            {
                combine = newB->next;
                newB -> block_size = newB -> block_size + combine -> block_size;
                newB -> adjacent_adr = combine -> adjacent_adr;
                newB -> next = combine -> next;

                if(newB -> next)
                {
                    newB ->next -> previous = newB;
                }

            }
        }

        return 0;

    }
}


int buddy(int mem, char* fileIn)
{

    int i = 0;
    int reqSeq = 0;       //which request it is
    int reqSize = 0;      //how large the request is
    char reqType[20];     //string of which request it is

    //file to read from
    FILE* file = fopen(fileIn, "r");

    //free space
    struct free_list* free;

    //all free space
    total_free_space = (mem * 1024);
    total_free = (mem * 1024);

    //request array. empty as no requests
    //DEFAULT VALUES
    for(i = 0; i < NUMBER_ENTRIES; i++)
    {
        req_arrayb[i].is_req = FALSE;
        req_arrayb[i].is_allocated = FALSE;
    }


    //collect the highest power
    int power;
    //power = log2(total_free_space);

    //blank the array
    for(i = 0; i <= 21; i++)
    {
        lst_ary[i].head -> next = NULL;
    }


    //head placeholder
    struct lel* blank;
    blank -> prev = NULL;
    blank -> next = NULL;
    blank -> adr = 0;
    blank -> bud_adr = 0;
    blank -> bit = total_free_space;

    //assign value as first head node
    lst_ary[power + 1].head -> next = blank;

    //start reading in
    while(fscanf(file, "%d %s %d", &reqSeq, &reqType, &reqSize) != EOF)
    {

        //check for alloc
        if(strcmp(reqType, "alloc") == 0 )
        {
            //create request
            req_arrayb[reqSeq].is_req = TRUE;
            req_arrayb[reqSeq].size = reqSize;

            allocateMemoryb(&req_arrayb[reqSeq]);

            //reset free values/chunk sizes for this req
            req_arrayb[reqSeq].elements_on_free_list = 0;
            req_arrayb[reqSeq].largest_chunk = 0;

            for(free = list_head.next; free; free = free -> next)
            {
                //update free sizes
                ++req_arrayb[reqSeq].elements_on_free_list;

                //update largest chunk
                if(free -> block_size > req_arrayb[reqSeq].largest_chunk)
                {
                    req_arrayb[reqSeq].largest_chunk = free -> block_size;
                }
            }

        }
        else
        {
            //not alloc, is free
            req_arrayb[reqSeq].size = req_arrayb[reqSize].size;
            req_arrayb[reqSeq].is_allocated = req_arrayb[reqSize].is_allocated;

            update_listc(reqSize);

            //update complete, reset values again
            req_arrayb[reqSeq].memory_left = total_free;
            req_arrayb[reqSeq].elements_on_free_list = 0;
            req_arrayb[reqSeq].largest_chunk = 0;

            //reupdate values
            for(free = list_head.next; free; free = free -> next)
            {
                //update free sizes
                req_arrayb[reqSeq].elements_on_free_list++;

                //update largest chunk
                if(free -> block_size > req_arrayb[reqSeq].largest_chunk)
                {
                    req_arrayb[reqSeq].largest_chunk = free -> block_size;
                }
            }
        }
    }

    //print results
    printf("POLICY: Buddy\tMEMORY SIZE: %d\n\n", mem);
    printf("\nNUMBER \tSEQ \tSIZE \tADR \tMEM LEFT \tLARGEST\n");

    int start = 1;
    int fails = 0;
    char function[6];

    for(start = 1; start < NUMBER_ENTRIES; start++)
    {

        if(req_array[start].is_allocated == FALSE)
        {
            //not allocated, invalidate the address
            req_array[start].base_adr = -1;
            //increment fails
            fails++;
        }

        //determine operation
        if(req_array[start].is_req == 1)
        {
            sprintf(function, "%s", "alloc");
        }
        else
        {
            sprintf(function, "%s", "free");
        }

        //print info
        printf("\t%d \t%s \t\t%d \t%d \t\t%d \t\t%d\n", i, function, req_array[start].size,
               req_array[start].base_adr, req_array[start].memory_left, req_array[start].largest_chunk);

    }

    //print failed allocations
    printf("%d Allocations Failed", fails);



    //close input file
    fclose(file);

}
