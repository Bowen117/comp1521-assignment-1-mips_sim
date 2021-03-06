// COMP1521 20T3 Assignment 1: mips_sim -- a MIPS simulator
// starting point code v0.1 - 13/10/20


// PUT YOUR HEADER COMMENT HERE


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#define MAX_LINE_LENGTH 256
#define INSTRUCTIONS_GROW 64

#define MUL 28
#define BEQ 4
#define BNE 5
#define ADDI 8
#define ORI 13
#define LUI 15
#define ADD 32
#define SUB 34
#define SLT 42
#define SYS 12

void execute_instructions(int n_instructions,
                          uint32_t instructions[n_instructions],
                          int trace_mode);
char *process_arguments(int argc, char *argv[], int *trace_mode);
uint32_t *read_instructions(char *filename, int *n_instructions_p);
uint32_t *instructions_realloc(uint32_t *instructions, int n_instructions);

// ADD YOUR FUNCTION PROTOTYPES HERE
int process_code(int *registers, uint32_t instruction, int trace_mode, int pc);
void add(int trace_mode, int *registers, int reg1, int reg2, int reg3);
void sub(int trace_mode, int *registers, int reg1, int reg2, int reg3);
void slt(int trace_mode, int *registers, int reg1, int reg2, int reg3);
void mul(int trace_mode, int *registers, int reg1, int reg2, int reg3);
void addi(int trace_mode, int *registers, int reg1, int reg2, int imm);
void ori(int trace_mode, int *registers, int reg1, int reg2, int imm);
void lui(int trace_mode, int *registers, int reg1, int imm);

// YOU SHOULD NOT NEED TO CHANGE MAIN

int main(int argc, char *argv[]) {
    int trace_mode;
    char *filename = process_arguments(argc, argv, &trace_mode);

    int n_instructions;
    uint32_t *instructions = read_instructions(filename, &n_instructions);

    execute_instructions(n_instructions, instructions, trace_mode);

    free(instructions);
    return 0;
}


// simulate execution of  instruction codes in  instructions array
// output from syscall instruction & any error messages are printed
//
// if trace_mode != 0:
//     information is printed about each instruction as it executed
//
// execution stops if it reaches the end of the array
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void execute_instructions(int n_instructions,
                          uint32_t instructions[n_instructions],
                          int trace_mode) {
    // REPLACE CODE BELOW WITH YOUR CODE

    int pc = 0;

    int registers[32];
    for (int i=0; i<32; i++) 
        registers[i] = 0;
           
    while (pc < n_instructions) {
        if (pc < 0) {
            printf("Illegal branch to address before instructions: PC = %d\n", pc);
            exit(0);
        }
        if (trace_mode) {
            printf("%d: 0x%08X", pc, instructions[pc]);
        }
        pc += process_code(registers, instructions[pc], trace_mode, pc);
    }
    if (pc > n_instructions) {
        printf("Illegal branch to address after instructions: PC = %d\n", pc);  
        exit(0);
    }  
}


// ADD YOUR FUNCTIONS HERE

