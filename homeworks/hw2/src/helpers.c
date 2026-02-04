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
    return block->block_size & ~3;
}
ics_footer* get_blockFooter(ics_header* header){
    size_t blockSize = get_blockSize(header);
    return (ics_footer*)((char*)header + blockSize - FOOTER_SIZE);
}
ics_header* get_blockHeader(ics_footer* footer){
    size_t block_size = footer->block_size & ~3; 
    return (ics_header*)((char*)footer - block_size + FOOTER_SIZE);
}

//  HEAP DEFINITONS  //
ics_free_header* extend_heap(size_t size){
    void* blockCurr = ics_get_brk(); //GET CURRENT BLOCK STATUS AMOUNT 
    size_t total_heapSize = (size_t)((char*)blockCurr - (char*)heapHead);

    if (total_heapSize >= MAX_HEAP_SIZE){ //ITS AT MAX.
        errno = ENOMEM;
        return NULL;
    }

    void* newBlock = ics_inc_brk();
    if (newBlock == (void*)-1){
        errno = ENOMEM;
        return NULL;
    }

    ics_free_header* blockNew = heapTail;

    heapTail = (char*)blockCurr - FOOTER_SIZE;

    size_t block_size = 4096;

    blockNew->header.block_size = (char*)heapTail - (char*)blockNew;
    blockNew->header.hid = HEADER_MAGIC;
    blockNew->header.padding_amount = 0;
    blockNew->next = NULL;
    blockNew->prev = NULL;

    ics_footer* prologue = get_blockFooter(&blockNew->header);
    prologue->block_size = (char*)heapTail - (char*)blockNew;
    prologue->fid = FOOTER_MAGIC;

    ics_header* epilogue = (ics_header*) heapTail;
    epilogue->block_size = 0 | 1;
    epilogue->hid = HEADER_MAGIC;
    epilogue->padding_amount = 0;

    return blockNew;
}

ics_free_header* coalesce_block(ics_free_header* block) {
    ics_header* header = &block->header;
    size_t blockSize = get_blockSize(header);
    
    // Check if previous block is free
    bool prev_allocated = true;
    ics_header* prev_block = NULL;
    if (header != (ics_header*)((char*)heapHead + 8)) {  // Not first block after prologue
        blockPrev = get_prev_block(header);
        blockAlloc = is_allocated(blockPrev);
    }
    
    // Check if next block is free
    bool next_allocated = true;
    ics_header* blockNext = get_next_block(header);
    if (blockNext != (ics_header*)heapTail) {  // Not epilogue
        next_allocated = is_allocated(blockNext);
    }
    
    // Case 1: Both neighbors allocated - just return current block
    if (prev_allocated && next_allocated) {
        return block;
    }
    
    // Case 2: Previous allocated, next free
    if (prev_allocated && !next_allocated) {
        // Remove next block from free list
        remove_from_freelist((ics_free_header*)blockNext);
        
        // Merge with next
        size_t next_size = get_block_size(blockNext);
        blockSize += next_size;
        
        // Update current block
        header->block_size = blockSize;
        ics_footer* footer = get_footer(header);
        footer->block_size = blockSize;
        footer->fid = FOOTER_MAGIC;
        
        return block;
    }
    
    // Case 3: Previous free, next allocated
    if (!prev_allocated && next_allocated) {
        // Remove previous block from free list
        remove_from_freelist((ics_free_header*)blockPrev);
        
        // Merge with previous
        size_t prev_size = get_block_size(blockPrev);
        blockSize += prev_size;
        
        // Update previous block (which becomes the merged block)
        prev_block->block_size = blockSize;
        ics_footer* footer = get_footer(prev_block);
        footer->block_size = blockSize;
        footer->fid = FOOTER_MAGIC;
        
        return (ics_free_header*)prev_block;
    }
    
    // Case 4: Both neighbors free
    if (!prev_allocated && !next_allocated) {
        // Remove both from free list
        remove_from_freelist((ics_free_header*)blockPrev);
        remove_from_freelist((ics_free_header*)blockNext);
        
        // Merge all three
        size_t prev_size = get_block_size(blockPrev);
        size_t next_size = get_block_size(blockNext);
        blockSize += prev_size + next_size;
        
        // Update previous block (which becomes the merged block)
        prev_block->block_size = blockSize;
        ics_footer* footer = get_footer(blockPrev);
        footer->block_size = blockSize;
        footer->fid = FOOTER_MAGIC;
        
        return (ics_free_header*)blockPrev;
    }
    return block;
}


bool setup_heap(void){ //Do i need to call ics_mem_init()? || Entire Method only runs once
    void* block = ics_inc_brk();
    if (block == (void*) -1){
        errno = ENOMEM;
        return false;
    }

    heapHead = block; 
    void* blockCurr = ics_get_brk(); //Gets address of starting page.

    ics_footer* prologue = heapHead;
    prologue->block_size = 0 | 1;
    prologue->fid = FOOTER_MAGIC;

    ics_free_header* newblock = (ics_free_header*)((char*)heapHead + FOOTER_SIZE);

    size_t newblockSize = 4096 - HEADER_SIZE - FOOTER_SIZE;
    newblock->header.block_size = newblockSize;
    newblock->header.hid = HEADER_MAGIC;
    newblock->header.padding_amount = 0;
    newblock->next = NULL;
    newblock->prev = NULL;

    ics_footer* footer = get_blockFooter(&newblock->header);
    footer->block_size = newblockSize;
    footer->fid = FOOTER_MAGIC;

    heapTail = blockCurr - HEADER_SIZE; //Block Start After Prologue
    ics_header* epilogue = heapTail;
    epilogue->block_size = 0 | 1;
    epilogue->hid = HEADER_MAGIC;
    epilogue->padding_amount = 0;

    freelist_head = newblock;

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
size_t total_padding(size_t malloc_size, size_t payload){
    return (payload - malloc_size);
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
void remove_block(ics_free_header* block){
    //Check if HEAD
    if (block->prev != NULL){
        block->prev->next = block->next;
    } else {
        freelist_head = block->next;
    }

    if (block->next != NULL){
        block->next->prev = block->prev;
    }

    block->next = NULL;
    block->prev = NULL;
}
bool split_block(ics_header* block, size_t total_size){
    size_t blockSize = get_blockSize(block);
    size_t remaining = blockSize - total_size;

    if (remaining < 32){
        return false;
    }

    block->block_size = total_size;
    ics_footer* block_footer = get_blockFooter(block);
    block_footer->block_size = total_size;
    block_footer->fid = FOOTER_MAGIC;

    ics_free_header* newBlock = (ics_free_header*)((char*)block + total_size);
    newBlock->header.block_size = remaining;
    newBlock->header.hid = HEADER_MAGIC;
    newBlock->header.padding_amount = 0;
    
    ics_footer* newFooter = get_blockFooter(&newBlock->header);
    newFooter->block_size = remaining;
    newFooter->fid = FOOTER_MAGIC;
    
    // Add new block to free list
    insert_block(newBlock);
    
    return true;
}
ics_free_header* find_bestfit(size_t size){
    ics_free_header* bestfit = NULL;
    size_t blockSize;
    ics_free_header* current = freelist_head;

    while (current != NULL){
        blockSize = get_blockSize(&current->header);
         if (blockSize >= size) { //if block can fit into the block 
            if (bestfit == NULL || blockSize < get_blockSize(&bestfit->header)) { // if the current size is LT, might be a closer fit.
                bestfit = current;
            }
        }
        current = current->next;
    }
    return bestfit;
}

