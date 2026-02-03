#include "helpers.h"

//defined MAX_HEAP_SIZE 16384

/* Helper function definitions go here */
static bool heapInitialized = false;
static void* heapHead = NULL;
static void* heapTail = NULL;

bool check_heapInitialized(void){ //T/F
    return heapInitialized;
}
void* get_heapHead(void){ //returns prologue address/head address
    return heapHead;
}
void* get_heapTail(void){ //returns epilogue/tail address
    return heapTail;
}

bool setup_heap(void){ //Do i need to call ics_mem_init()? || Entire Method only runs once
    void* block = ics_inc_brk();
    if (block == -1){
        errno = ENOMEM;
        return false;
    }

    heapHead = block; 
    void* blockCurr = ics_get_brk(); //Gets address of starting page.

    ics_footer* prologue = heapHead;
    prologue->block_size = 0 | 1;
    prologue->fid = FOOTER_MAGIC;

    heapTail = blockCurr - 8; //Block Start After Prologue
    ics_header* epilogue = heapTail;
    epilogue->block_size = 0 | 1;
    epilogue->hid = HEADER_MAGIC;
    epilogue->padding_amount = 0;

    heapInitialized = true; //Updates the heap to be initialized
    return true;
}