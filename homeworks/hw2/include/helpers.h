#ifndef HELPERS_H
#define HELPERS_H
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include "icsmm.h"

#define MAX_HEAP_SIZE 16384
#define HEADER_SIZE sizeof(ics_header)
#define FOOTER_SIZE sizeof(ics_footer)


/* Helper function declarations go here */
//GLOBAL VARIABLES TO KEEP TRACK OF HEAP / BLOCKS / PTRS
static bool heapInitialized;
static void* heapHead;
static void* heapTail;

//GLOBAL VARIABlES ACCESS
bool check_heapInitialized(void);
void* get_heapHead(void);
void* get_heapTail(void);

//  BLOCK ACCESS //
size_t get_blockSize(ics_header* block);

bool is_allocated(ics_header* header);
bool valid_ptr(void* header);
void set_allocationBit(ics_header* header, ics_footer* footer, bool allocated);
ics_footer* get_blockFooter(ics_header* header);
ics_header* get_blockHeader(ics_footer* footer);
ics_header* get_blockPrev(ics_header* header);
ics_header* get_blockNext(ics_header* header);

// HEAP METHODS //
bool check_heap(void* block);
ics_free_header* extend_heap(size_t size);
ics_free_header* coalesce_block(ics_free_header* block);
bool setup_heap(void);

//  PAYLOAD / BLOCK CALCULATIONS    //
size_t total_blockSize(size_t malloc_size);
size_t total_padding(size_t malloc_size, size_t payload);

//  FREE LIST METHODS //
void insert_block(ics_free_header* block);
void remove_block(ics_free_header* block);
bool split_block(ics_header* block, size_t total_size);
ics_free_header* find_bestfit(size_t size);



#endif
