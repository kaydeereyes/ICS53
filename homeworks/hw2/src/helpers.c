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
bool is_allocated(ics_header* header){
    return (header->block_size & 1) == 1;
}
bool valid_ptr(void* ptr){
    if (ptr == NULL) return false;

    ics_header* header = (ics_header*)((char*)ptr - HEADER_SIZE);
    if (!check_heap(header)) return false;
    if (!is_allocated(header)) return false;
    if (header->hid != HEADER_MAGIC) return false;

    ics_footer* footer = get_blockFooter(header);
    if (footer->fid != FOOTER_MAGIC) return false;

    if ((header->block_size & ~1) != (footer->block_size & ~1)) return false;
    if ((header->block_size & 1) != (footer->block_size & 1)) return false;
    
    return true;
}
void set_allocationBit(ics_header* header, ics_footer* footer, bool allocated){
    if (allocated){ //SETS BIT
        header->block_size |= 1;
        footer->block_size |= 1;
    } else {
        header->block_size &= ~1;
        footer->block_size &= ~1;
    }
}
ics_footer* get_blockFooter(ics_header* header){
    size_t blockSize = get_blockSize(header);
    return (ics_footer*)((char*)header + blockSize - FOOTER_SIZE);
}
ics_header* get_blockHeader(ics_footer* footer){
    size_t block_size = footer->block_size & ~3; 
    return (ics_header*)((char*)footer - block_size + FOOTER_SIZE);
}
ics_header* get_blockPrev(ics_header* header){
    ics_footer* footerPrev = (ics_footer*)((char*)header - FOOTER_SIZE);
    size_t prev_blockSize = footerPrev->block_size & ~3;
    return (ics_header*)((char*)header - prev_blockSize);

}
ics_header* get_blockNext(ics_header* header){
    size_t blockSize = get_blockSize(header);
    return (ics_header*)((char*)header + blockSize);

}
//  HEAP DEFINITONS  //
bool check_heap(void* block){
    return (block >= heapHead && block < heapTail);
}
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

    ics_free_header* blockNew = (ics_free_header*)heapTail;
    heapTail = ics_get_brk() - HEADER_SIZE;
    size_t blockSizeNew = (char*)heapTail - (char*)blockNew;


    //size_t block_size = 4096;

    blockNew->header.block_size = blockSizeNew;
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

    bool prev_allocated = true;
    bool next_allocated = true;

    ics_header* blockPrev = NULL;   
    ics_header* blockNext = NULL;
    
    // Check if previous block is free
    if ((char*)header > (char*)heapHead + HEADER_SIZE) {
        ics_footer* prev_footer = (ics_footer*)((char*)header - FOOTER_SIZE);
        if (prev_footer->fid == FOOTER_MAGIC) {
            blockPrev = get_blockHeader(prev_footer);
            prev_allocated = is_allocated(blockPrev);
        }
    }
    
    // Check if next block is free
    blockNext = get_blockNext(header);
    if (blockNext != (ics_header*)heapTail){  // Not epilogue
        next_allocated = is_allocated(blockNext);
        } else {
        next_allocated = true; 
    }
    

    // Case 1: Both neighbors allocated - just return current block
    if (prev_allocated && next_allocated) {
        return block;
    }
    
    // Case 2: Previous allocated, next free
    if (prev_allocated && !next_allocated) {
        //printf("Press Enter to Continue");
        // Remove next block from free list
        ics_free_header* nextBlock = (ics_free_header*)blockNext;
        size_t next_size = get_blockSize(blockNext);
        remove_block((ics_free_header*)nextBlock);
        
        // Merge with next
        blockSize += next_size;
        
        // Update current block
        header->block_size = blockSize;
        header->hid = HEADER_MAGIC;  
        ics_footer* footer = get_blockFooter(header);
        footer->block_size = blockSize;
        footer->fid = FOOTER_MAGIC;

    }

    if ((char*)header > (char*)heapHead + FOOTER_SIZE) {
        ics_footer* prev_footer = (ics_footer*)((char*)header - FOOTER_SIZE);
        
        if (prev_footer->fid == FOOTER_MAGIC && !(prev_footer->block_size & 1)) {
            size_t prev_size = prev_footer->block_size & ~3;
            ics_header* prev_header = (ics_header*)((char*)header - prev_size);
            
            if ((void*)prev_header >= heapHead && 
                prev_header->hid == HEADER_MAGIC &&
                !is_allocated(prev_header)) {
                
                // Previous block is free - extend it forward to include current block
                ics_free_header* prevBlock = (ics_free_header*)prev_header;
                remove_block(prevBlock);
                
                // Update previous block to include current block
                size_t total_size = prev_size + blockSize;
                prev_header->block_size = total_size;
                
                ics_footer* footer = get_blockFooter(prev_header);
                footer->block_size = total_size;
                footer->fid = FOOTER_MAGIC;
                
                prevBlock->next = NULL;
                prevBlock->prev = NULL;
                
                return prevBlock;
            }
        }
    }
    
    block->next = NULL;
    block->prev = NULL;
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
            return;
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

    if (blockSize - total_size < 32) return false;

    block->block_size = total_size | 1;
    ics_footer* block_footer = get_blockFooter(block);
    block_footer->block_size = total_size | 1;
    block_footer->fid = FOOTER_MAGIC;

    ics_free_header* newBlock = (ics_free_header*)((char*)block + total_size);
    newBlock->header.block_size = blockSize - total_size;
    newBlock->header.hid = HEADER_MAGIC;
    newBlock->header.padding_amount = 0;

    ics_footer* newFooter = get_blockFooter(&newBlock->header);
    newFooter->block_size = blockSize - total_size;
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

