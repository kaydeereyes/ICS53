#ifndef HELPERS_H
#define HELPERS_H
#include <stdbool.h>
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

// HEAP METHODS //
bool setup_heap(void);

//  PAYLOAD / BLOCK CALCULATIONS    //
size_t total_blockSize(size_t malloc_size);

//  FREE LIST METHODS //
void insert_block(ics_free_header* block);



#endif
