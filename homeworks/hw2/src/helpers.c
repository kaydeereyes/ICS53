#include "helpers.h"

//defined MAX_HEAP_SIZE 16384

/* Helper function definitions go here */
static bool heapInitialized = false;
static void* heapHead = NULL;
static void* heapTail = NULL;

//  GLOBAL ACCESS METHODS //

bool check_heapInitialized(void){ //T/F
    return heapInitialized;
}
void* get_heapHead(void){ //returns prologue address/head address
    return heapHead;
}
void* get_heapTail(void){ //returns epilogue/tail address
    return heapTail;
}

//  BLOCK ACCESS DEFINITIONS    //
size_t get_blockSize(ics_header* block){
    return block->block_size & ~1;
}


//  HEAP DEFINITONS  //
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

// PAYLOAD / BLOCK CALCULATIONS //
size_t total_blockSize(size_t malloc_size){
    size_t payload = (malloc_size < 16) ? 16 : malloc_size; //IF <16 = 16; ELSE MALLOC SIZE

    if (payload % 16 != 0){
        payload = ((payload / 16) + 1) * 16;
    }
    size_t total_size = HEADER_SIZE + payload + FOOTER_SIZE;
    return total_size;
}

//  FREE LIST METHODS   //
void insert_block(ics_free_header* block){
    size_t blockSize = get_blockSize(&block->header);

    //LIST EMPTY
    if (freelist_head == NULL){
        freelist_head = block;
        block->next = NULL;
        block->prev = NULL;
        return;
    }

    //INSERT AT HEAD
    size_t headSize = get_blockSize(&freelist_head->header);
    if (blockSize >= headSize){
        block->next = freelist_head;
        block->prev = NULL;
        freelist_head->prev = block;
        freelist_head = block; //SETS BLOCK AS THE NEW HEAD
        return;
    }

    //IF NONE ^ ITERATES THROUGH FREE_LIST TO FIND THE NEXT FREE BLOCK
    ics_free_header* current = freelist_head;
    while (current != NULL){
        size_t current_blockSize = get_blockSize(&current->header);

        if (blockSize >= current_blockSize){
            block->next = current;
            block->prev = current->prev;

            if (current->prev != NULL){
                current->prev->next = block;
            } else {
                freelist_head = block;
            }
            current->prev = block;
            return;
        }
        if (current->next == NULL){
            current->next = block;
            block->prev = current;
            block->next = NULL;
        }
        current = current->next;
    }
}