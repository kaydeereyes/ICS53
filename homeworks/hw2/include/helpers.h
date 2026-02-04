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

ics_footer* get_blockFooter(ics_header* header);
ics_header* get_blockHeader(ics_footer* footer);

// HEAP METHODS //
ics_free_header* extend_heap(size_t size);
bool setup_heap(void);

//  PAYLOAD / BLOCK CALCULATIONS    //
size_t total_blockSize(size_t malloc_size);
size_t total_padding(size_t malloc_size, size_t payload);

//  FREE LIST METHODS //
void insert_block(ics_free_header* block);
void remove_block(ics_free_header* block);
ics_free_header* find_bestfit(size_t size);



#endif
