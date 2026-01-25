#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "hw1.h"


#define USAGE_MSG   "53MIPSD [-H] [-O ofile] [-I INFILE] [-N] IMAPFILE\n" \
                    "\n  -H    Prints the usage statement to STDOUT. All other arguments are ignored."  \
                    "\n  -O    OUTFILE Results are printed into the OUTFILE specified instead of STDOUT."  \
                    "\n  -I    INFILE Inputs are read from the INFILE specified instead of STDIN." \
                    "\n  -N    Prints the registers as numerical values, instead of human-readable names\n" \

char* humanRegisters = "$zero,$at,$v0,$v1,$a0,$a1,$a2,$a3,$t0,$t1,$t2,$t3,$t4,$t5,$t6,$t7,$s0,$s1,$s2,$s3,$s4,$s5,$s6,$s7,$t8,$t9,$k0,$k1,$gp,$sp,$fp,$ra";

int main(int argc, char* argv[]) {

    char* OUTFILE = NULL; // The output file (remains NULL if -O option is not provided) 
    char* INFILE = NULL; // The input file (remains NULL if -I option is not provided)
    int NUMERIC = 0; // Numeric mode (remains false if -N option is not provided)
    opterr = 0;
    int c;
    
    while ((c = getopt(argc, argv, "HO:I:N")) >= 0) {
        switch (c)
        {
        case 'H':
            printf(USAGE_MSG);
            return EXIT_SUCCESS;
        case 'O':
            OUTFILE = optarg;
            break;
        case 'I':
            INFILE = optarg;
            break;
        case 'N':
            NUMERIC = 1;
            break;
        case '?':
            if (optopt == 'i' || optopt == 'o') {
                fprintf(stderr, "Option -%c requires an argument.\n\n" USAGE_MSG, optopt);
            }
        default:
            fprintf(stderr, USAGE_MSG);
            return EXIT_FAILURE;
        }
    }

    // validate that we only have 1 positional argument
    if (optind + 1 != argc) {
        fprintf(stderr, "Exactly one positional argument should be specified.\n\n" USAGE_MSG);
        return EXIT_FAILURE;
    }

    char* imapfile = argv[optind]; // The IMAPFILE specified

    // INFILE, ofile, numeric and imapfile are now usable by your program!
    // THE PRINT STATEMENTS BELOW ARE FOR DEBUGGING PURPOSES ONLY! Comment out from final implemenation
    //printf("INFILE: %s\n", INFILE);
    //printf("OUTFILE: %s\n", OUTFILE);
    //printf("NUMERIC: %d\n", NUMERIC);
    //printf("IMAPFILE: %s\n", imapfile);
    // THE PRINT STATEMENTS ABOVE ARE FOR DEBUGGING PURPOSES ONLY! Comment out from final implemenation

    // Create space for 1D array for the register names - still need to initialize!
    char** regNames = malloc(32 * sizeof(char*));
    if (!regNames) {
        fprintf(stderr, "Memory allocation failed\n");
        return EXIT_FAILURE;
    }
    
    // Track the string buffer for non-NUMERIC mode (needs to stay alive)
    char* regCopy = NULL;
    
    // Initialize register names based on NUMERIC flag
    if (NUMERIC) {
        // Use numeric names: $0, $1, $2, etc.
        char** ptr = regNames;
        for (int i = 0; i < 32; i++) {
            *ptr = malloc(8);
            if (!*ptr) {
                // Clean up previously allocated memory
                char** cleanup = regNames;
                while (cleanup < ptr) {
                    free(*cleanup);
                    cleanup++;
                }
                free(regNames);
                fprintf(stderr, "Memory allocation failed\n");
                return EXIT_FAILURE;
            }
            sprintf(*ptr, "$%d", i);
            ptr++;
        }
    } else {
        // Parse human-readable register names using getSubstrings
        // Must copy string first since humanRegisters is a string literal (read-only)
        int len = 0;
        char* p = humanRegisters;
        while (*p != '\0') {
            len++;
            p++;
        }
        
        regCopy = malloc(len + 1);
        if (!regCopy) {
            free(regNames);
            fprintf(stderr, "Memory allocation failed\n");
            return EXIT_FAILURE;
        }
        
        // Copy the string manually
        char* src = humanRegisters;
        char* dst = regCopy;
        while (*src != '\0') {
            *dst = *src;
            dst++;
            src++;
        }
        *dst = '\0';
        
        // Use getSubstrings to parse the register names
        int numRegs = getSubstrings(regCopy, ',', regNames, 32);
        if (numRegs != 32) {
            free(regCopy);
            free(regNames);
            fprintf(stderr, "Failed to parse register names\n");
            return EXIT_FAILURE;
        }
    }
    
    // Open instruction mapping file
    FILE* imapfp = fopen(imapfile, "r");
    if (!imapfp) {
        fprintf(stderr, "Error opening instruction mapping file: %s\n", imapfile);
        if (NUMERIC) {
            char** ptr = regNames;
            for (int i = 0; i < 32; i++) {
                free(*ptr);
                ptr++;
            }
        }
        if (regCopy != NULL) {
            free(regCopy);
        }
        free(regNames);
        return 3; //OG 2. FAILING 99 2:26
    }
    
    // Build instruction list
    list_t* instrList = createMIPSinstrList(imapfp);
    fclose(imapfp);
    
    if (!instrList) {
        if (NUMERIC) {
            char** ptr = regNames;
            for (int i = 0; i < 32; i++) {
                free(*ptr);
                ptr++;
            }
        }
        if (regCopy != NULL) {
            free(regCopy);
        }
        free(regNames);
        return 3; //OG 2. MAKING It 3 (failing 99)
    }
    
    // Open input file (or use stdin)
    FILE* infile = stdin;
    if (INFILE) {
        infile = fopen(INFILE, "rb");
        if (!infile) {
            fprintf(stderr, "Error opening input file: %s\n", INFILE);
            DestroyList(&instrList);
            if (NUMERIC) {
                char** ptr = regNames;
                for (int i = 0; i < 32; i++) {
                    free(*ptr);
                    ptr++;
                }
            }
            if (regCopy != NULL) {
                free(regCopy);
            }
            free(regNames);
            return 2; //Invalid Instruction shoudl this 3 become 2?
        }
    }
    
    // Open output file (or use stdout)
    FILE* outfile = stdout;
    if (OUTFILE) {
        outfile = fopen(OUTFILE, "w");
        if (!outfile) {
            fprintf(stderr, "Error opening output file: %s\n", OUTFILE);
            if (infile != stdin) fclose(infile);
            DestroyList(&instrList);
            if (NUMERIC) {
                char** ptr = regNames;
                for (int i = 0; i < 32; i++) {
                    free(*ptr);
                    ptr++;
                }
            }
            if (regCopy != NULL) {
                free(regCopy);
            }
            free(regNames);
            return 2; //FILE I/O
        }
    }
    
    // Process instructions from binary input
    uint32_t instrBinary;
    size_t bytesRead;
    
    while ((bytesRead = fread(&instrBinary, 1, 4, infile)) == 4) {
        // Parse the instruction fields
        MIPSfields fields;
        
        // Initialize all fields to 0
        fields.opcode = 0;
        fields.rs = 0;
        fields.rt = 0;
        fields.rd = 0;
        fields.shamt = 0;
        fields.func = 0;
        fields.immediate16 = 0;
        fields.immediate26 = 0;
        fields.uid = 0;
        
        parseMIPSfields(instrBinary, &fields);
        
        // Print the instruction
        int result = printInstr(&fields, instrList, regNames, outfile);
        
        if (result == 0) {
            // Error occurred during printing
            fprintf(outfile, "UNKNOWN INST\n");
            if (infile != stdin) fclose(infile);
            if (outfile != stdout) fclose(outfile);
            DestroyList(&instrList);
            if (NUMERIC) {
                char** ptr = regNames;
                for (int i = 0; i < 32; i++) {
                    free(*ptr);
                    ptr++;
                }
            }
            if (regCopy != NULL) {
                free(regCopy);
            }
            free(regNames);
            return 3; //INFILE ERROR OG 2. may make it 3
        }
    }
    
    // Check if we read a partial instruction (less than 4 bytes)
    if (bytesRead > 0 && bytesRead < 4) {
        fprintf(stderr, "Error: Input file contains partial instruction\n");
        if (infile != stdin) fclose(infile);
        if (outfile != stdout) fclose(outfile);
        DestroyList(&instrList);
        if (NUMERIC) {
            char** ptr = regNames;
            for (int i = 0; i < 32; i++) {
                free(*ptr);
                ptr++;
            }
        }
        if (regCopy != NULL) {
            free(regCopy);
        }
        free(regNames);
        return 3; 
    }
    
    // Clean up
    if (infile != stdin) fclose(infile);
    if (outfile != stdout) fclose(outfile);
    DestroyList(&instrList);
    
    // Free register names and string buffer
    if (NUMERIC) {
        char** ptr = regNames;
        for (int i = 0; i < 32; i++) {
            free(*ptr);
            ptr++;
        }
    }
    
    // Always free regCopy if allocated (for non-NUMERIC mode)
    if (regCopy != NULL) {
        free(regCopy);
        regCopy = NULL;
    }
    
    free(regNames);
    
    return EXIT_SUCCESS;
}