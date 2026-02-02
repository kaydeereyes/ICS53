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
void* get_heapTail(void)[ //returns epilogue/tail address
    return heapTail;
}



