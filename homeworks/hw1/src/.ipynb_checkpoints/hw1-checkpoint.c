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
    //DEFINED STRUCT IN MIPSfields
    //bits 31-26
    f->opcode = (instruction >> 26) & 0x3F;
    //bits 25-21
    f->rs = (instruction >> 21) & 0x1F;
    //bits 20-16
    f->rt = (instruction >> 16) & 0x1F;
    //bits 15-11
    f->rd = (instruction >> 11) & 0x1F;
    //bits 10-6
    f->shamt = (instruction >> 6) & 0x1F;
    //bits 5-0
    f->func = instruction & 0x3F;
    //immediate16 (15-0)
    f->immediate16 = instruction & 0xFFFF;
    //immediate26 (25-0)
    f->immediate26 = instruction & 0x3FFFFFF;

    if (f->opcode == 0){
        f->uid = f->func;
    } else {
        f->uid = f->opcode << 26;
    }
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