int process_code(int *registers, uint32_t instruction, int trace_mode, int pc) {
    // #DEFINE some values for different operations (that perhaps you can
    // do OR operations with).
    
    // extract first 6 bits (from left).
    // if it's all 0s, read the last 6 bits to determine operation.
    uint32_t operation = instruction >> 26;
    if (operation == 0) {
        operation = (instruction & 2047);
        if (operation == ADD) {
            uint32_t reg1 = instruction >> 21;
            reg1 = reg1 & 31;
            uint32_t reg2 = instruction >> 16;
            reg2 = reg2 & 31;
            uint32_t reg3 = instruction >> 11;
            reg3 = reg3 & 31;
            
            add(trace_mode, registers, reg1, reg2, reg3);
        }
        else if (operation == SUB) {
            uint32_t reg1 = instruction >> 21;
            reg1 = reg1 & 31;
            uint32_t reg2 = instruction >> 16;
            reg2 = reg2 & 31;
            uint32_t reg3 = instruction >> 11;
            reg3 = reg3 & 31;
            
            sub(trace_mode, registers, reg1, reg2, reg3);
        }
        else if (operation == SLT) {
            uint32_t reg1 = instruction >> 21;
            reg1 = reg1 & 31;
            uint32_t reg2 = instruction >> 16;
            reg2 = reg2 & 31;
            uint32_t reg3 = instruction >> 11;
            reg3 = reg3 & 31;
            
            slt(trace_mode, registers, reg1, reg2, reg3);
        }
        else if (operation == SYS) { //SYSCALL
            uint32_t service = registers[2]; // $v0
            uint32_t argument = registers[4]; // $a0
            if (trace_mode) {
                printf(" syscall\n");
                printf(">>> syscall %d\n", service);               
            }
            if (service == 1) { // PRINT INTEGER
                if (trace_mode) {
                    printf("<<< ");
                    printf("%d\n", argument);
                } else {
                    printf("%d", argument);
                }
            }
            else if (service == 10) { // exit(0)
                exit(0);
            }
            else if (service == 11) { // print char
                argument = (argument & 255);
                if (trace_mode) {
                    printf("<<< ");                   
                    printf("%c\n", argument);
                } else {
                    printf("%c", argument);
                }               
            } else {
                printf("Unknown system call: %d\n", service);
                exit(0);
            }
        } else {
            if (trace_mode) {
                printf(" invalid instruction code\n");
            } else {
                printf("invalid instruction code\n");
            }
            exit(0);   
        }
        return 1;    
    } else {
        if (operation == MUL) {
            uint32_t reg1 = instruction >> 21;
            reg1 = reg1 & 31;
            uint32_t reg2 = instruction >> 16;
            reg2 = reg2 & 31;
            uint32_t reg3 = instruction >> 11;
            reg3 = reg3 & 31;
            
            mul(trace_mode, registers, reg1, reg2, reg3);
        }
        else if (operation == ADDI) {
            uint32_t reg1 = instruction >> 21;
            reg1 = reg1 & 31;
            uint32_t reg2 = instruction >> 16;
            reg2 = reg2 & 31;
            uint32_t imm = (instruction & 65535);
            int x = (int16_t) imm;
            
            addi(trace_mode, registers, reg1, reg2, x);
        }
        else if (operation == ORI) {
            uint32_t reg1 = instruction >> 21;
            reg1 = reg1 & 31;
            uint32_t reg2 = instruction >> 16;
            reg2 = reg2 & 31;
            uint32_t imm = (instruction & 65535);
            
            ori(trace_mode, registers, reg1, reg2, imm);
        }
        else if (operation == LUI) {
            uint32_t reg1 = instruction >> 16;
            reg1 = reg1 & 31;
            uint32_t imm = (instruction & 65535);
            
            lui(trace_mode, registers, reg1, imm);
        }
        else if (operation == BEQ) {
            uint32_t reg1 = instruction >> 21;
            reg1 = reg1 & 31;
            uint32_t reg2 = instruction >> 16;
            reg2 = reg2 & 31; 
            uint32_t imm = (instruction & 65535);   
            int x = (int16_t) imm;
            if (registers[reg1] == registers[reg2]) {
                if (trace_mode) {
                   printf(" beq  $%d, $%d, %d\n", reg1, reg2, x);
                   printf(">>> branch taken to PC = %d\n", (pc + x));
                }
                return x; 
            } else {
                printf(">>> branch not taken\n");
            }
       }      
       else if (operation == BNE) { 
            uint32_t reg1 = instruction >> 21;
            reg1 = reg1 & 31;
            uint32_t reg2 = instruction >> 16;
            reg2 = reg2 & 31; 
            uint32_t imm = (instruction & 65535); 
            int x = (int16_t) imm;
            if (registers[reg1] != registers[reg2]) {
                if (trace_mode) {
                    printf(" bne  $%d, $%d, %d\n", reg1, reg2, x);
                    printf(">>> branch taken to PC = %d\n", (pc + x));
                }
                return x;
            } else {
                if (trace_mode) {
                    printf(" bne  $%d, $%d, %d\n", reg1, reg2, x);
                    printf(">>> branch not taken\n");
                }               
            }
       } else {
            if (trace_mode) {
                printf(" invalid instruction code\n");
            } else {
                printf("invalid instruction code\n");
            }
            exit(0);
       } 
              
   }
   return 1; 
}

void add(int trace_mode, int *registers, int reg1, int reg2, int reg3) {    
    registers[reg3] = registers[reg1] + registers[reg2];
    if (trace_mode) {
        printf(" add  $%d, $%d, $%d\n", reg3, reg1, reg2);
        printf(">>> $%d = %d\n", reg3, registers[reg3]);
    } 
    registers[0] = 0;
}

