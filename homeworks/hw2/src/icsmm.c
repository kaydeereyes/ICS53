/**
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 * If you want to make helper functions, put them in helpers.c
 */
#include "icsmm.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

ics_free_header *freelist_head = NULL; // pointer to the head

void *ics_malloc(size_t size) {
    //STEP 0: CHECK SIZE & IF VALID
    if (size == 0){  //Checks if size is valid, should it see if number is negative?
        errno = EINVAL;
        return NULL; 
    }
    if (size > MAX_HEAP_SIZE){ //Checks if size is greater than maximum payload. 18384 (4096 * 4)
        errno = ENOMEM;
        return NULL;
    }

    if (!check_heapInitialized){
        if (!setup_heap){
            return NULL;
        }
    }
    

    
    return NULL;
}

void *ics_realloc(void *ptr, size_t size) { 
    return NULL; 
}

int ics_free(void *ptr) { 
    return -1; 
    }
