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
    if (size > MAX_HEAP_SIZE - HEADER_SIZE - FOOTER_SIZE){ //Checks if size is greater than maximum payload. 18384 (4096 * 4)
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
        bestFit = coalesce_block(bestFit);
        //STEP 4: INSERT BLOCK INTO BEST FIT
        insert_block(bestFit);  

        //bestFit = find_bestfit(total_block);
    }

    //STEP 5: REMOVE BLOCK FROM FREE LIST
    remove_block(bestFit);

    //STEP 7: SPLIT BLOCK IF THERES EXCESS
    size_t bestFitSize = get_blockSize(&bestFit->header);
    size_t remainder = bestFitSize - total_block;
    
    if (remainder >= 32) {
        size_t flags = 1;       // bit 0 = allocated
        if (padding > 0) {
            flags |= 2;         // bit 1 = has padding
        }
        // SPLIT: Use first part, create free block from remainder
        
        // Set up allocated block
        bestFit->header.block_size = total_block | flags;
        bestFit->header.hid = HEADER_MAGIC;
        bestFit->header.padding_amount = padding;
        
        ics_footer* allocFooter = get_blockFooter(&bestFit->header);
        allocFooter->block_size = total_block | flags;
        allocFooter->fid = FOOTER_MAGIC;
        
        // Create remainder free block
        ics_free_header* remainderBlock = (ics_free_header*)((char*)bestFit + total_block);
        
        remainderBlock->header.block_size = remainder;
        remainderBlock->header.hid = HEADER_MAGIC;
        remainderBlock->header.padding_amount = 0;
        remainderBlock->next = NULL;
        remainderBlock->prev = NULL;
        
        ics_footer* remainderFooter = get_blockFooter(&remainderBlock->header);
        remainderFooter->block_size = remainder;
        remainderFooter->fid = FOOTER_MAGIC;
        
        // Add remainder to free list
        insert_block(remainderBlock);
        
    } else {
        // DON'T SPLIT: Use entire block
        size_t actual_payload = bestFitSize - HEADER_SIZE - FOOTER_SIZE;
        size_t actual_padding = actual_payload - size;
        
        size_t flags = 1;       // bit 0 = allocated
        if (actual_padding > 0) {
            flags |= 2;         // bit 1 = has padding
        }

        bestFit->header.block_size = bestFitSize | flags;
        bestFit->header.hid = HEADER_MAGIC;
        bestFit->header.padding_amount = actual_padding;
        
        ics_footer* footer = get_blockFooter(&bestFit->header);
        footer->block_size = bestFitSize | flags;
        footer->fid = FOOTER_MAGIC;
    }

    return (void*)((char*)bestFit + HEADER_SIZE);
}

void *ics_realloc(void *ptr, size_t size) { 
    if (ptr == NULL){
        return ics_malloc(size);
    }

    if (size == 0){
        if (!valid_ptr(ptr)) {
        errno = EINVAL;
        return NULL;
        }

        ics_header* header = (ics_header*)((char*)ptr - HEADER_SIZE);
        ics_free_header* block = (ics_free_header*)header;

        set_allocationBit(header, get_blockFooter(header), false);

        block->next = NULL;
        block->prev = NULL;

        // DO NOT coalesce
        insert_block(block);

        return NULL;
    }

    if (!valid_ptr(ptr)){
        errno = EINVAL;
        return NULL;
    }
    ics_header* header = (ics_header*)((char*)ptr - HEADER_SIZE);
    size_t curr_blockSize = get_blockSize(header);
    size_t curr_payloadSize = curr_blockSize - HEADER_SIZE - FOOTER_SIZE;

    size_t new_blockSize = total_blockSize(size);

    if (new_blockSize > curr_blockSize) {
        // Allocate new larger block
        void* new_ptr = ics_malloc(size);
        if (new_ptr == NULL) {
            return NULL;  // errno already set
        }
        size_t copy_size = (curr_payloadSize < size) ? curr_payloadSize : size;
        char* src = (char*)ptr;
        char* dst = (char*)new_ptr;
        for (size_t i = 0; i < copy_size; i++) {
            dst[i] = src[i];
        }
        ics_free(ptr);
        return new_ptr;
    }

    size_t freed_space = curr_blockSize - new_blockSize;
    
    if (freed_space < 32) {
        // Can't split, just return same pointer
        return ptr;
    }
    
    return NULL; 
}

int ics_free(void *ptr) { 
    if (!valid_ptr(ptr)){
        errno = EINVAL;
        return -1;
    }

    ics_free_header* block = (ics_free_header*)((char*)ptr - HEADER_SIZE);

    set_allocationBit(&block->header, get_blockFooter(&block->header), false);

    block->next = NULL;
    block->prev = NULL;

    block = coalesce_block(block);
    
    insert_block(block);

    return 0; 
}
