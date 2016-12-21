//Eric Wang
//eric_wang1@student.uml.edu
//code taken from the assign 5 data structs file.


#include "main.h"

int allocateMemorya(struct request* request)
{
    //create two free lists
    struct free_list* free = NULL;
    struct free_list* valid = NULL;

    int sizeDiff = 0;
    int leftover = 0;

    //first loop prep
    int flag = TRUE;

    //find smallest block that fits
    for(free = list_head.next; free; free = free -> next)
    {
        if(request -> size <= free -> block_size)
        {
            sizeDiff = free -> block_size - request -> size;

            //do for first instance found
            if(flag)
            {
                leftover = sizeDiff;
                valid = free;
                flag = FALSE;
            }

            //check for even smaller chunk
            if (leftover > sizeDiff)
            {
                leftover = sizeDiff;
                valid = free;
            }

        }
    }

    //if valid spot is found
    if(valid != NULL)
    {
        //put info in
        request -> is_allocated = TRUE;
        request -> base_adr = valid -> block_adr;
        request -> next_boundary_adr = request -> base_adr + request -> size;

        //update space
        total_free = total_free - request -> size;
        request -> memory_left = total_free;

        //handle perfect match
        if((valid -> block_size = valid -> block_size - request -> size) == 0)
        {
            //skip the valid block, shift list
            valid -> previous -> next = valid -> next;
            valid -> next -> previous = valid -> previous;

            return 0;
        }

        //update  smaller block
        valid -> block_adr = valid -> block_adr + request -> size;
        return 0;

    }

    //spot was not found
    request -> memory_left = total_free;
    return 0;

}

int update_listb(int index)
{

    //temp free lists declarations
    struct free_list* free;
    struct free_list* newB;
    struct free_list* combine;

    //not allocated, no need to modify
    if(req_array[index].is_allocated == FALSE)
    {
        return 0;
    }

    //block needs to be free/marked done
    req_array[index].is_allocated = DONE;
    total_free += req_array[index].size;

    //scan the whole list for possible combinations
    for(free = list_head.next; free; free = free -> next)
    {

        //free newB does not pair up with checked newB
        if(req_array[index].base_adr > free -> block_adr)
        {
            continue;
        }

        //create a new block to store
        newB = malloc(sizeof(struct free_list));
        newB -> block_size = req_array[index].size;
        newB -> block_adr = req_array[index].base_adr;
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
    return 0;
}


int best(int mem, char* fileIn)
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
        req_array[i].is_req = FALSE;
        req_array[i].is_allocated = FALSE;
    }

    //top node setup.  issue
    //top is a pointer, care
    top = malloc(sizeof(struct free_list));
    top -> next = NULL;
    top -> previous = &list_head;
    top -> block_size = total_free_space;
    top -> block_adr = 0;
    top -> adjacent_adr = total_free_space;

    //set the list
    list_head.next = top;
    list_head.previous = NULL;
    list_head.block_size = -1;
    list_head.block_adr = -1;
    list_head.adjacent_adr = 0;

    //start reading in
    while(fscanf(file, "%d %s %d", &reqSeq, &reqType, &reqSize) != EOF)
    {

        //check for alloc
        if(strcmp(reqType, "alloc") == 0 )
        {
            //create request
            req_array[reqSeq].is_req = TRUE;
            req_array[reqSeq].size = reqSize;

            allocateMemorya(&req_array[reqSeq]);

            //reset free values/chunk sizes for this req
            req_array[reqSeq].elements_on_free_list = 0;
            req_array[reqSeq].largest_chunk = 0;

            for(free = list_head.next; free; free = free -> next)
            {
                //update free sizes
                req_array[reqSeq].elements_on_free_list++;

                //update largest chunk
                if(free -> block_size > req_array[reqSeq].largest_chunk)
                {
                    req_array[reqSeq].largest_chunk = free -> block_size;
                }
            }

        }
        else
        {
            //not alloc, is free
            req_array[reqSeq].size = req_array[reqSize].size;
            req_array[reqSeq].is_allocated = req_array[reqSize].is_allocated;

            update_listb(reqSize);

            //update complete, reset values again
            req_array[reqSeq].memory_left = total_free;
            req_array[reqSeq].elements_on_free_list = 0;
            req_array[reqSeq].largest_chunk = 0;

            //reupdate values
            for(free = list_head.next; free; free = free -> next)
            {
                //update free sizes
                ++req_array[reqSeq].elements_on_free_list;

                //update largest chunk
                if(free -> block_size > req_array[reqSeq].largest_chunk)
                {
                    req_array[reqSeq].largest_chunk = free -> block_size;
                }
            }

        }

    }

    //print results
    printf("POLICY: Best\tMEMORY SIZE: %d\n\n", mem);
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
            //increment fails  THIS DOUBLECOUNTS FAILED FREES AS WELL
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
        printf("\t%d \t%s \t\t%d \t%d \t\t%d \t\t%d\n", start, function, req_array[start].size,
               req_array[start].base_adr, req_array[start].memory_left, req_array[start].largest_chunk);

    }

    //print failed allocations
    printf("%d Allocations Failed", fails / 2);



    //close input file
    fclose(file);

return 0;
}
