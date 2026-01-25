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
        //free(tokens);
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
    const MIPSinstr* a = (const MIPSinstr*) s1;
    const MIPSinstr* b = (const MIPSinstr*) s2;

    if (a->uid < b->uid) return -1;
    if (a->uid == b->uid)return 0;
    return 1;
}

void MIPSinstr_Printer(void* data, void* fp) {
    if (!data || !fp) return;

    MIPSinstr* instr = (MIPSinstr*) data;
    FILE* file = (FILE*) fp;

    // Print type (char)
    fprintf(file, "%c\t", instr->type);

    // Print unsigned numeric fields
    fprintf(file, "%u\t", instr->uid);
    fprintf(file, "%u\t", instr->pretty);
    fprintf(file, "%u\t", instr->usagecnt);

    // Print mnemonic string safely
    if (instr->mnemonic)
        fprintf(file, "%s", instr->mnemonic);
    
    fprintf(file, "\n"); // newline at end
}

void MIPSinstr_Deleter(void* data) {
    if (!data) return; //CHECKS IF NULL

    MIPSinstr* instr = (MIPSinstr*) data; //types it as MIPSinstr

    if (instr->mnemonic) { 
        free(instr->mnemonic);
        instr->mnemonic = NULL; 
    }

    free(instr);
}

node_t* FindInList(list_t* list, void* token) {
    if (!list || !token || !list->comparator) { //checks for NULL pointers
        return NULL; //not valid LL
    }

    node_t* current = list->head; //dont forget ->head to get the point where it 
    while (current != NULL){
        if (current->data && list->comparator(current->data, token) == 0) {
            return current;
        }
        current = current -> next; //goes to the next node
    }
    return NULL; //not found
}

void DestroyList(list_t** list)  {
    if (!list) return; //void, no return value

    list_t* listPtr = *list;
    node_t* current = listPtr->head; //points to start of the list

    while (current != NULL){
        node_t* next = current->next;

        if (listPtr->deleter && current->data) {
            listPtr->deleter(current->data);
        }
        free(current);
        current = next;
    }
    free(listPtr);
    *list = NULL;
}


// Part 3 Functions
list_t* createMIPSinstrList(FILE* IMAPFILE) {
    if (!IMAPFILE) return NULL;

    list_t* instrList = CreateList(MIPSinstr_uidComparator,
                               MIPSinstr_Printer,
                               MIPSinstr_Deleter);

    if (!instrList) return NULL;

    char* buffer = malloc(256 * sizeof(char));
    if (!buffer) {
        DestroyList(&instrList);
        return NULL;
    }

    while (fgets(buffer, 256, IMAPFILE)) {

        char* end = buffer;
        while (*end != '\0') end++;
        if (end != buffer && *(end - 1) == '\n') *(end - 1) = '\0';

        MIPSinstr* instr = loadInstrFormat(buffer);
        if (!instr) {
            DestroyList(&instrList);
            free(buffer);
            return NULL;
        }

        if (FindInList(instrList, instr)) {
            MIPSinstr_Deleter(instr);
            DestroyList(&instrList);
            free(buffer);
            return NULL;
        }

        InsertAtHead(instrList, instr);
    }

    free(buffer);  // free line buffer
    return instrList;
}

