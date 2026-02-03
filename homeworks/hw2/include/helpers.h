#ifndef HELPERS_H
#define HELPERS_H
#include <stdbool.h>
#include <errno.h>
#include "icsmm.h"

#define MAX_HEAP_SIZE 16384

/* Helper function declarations go here */
//GLOBAL VARIABLES TO KEEP TRACK OF HEAP / BLOCKS / PTRS
static bool heapInitialized;
static void* heapHead;
static void* heapTail;

//GLOBAL VARIABlES ACCESS
bool check_heapInitialized(void);
void* get_heapHead(void);
void* get_heapTail(void);

//HEAP METHODS
bool setup_heap(void);

//FREE LIST METHODS




#endif
