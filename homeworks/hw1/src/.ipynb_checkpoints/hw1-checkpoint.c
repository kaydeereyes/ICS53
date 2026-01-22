#include "linkedlist.h"
#include "hw1_helpers.h"
#include "hw1.h"

// Part 1 Functions
int getSubstrings(char *str,  char delim, char ** array, int maxSize) {
    ///Modifies a set of strings into a set of substrings. Returns number of substrings.

    if (str == NULL || array == NULL || maxSize < 1)
        return -1;

    if (*str == '\0' || delim == '\0')
        return 0;
    
    char *start = str; //starting char of the substring.
    char **point = array; //current array slot
    int count = 0; //tracks number of substrings

    for (char *p = str; *p != '\0'; p++){ //iterates through the str. [s][t][r]
        if (*p == delim){ //*p dereferences the addres its pointing to to becomevalue.
            *p = '\0'; //terminates delim, makes it NULL
            *point = start; //stores pointer to start of the substring
            point++; //moves on in array 
            count++; //increases count
            
            if (count == maxSize)
                return count; // if array capacity is reached

            start = p + 1; //next substring after the delim 
        }
        
    }

    if (count < maxSize){
        *point = start;
        count++;
    }

    return count;
}

void parseMIPSfields(const uint32_t instruction, MIPSfields* f) {

}

MIPSinstr* loadInstrFormat(char* line) {

    return (MIPSinstr*) 0xDEADBEEF;
}

// Part 2 Functions
int MIPSinstr_uidComparator(const void* s1, const void* s2) {

    return 0xDEADBEEF;
}

void MIPSinstr_Printer(void* data, void* fp) {

}

void MIPSinstr_Deleter(void* data) {

}

node_t* FindInList(list_t* list, void* token) {

    return (node_t*) 0xDEADBEEF;
}

void DestroyList(list_t** list)  {

}


// Part 3 Functions
list_t* createMIPSinstrList(FILE* IMAPFILE) {

    return (list_t*) 0xDEADBEEF;
}

int printInstr(MIPSfields* instr, list_t* MIPSinstrList, char** regNames, FILE* OUTFILE) {

    return 0xDEADBEEF;
}

