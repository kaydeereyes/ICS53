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
    if (!line) return NULL;

    char* end = line;
    while (*end != '\0') end++;
    if (end != line && *(end - 1) == '\n') {
        *(end - 1) = '\0';
    }

    MIPSinstr* instr = malloc(sizeof(MIPSinstr));
    if (instr == NULL){
        return NULL;
    }

    //INITIALIZES
    instr -> type = '\0'; //Essentially like self.value = '\0' in python
    instr -> uid = 0;
    instr -> pretty = 0;
    instr -> mnemonic = NULL; //mnemonic is type char
    instr -> usagecnt = 0;

    //ALLOCATES TOKENS
    int maxTokens = 4;
    char** tokens = malloc(maxTokens * sizeof(char*));
    if (!tokens){
        free(instr);
        free(tokens);
        return NULL;
    }

    //GETS INSTRUCTIONS
    int n = getSubstrings(line, ' ', tokens, maxTokens);
    if (n != maxTokens){
        free(instr);
        free(tokens);
        return NULL;
    }

    char **t = tokens; //Walks through the array.

    //TYPE:

    char *type_str = *t; t++;
    if (*type_str != 'r' && *type_str != 'i' && *type_str != 'j') {
        free(tokens);
        free(instr);
        return NULL;
    }
    instr->type = *type_str;

    //UID:

    char *uid_str = *t; t++;
    unsigned int uid = 0;
    char *p = uid_str;
    while ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')) {
        int digit;
        if (*p >= '0' && *p <= '9') digit = *p - '0';
        else if (*p >= 'a' && *p <= 'f') digit = *p - 'a' + 10;
        else digit = *p - 'A' + 10;

        uid = uid * 16 + digit;
        p++;
    }
    if (p == uid_str) {  // no hex digits
        free(tokens);
        free(instr);
        return NULL;
    }
    instr->uid = uid;

    //MNEM:
    char *mnemonic_str = *t; t++;
    char *q = mnemonic_str;
    int mnemonic_len = 0;
    while (*q >= 'a' && *q <= 'z') {
        mnemonic_len++;
        q++;
    }
    if (mnemonic_len == 0) {
        free(tokens);
        free(instr);
        return NULL;
    }
    instr->mnemonic = malloc(mnemonic_len + 1);
    if (!instr->mnemonic) {
        free(tokens);
        free(instr);
        return NULL;
    }
    q = mnemonic_str;
    char *dst = instr->mnemonic;
    while (*q >= 'a' && *q <= 'z') {
        *dst = *q;
        dst++;
        q++;
    }
    *dst = '\0';

    //PRETTY:
    char *pretty_str = *t; t++;
    int pretty = 0;
    p = pretty_str;
    while (*p >= '0' && *p <= '9') {
        pretty = pretty * 10 + (*p - '0');
        p++;
    }
    if (p == pretty_str || pretty < 0 || pretty > 10) {
        free(instr->mnemonic);
        free(tokens);
        free(instr);
        return NULL;
    }
    instr->pretty = (uint8_t)pretty;

    // set usage count
    instr->usagecnt = 0;

    // free token pointer array (substrings still valid in original line)
    free(tokens);

    return instr;
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

