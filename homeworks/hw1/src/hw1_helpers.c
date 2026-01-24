// Define any helper functions here

#include <stdint.h>

//HELPER FUNCTION TO SKIP WHITESPACE (HELPS TO PARSE THROUGH LINE)
char* skipWhitespace(char* p){
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'){
        p++;
    }
    return p;
}

char* parseHex(char* p, uint32_t* result, int* digitsRead){
    *result = 0;
    *digitsRead = 0; //our validation check, if 0, will be detected later.

    while ((*p >= '0' && *p <= '9') || //checks if 0-9
           (*p >= 'a' && *p <= 'f') || //checks if a-f
           (*p >= 'A' && *p <= 'F')) { //checks if 
        int digit;
        if (*p >= '0' && *p <= '9') {
            digit = *p - '0';
        } else if (*p >= 'a' && *p <= 'f') {
            digit = *p - 'a' + 10;
        } else {
            digit = *p - 'A' + 10;
        }
        *result = *result * 16 + digit;
        (*digitsRead)++;
        p++;
    }
    return p;
}    