int printInstr(MIPSfields* instr, list_t* MIPSinstrList, char** regNames, FILE* OUTFILE) {
    if (!instr || !MIPSinstrList || !regNames || !OUTFILE) return 0;
    
    MIPSinstr temp;
    temp.uid = instr->uid;
    
    node_t* node = FindInList(MIPSinstrList, &temp);
    if (!node || !node->data) return 0;
    
    MIPSinstr* mapping = (MIPSinstr*) node->data;
    if (!mapping || !mapping->mnemonic) return 0;
    
    mapping->usagecnt++;
    char* mnemonic = mapping->mnemonic;
    char inst_type = mapping->type;
    
    switch (mapping->pretty) {
        case 0: {
            // Special R-type: mnemonic rd, rt, shamt (for shifts) or just rd, rs
            if (inst_type == 'r') {
                // Check if it's a shift instruction or move instruction
                if (instr->rs == 0) {
                    // sll, srl, sra: rd, rt, shamt
                    fprintf(OUTFILE, "%s %s, %s, %d\n",
                            mnemonic,
                            *(regNames + instr->rd),
                            *(regNames + instr->rt),
                            instr->shamt);
                } else {
                    // jr, mfhi, etc: just rs or just rd
                    fprintf(OUTFILE, "%s %s\n",
                            mnemonic,
                            *(regNames + instr->rs));
                }
            } else {
                fprintf(OUTFILE, "%s\n", mnemonic);
            }
            break;
        }
        
        case 1: {
            // R-type two registers: mnemonic rs, rt (div, divu)
            fprintf(OUTFILE, "%s %s, %s\n",
                    mnemonic,
                    *(regNames + instr->rs),
                    *(regNames + instr->rt));
            break;
        }
        
        case 2: {
            // I-type or special: mnemonic rt, rs, immediate (hex)
            if (inst_type == 'i') {
                fprintf(OUTFILE, "%s %s, %s, 0x%X\n",
                        mnemonic,
                        *(regNames + instr->rt),
                        *(regNames + instr->rs),
                        instr->immediate16);
            } else if (inst_type == 'r') {
                // jalr, mult, multu: special R-type
                fprintf(OUTFILE, "%s %s, %s\n",
                        mnemonic,
                        *(regNames + instr->rs),
                        *(regNames + instr->rt));
            } else {
                // J-type: target
                fprintf(OUTFILE, "%s 0x%X\n",
                        mnemonic,
                        instr->immediate26);
            }
            break;
        }
        
        case 3: {
            // Normal R-type: mnemonic rd, rs, rt
            fprintf(OUTFILE, "%s %s, %s, %s\n",
                    mnemonic,
                    *(regNames + instr->rd),
                    *(regNames + instr->rs),
                    *(regNames + instr->rt));
            break;
        }
        
        case 4: {
            // Load/Store: mnemonic rt, immediate(rs)
            fprintf(OUTFILE, "%s %s, 0x%X(%s)\n",
                    mnemonic,
                    *(regNames + instr->rt),
                    instr->immediate16,
                    *(regNames + instr->rs));
            break;
        }
        
        case 5: {
            // No operands (syscall)
            fprintf(OUTFILE, "%s\n", mnemonic);
            break;
        }
        
        case 6: {
            // J-type: mnemonic target
            fprintf(OUTFILE, "%s 0x%X\n",
                    mnemonic,
                    instr->immediate26);
            break;
        }
        
        case 7: {
            // Branch with one register: mnemonic rs, offset
            fprintf(OUTFILE, "%s %s, 0x%X\n",
                    mnemonic,
                    *(regNames + instr->rs),
                    instr->immediate16);
            break;
        }
        
        case 8: {
            // Shift variable: mnemonic rd, rt, shamt
            fprintf(OUTFILE, "%s %s, %s, %d\n",
                    mnemonic,
                    *(regNames + instr->rd),
                    *(regNames + instr->rt),
                    instr->shamt);
            break;
        }
        
        case 9: {
            // Branch: mnemonic rs, rt, offset
            fprintf(OUTFILE, "%s %s, %s, 0x%X\n",
                    mnemonic,
                    *(regNames + instr->rs),
                    *(regNames + instr->rt),
                    instr->immediate16);
            break;
        }
        
        case 10: {
            // I-type: mnemonic rt, immediate
            fprintf(OUTFILE, "%s %s, 0x%X\n",
                    mnemonic,
                    *(regNames + instr->rt),
                    instr->immediate16);
            break;
        }
        
        default: {
            fprintf(OUTFILE, "%s\n", mnemonic);
            break;
        }
    }
    
    return 1;
}

