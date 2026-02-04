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

    //STEP 1: INITIALIZE THE HEAP & BLOCK
    if (!check_heapInitialized()){
        if (!setup_heap()){
            return NULL;
        }
    }

    //STEP 2: CALCULATE BLOCK
    size_t total_block = total_blockSize(size);
    size_t payload = total_block - HEADER_SIZE - FOOTER_SIZE;
    size_t padding = total_padding(size, payload);

    //STEP 3: FIND BEST FIT
    ics_free_header* bestFit = find_bestfit(total_block);

    if (bestFit == NULL){
        bestFit = extend_heap(total_block);
    
        if (bestFit == NULL) {
            return NULL;  // errno already set to ENOMEM
        }
    }

    //STEP 4: INSERT BLOCK INTO BEST FIT
    insert_block(bestFit);

    //STEP 5: REMOVE BLOCK FROM FREE LIST
    remove_block(bestFit);

    //STEP 6: SETUP BLOCK WITH INFORMATION
    bestFit->header.block_size = total_block | 1;  // Set allocated bit
    bestFit->header.hid = HEADER_MAGIC;
    bestFit->header.padding_amount = padding;
    
    ics_footer* footer = get_blockFooter(&bestFit->header);
    footer->block_size = total_block | 1;  // Set allocated bit
    footer->fid = FOOTER_MAGIC;

    return (void*)((char*)bestFit + HEADER_SIZE);
}

void *ics_realloc(void *ptr, size_t size) { 
    return NULL; 
}

int ics_free(void *ptr) { 
    return -1; 
}
