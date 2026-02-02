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
    if (size == 0){  //Checks whether or not size is valid / greater than 4 pages
        errno = EINVAL;
        return NULL; 
    }
    if (size > MAX_HEAP_SIZE){ //Checks whether or not requested size is greater than maximum payload. 18384 (4096 * 4)
        errno = ENOMEM;
        return NULL;
    }
    

    
    return NULL;
}

void *ics_realloc(void *ptr, size_t size) { 
    return NULL; 
}

int ics_free(void *ptr) { 
    return -1; 
    }
