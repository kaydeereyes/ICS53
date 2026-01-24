#ifndef HELPERS_1_H
#define HELPERS_1_H

#include <stdint.h>

// Declare any helper functions here
char* skipWhitespace(char* p);
char* parseHex(char* p, uint32_t* result, int* digitsRead);

#endif