void sub(int trace_mode, int *registers, int reg1, int reg2, int reg3) {
    registers[reg3] = registers[reg1] - registers[reg2];
    if (trace_mode) {
        printf(" sub  $%d, $%d, $%d\n", reg3, reg1, reg2);
        printf(">>> $%d = %d\n", reg3, registers[reg3]);
    } 
    registers[0] = 0;
}

void slt(int trace_mode, int *registers, int reg1, int reg2, int reg3) {
    if (registers[reg1] < registers[reg2] ) {
        registers[reg3] = 1;
    } else {
        registers[reg3] = 0;
    }
    if (trace_mode) {
        printf(" slt  $%d, $%d, $%d\n", reg3, reg1, reg2);
        printf(">>> $%d = %d\n", reg3, registers[reg3]);
    } 
    registers[0] = 0;
}

void mul(int trace_mode, int *registers, int reg1, int reg2, int reg3) {
    registers[reg3] = registers[reg1] * registers[reg2];
    if (trace_mode) {
        printf(" mul  $%d, $%d, $%d\n", reg3, reg1, reg2);
        printf(">>> $%d = %d\n", reg3, registers[reg3]);
    } 
    registers[0] = 0;
}

void addi(int trace_mode, int *registers, int reg1, int reg2, int imm) {
    registers[reg2] = registers[reg1] + imm;
    if (trace_mode) {
        printf(" addi $%d, $%d, %d\n", reg2, reg1, imm);
        printf(">>> $%d = %d\n", reg2, registers[reg2]);
    } 
    registers[0] = 0;
}

void ori(int trace_mode, int *registers, int reg1, int reg2, int imm) {
    registers[reg2] = (registers[reg1] | imm);
    if (trace_mode) {
        printf(" ori  $%d, $%d, %d\n", reg2, reg1, imm);
        printf(">>> $%d = %d\n", reg2, registers[reg2]);
    } 
    registers[0] = 0;
}

void lui(int trace_mode, int *registers, int reg1, int imm) {
    registers[reg1] = (imm << 16);
    if (trace_mode) {
        printf(" lui  $%d, %d\n", reg1, imm);
        printf(">>> $%d = %d\n", reg1, registers[reg1]);
    } 
    registers[0] = 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// YOU DO NOT NEED TO CHANGE CODE BELOW HERE


// check_arguments is given command-line arguments
// it sets *trace_mode to 0 if -r is specified
//          *trace_mode is set to 1 otherwise
// the filename specified in command-line arguments is returned

char *process_arguments(int argc, char *argv[], int *trace_mode) {
    if (
        argc < 2 ||
        argc > 3 ||
        (argc == 2 && strcmp(argv[1], "-r") == 0) ||
        (argc == 3 && strcmp(argv[1], "-r") != 0)) {
        fprintf(stderr, "Usage: %s [-r] <file>\n", argv[0]);
        exit(1);
    }
    *trace_mode = (argc == 2);
    return argv[argc - 1];
}


// read hexadecimal numbers from filename one per line
// numbers are return in a malloc'ed array
// *n_instructions is set to size of the array

uint32_t *read_instructions(char *filename, int *n_instructions_p) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "%s: '%s'\n", strerror(errno), filename);
        exit(1);
    }

    uint32_t *instructions = NULL;
    int n_instructions = 0;
    char line[MAX_LINE_LENGTH + 1];
    while (fgets(line, sizeof line, f) != NULL) {

        // grow instructions array in steps of INSTRUCTIONS_GROW elements
        if (n_instructions % INSTRUCTIONS_GROW == 0) {
            instructions = instructions_realloc(instructions, n_instructions + INSTRUCTIONS_GROW);
        }

        char *endptr;
        instructions[n_instructions] = strtol(line, &endptr, 16);
        if (*endptr != '\n' && *endptr != '\r' && *endptr != '\0') {
            fprintf(stderr, "%s:line %d: invalid hexadecimal number: %s",
                    filename, n_instructions + 1, line);
            exit(1);
        }
        n_instructions++;
    }
    fclose(f);
    *n_instructions_p = n_instructions;
    // shrink instructions array to correct size
    instructions = instructions_realloc(instructions, n_instructions);
    return instructions;
}


// instructions_realloc is wrapper for realloc
// it calls realloc to grow/shrink the instructions array
// to the speicfied size
// it exits if realloc fails
// otherwise it returns the new instructions array
uint32_t *instructions_realloc(uint32_t *instructions, int n_instructions) {
    instructions = realloc(instructions, n_instructions * sizeof *instructions);
    if (instructions == NULL) {
        fprintf(stderr, "out of memory");
        exit(1);
    }
    return instructions;
}
