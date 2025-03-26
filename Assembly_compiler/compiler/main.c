/**
 * @file main.c
 * @brief Main implementation of the Assembly Language Compiler
 * 
 * This file contains the main implementation of the compiler, including
 * lexical analysis, symbol table generation, and intermediate code generation.
 * 
 * @copyright Copyright (c) 2017 Komma Ravi Teja
 * @license MIT License
 */

#include "FunctionHeaders.h"

/* Global variables */
int intermediate_index = 0;
intermediate_lang **intermediate_table = NULL;

int symbol_index = 0;
symbol_table **symbol_tab = NULL;

int blocks_index = 0;
blocks_table **block_tab = NULL;

/**
 * @brief Processes a CONST declaration
 * 
 * This function adds a constant to the symbol table and stores its value
 * in the memory array.
 * 
 * @param tokens Array of tokens from the current line
 * @param memory Memory array
 * @param memory_index Pointer to the current memory index
 */
void const_func(char (*tokens)[10], int *memory, int *memory_index) {
    if (symbol_index == 0) {
        symbol_tab[symbol_index]->address = VARIABLE_MEMORY_START;
        strncpy(symbol_tab[symbol_index]->variable_name, tokens[1], VARIABLE_LENGTH - 1);
        symbol_tab[symbol_index]->variable_name[VARIABLE_LENGTH - 1] = '\0';
        symbol_tab[symbol_index]->size = CONST_VARIABLE_SIZE;
    } else {
        strncpy(symbol_tab[symbol_index]->variable_name, tokens[1], VARIABLE_LENGTH - 1);
        symbol_tab[symbol_index]->variable_name[VARIABLE_LENGTH - 1] = '\0';
        symbol_tab[symbol_index]->size = CONST_VARIABLE_SIZE;
        
        if (symbol_tab[symbol_index - 1]->size != 0) {
            symbol_tab[symbol_index]->address = symbol_tab[symbol_index - 1]->address + 
                                               symbol_tab[symbol_index - 1]->size;
        } else {
            symbol_tab[symbol_index]->address = symbol_tab[symbol_index - 1]->address + 1;
        }
    }
    
    symbol_index++;
    memory[*memory_index] = atoi(tokens[3]);
    (*memory_index)++;
}

/**
 * @brief Processes a DATA declaration
 * 
 * This function adds a variable or array to the symbol table.
 * 
 * @param tokens Array of tokens from the current line
 * @param memory Memory array
 * @param memory_index Pointer to the current memory index
 */
void data_func(char (*tokens)[10], int *memory, int *memory_index) {
    int i = 0, size = 0;
    char variable_name[VARIABLE_LENGTH];
    
    if (symbol_index == 0) {
        symbol_tab[symbol_index]->address = VARIABLE_MEMORY_START;
        
        /* Extract variable name (without array size) */
        while (tokens[1][i] != '\0' && tokens[1][i] != '[') {
            variable_name[i] = tokens[1][i];
            i++;
        }
        variable_name[i] = '\0';
        
        strncpy(symbol_tab[symbol_index]->variable_name, variable_name, VARIABLE_LENGTH - 1);
        symbol_tab[symbol_index]->variable_name[VARIABLE_LENGTH - 1] = '\0';
        
        /* Check if it's an array and extract size */
        if (tokens[1][i] == '[') {
            i++; /* Skip '[' */
            while (tokens[1][i] != ']' && tokens[1][i] != '\0') {
                size = size * 10 + (tokens[1][i] - '0');
                i++;
            }
        }
        
        /* Set size (default to 1 for scalar variables) */
        symbol_tab[symbol_index]->size = (size > 0) ? size : 1;
        
        /* Update memory index */
        *memory_index = symbol_tab[symbol_index]->address + symbol_tab[symbol_index]->size;
        symbol_index++;
        return;
    } else {
        /* Extract variable name (without array size) */
        while (tokens[1][i] != '\0' && tokens[1][i] != '[') {
            variable_name[i] = tokens[1][i];
            i++;
        }
        variable_name[i] = '\0';
        
        strncpy(symbol_tab[symbol_index]->variable_name, variable_name, VARIABLE_LENGTH - 1);
        symbol_tab[symbol_index]->variable_name[VARIABLE_LENGTH - 1] = '\0';
        
        /* Check if it's an array and extract size */
        if (tokens[1][i] == '[') {
            i++; /* Skip '[' */
            while (tokens[1][i] != ']' && tokens[1][i] != '\0') {
                size = size * 10 + (tokens[1][i] - '0');
                i++;
            }
        }
        
        /* Set size (default to 1 for scalar variables) */
        symbol_tab[symbol_index]->size = (size > 0) ? size : 1;
        
        /* Calculate address based on previous variable */
        if (symbol_tab[symbol_index - 1]->size != 0) {
            symbol_tab[symbol_index]->address = symbol_tab[symbol_index - 1]->address + 
                                               symbol_tab[symbol_index - 1]->size;
        } else {
            symbol_tab[symbol_index]->address = symbol_tab[symbol_index - 1]->address + 1;
        }
        
        /* Update memory index */
        *memory_index = symbol_tab[symbol_index]->address + symbol_tab[symbol_index]->size;
        symbol_index++;
        return;
    }
}

/**
 * @brief Generates an opcode for an instruction mnemonic
 * 
 * @param instruction Instruction mnemonic
 * @return int Opcode for the instruction
 */
int generate_opcode(const char *instruction) {
    if (strcmp(instruction, "MOV") == 0)
        return OP_MOV_MEM_TO_REG;
    if (strcmp(instruction, "ADD") == 0)
        return OP_ADD;
    if (strcmp(instruction, "SUB") == 0)
        return OP_SUB;
    if (strcmp(instruction, "MUL") == 0)
        return OP_MUL;
    if (strcmp(instruction, "JUMP") == 0 || strcmp(instruction, "ELSE") == 0)
        return OP_JUMP;
    if (strcmp(instruction, "IF") == 0)
        return OP_IF;
    if (strcmp(instruction, "EQ") == 0)
        return OP_EQ;
    if (strcmp(instruction, "LT") == 0)
        return OP_LT;
    if (strcmp(instruction, "GT") == 0)
        return OP_GT;
    if (strcmp(instruction, "LTEQ") == 0)
        return OP_LTEQ;
    if (strcmp(instruction, "GTEQ") == 0)
        return OP_GTEQ;
    if (strcmp(instruction, "PRINT") == 0)
        return OP_PRINT;
    if (strcmp(instruction, "READ") == 0)
        return OP_READ;
    if (strcmp(instruction, "ENDIF") == 0)
        return OP_ENDIF;
    if (strcmp(instruction, "END") == 0)
        return OP_END;
    
    fprintf(stderr, "Warning: Unknown instruction '%s'\n", instruction);
    return -1;
}

/**
 * @brief Gets the memory address for a variable or register
 * 
 * @param variable_name Variable or register name
 * @return int Memory address, or -1 if not found
 */
int getAddress(char *variable_name) {
    int i = 0, is_array = 0, array_index = 0;
    char temp[VARIABLE_LENGTH];
    
    /* Check if it's a register (AX, BX, etc.) */
    if (variable_name[1] == 'X' && variable_name[0] >= 'A' && variable_name[0] <= 'H') {
        return variable_name[0] - 'A'; /* Register addresses are 0-7 */
    } else {
        /* Check if it's an array reference */
        while (variable_name[i] != '\0') {
            if (variable_name[i] == '[') {
                sscanf(variable_name, "%[^[]", temp);
                is_array = 1;
                i++;
                
                /* Parse array index */
                while (variable_name[i] != ']' && variable_name[i] != '\0') {
                    array_index = array_index * 10 + (variable_name[i] - '0');
                    i++;
                }
                break;
            }
            i++;
        }
        
        if (is_array == 0) {
            sscanf(variable_name, "%[^*]", temp);
        }
    }
    
    /* Copy extracted variable name */
    strncpy(variable_name, temp, VARIABLE_LENGTH - 1);
    variable_name[VARIABLE_LENGTH - 1] = '\0';
    
    /* Look up in symbol table */
    for (int i = 0; i < symbol_index; i++) {
        if (strcmp(symbol_tab[i]->variable_name, variable_name) == 0) {
            if (is_array) {
                return symbol_tab[i]->address + array_index;
            } else {
                return symbol_tab[i]->address;
            }
        }
    }
    
    fprintf(stderr, "Error: Variable '%s' not found\n", variable_name);
    return -1; /* Variable not found */
}

/**
 * @brief Processes a MOV instruction
 * 
 * @param param Parameters for the instruction
 * @param instruction_no Current instruction number
 */
void mov_func(char *param, int instruction_no) {
    char dest[VARIABLE_LENGTH], src[VARIABLE_LENGTH];
    char *token;
    
    /* Parse parameters */
    token = strtok(param, ", ");
    if (token == NULL) {
        fprintf(stderr, "Error: Invalid MOV instruction at line %d\n", instruction_no);
        return;
    }
    strncpy(dest, token, VARIABLE_LENGTH - 1);
    dest[VARIABLE_LENGTH - 1] = '\0';
    
    token = strtok(NULL, ", ");
    if (token == NULL) {
        fprintf(stderr, "Error: Invalid MOV instruction at line %d\n", instruction_no);
        return;
    }
    strncpy(src, token, VARIABLE_LENGTH - 1);
    src[VARIABLE_LENGTH - 1] = '\0';
    
    /* Set up instruction */
    intermediate_table[intermediate_index]->instruc_no = instruction_no;
    
    if (dest[1] == 'X' && dest[0] >= 'A' && dest[0] <= 'H') {
        /* Destination is register */
        intermediate_table[intermediate_index]->opcode = OP_MOV_REG_TO_MEM;
        intermediate_table[intermediate_index]->parameters[0] = getAddress(dest);
        intermediate_table[intermediate_index]->parameters[1] = getAddress(src);
        intermediate_table[intermediate_index]->parameters[2] = -1;  /* End marker */
    } else {
        /* Destination is memory */
        intermediate_table[intermediate_index]->opcode = OP_MOV_MEM_TO_REG;
        intermediate_table[intermediate_index]->parameters[0] = getAddress(dest);
        intermediate_table[intermediate_index]->parameters[1] = getAddress(src);
        intermediate_table[intermediate_index]->parameters[2] = -1;  /* End marker */
    }
    
    intermediate_index++;
}

/**
 * @brief Processes binary operations (ADD, SUB, MUL)
 * 
 * @param opcode Operation code
 * @param param Parameters for the instruction
 * @param instruction_no Current instruction number
 */
void binaryOperations_func(int opcode, char *param, int instruction_no) {
    char dest[VARIABLE_LENGTH], operand1[VARIABLE_LENGTH], operand2[VARIABLE_LENGTH];
    char *token;
    
    /* Parse parameters */
    token = strtok(param, ", ");
    if (token == NULL) {
        fprintf(stderr, "Error: Invalid binary operation at line %d\n", instruction_no);
        return;
    }
    strncpy(dest, token, VARIABLE_LENGTH - 1);
    dest[VARIABLE_LENGTH - 1] = '\0';
    
    token = strtok(NULL, ", ");
    if (token == NULL) {
        fprintf(stderr, "Error: Invalid binary operation at line %d\n", instruction_no);
        return;
    }
    strncpy(operand1, token, VARIABLE_LENGTH - 1);
    operand1[VARIABLE_LENGTH - 1] = '\0';
    
    token = strtok(NULL, ", ");
    if (token == NULL) {
        fprintf(stderr, "Error: Invalid binary operation at line %d\n", instruction_no);
        return;
    }
    strncpy(operand2, token, VARIABLE_LENGTH - 1);
    operand2[VARIABLE_LENGTH - 1] = '\0';
    
    /* Set up instruction */
    intermediate_table[intermediate_index]->opcode = opcode;
    intermediate_table[intermediate_index]->instruc_no = instruction_no;
    intermediate_table[intermediate_index]->parameters[0] = getAddress(dest);
    intermediate_table[intermediate_index]->parameters[1] = getAddress(operand1);
    intermediate_table[intermediate_index]->parameters[2] = getAddress(operand2);
    intermediate_table[intermediate_index]->parameters[3] = -1;  /* End marker */
    
    intermediate_index++;
}

/**
 * @brief Processes a READ instruction
 * 
 * @param param Parameter for the instruction
 * @param instruction_no Current instruction number
 */
void read_func(char *param, int instruction_no) {
    intermediate_table[intermediate_index]->parameters[0] = getAddress(param);
    intermediate_table[intermediate_index]->parameters[1] = -1;  /* End marker */
    intermediate_table[intermediate_index]->opcode = OP_READ;
    intermediate_table[intermediate_index]->instruc_no = instruction_no;
    
    intermediate_index++;
}

/**
 * @brief Processes a PRINT instruction
 * 
 * @param param Parameter for the instruction
 * @param instruction_no Current instruction number
 */
void print_func(char *param, int instruction_no) {
    intermediate_table[intermediate_index]->parameters[0] = getAddress(param);
    intermediate_table[intermediate_index]->parameters[1] = -1;  /* End marker */
    intermediate_table[intermediate_index]->opcode = OP_PRINT;
    intermediate_table[intermediate_index]->instruc_no = instruction_no;
    
    intermediate_index++;
}

/**
 * @brief Processes an IF instruction
 * 
 * @param param Parameters for the instruction
 * @param instruction_no Current instruction number
 * @param stack Stack for tracking nested control structures
 * @param top Pointer to the stack top
 */
void if_func(char *param, int instruction_no, int *stack, int *top) {
    char operand1[VARIABLE_LENGTH], oper[4], operand2[VARIABLE_LENGTH];
    
    /* Parse parameters */
    if (sscanf(param, "%s %s %s", operand1, oper, operand2) != 3) {
        fprintf(stderr, "Error: Invalid IF statement at line %d\n", instruction_no);
        return;
    }
    
    /* Set up instruction */
    intermediate_table[intermediate_index]->instruc_no = instruction_no;
    intermediate_table[intermediate_index]->opcode = OP_IF;
    intermediate_table[intermediate_index]->parameters[0] = getAddress(operand1);
    intermediate_table[intermediate_index]->parameters[1] = getAddress(operand2);
    intermediate_table[intermediate_index]->parameters[2] = generate_opcode(oper);
    intermediate_table[intermediate_index]->parameters[3] = WILDCARD_VALUE;  /* To be filled later */
    intermediate_table[intermediate_index]->parameters[4] = -1;  /* End marker */
    
    /* Push into stack */
    if (*top >= STACK_SIZE - 1) {
        fprintf(stderr, "Error: Stack overflow at line %d\n", instruction_no);
        return;
    }
    stack[++(*top)] = instruction_no;
    
    intermediate_index++;
}

/**
 * @brief Processes an ELSE instruction
 * 
 * @param instruction_no Current instruction number
 * @param stack Stack for tracking nested control structures
 * @param top Pointer to the stack top
 */
void else_func(int instruction_no, int *stack, int *top) {
    /* Set up instruction */
    intermediate_table[intermediate_index]->instruc_no = instruction_no;
    intermediate_table[intermediate_index]->opcode = OP_JUMP;
    intermediate_table[intermediate_index]->parameters[0] = WILDCARD_VALUE;  /* To be filled later */
    intermediate_table[intermediate_index]->parameters[1] = -1;  /* End marker */
    
    /* Push into stack */
    if (*top >= STACK_SIZE - 1) {
        fprintf(stderr, "Error: Stack overflow at line %d\n", instruction_no);
        return;
    }
    stack[++(*top)] = instruction_no;
    
    intermediate_index++;
}

/**
 * @brief Processes an ENDIF instruction
 * 
 * @param instruction_no Current instruction number
 * @param stack Stack for tracking nested control structures
 * @param top Pointer to the stack top
 */
void endif_func(int instruction_no, int *stack, int *top) {
    if (*top < 0) {
        fprintf(stderr, "Error: Unmatched ENDIF at line %d\n", instruction_no);
        return;
    }
    
    /* Pop ELSE or IF from stack */
    int popped_value = stack[(*top)--];
    int i = intermediate_index;
    
    /* Find the instruction */
    while (i > 0 && intermediate_table[i-1]->instruc_no != popped_value) {
        i--;
    }
    
    if (i <= 0) {
        fprintf(stderr, "Error: Could not find matching IF/ELSE for ENDIF at line %d\n", instruction_no);
        return;
    }
    
    /* Update the jump target */
    intermediate_table[i-1]->parameters[0] = instruction_no;
    
    /* If it was an ELSE, we're done */
    if (intermediate_table[i-1]->opcode == OP_JUMP) {
        return;
    }
    
    /* If it was an IF, we need to update the false branch target */
    if (*top < 0) {
        fprintf(stderr, "Error: Unmatched IF-ENDIF at line %d\n", instruction_no);
        return;
    }
    
    /* Pop the matching IF */
    int temp = popped_value;
    popped_value = stack[(*top)--];
    i = intermediate_index;
    
    /* Find the instruction */
    while (i > 0 && intermediate_table[i-1]->instruc_no != popped_value) {
        i--;
    }
    
    if (i <= 0) {
        fprintf(stderr, "Error: Could not find matching IF for ENDIF at line %d\n", instruction_no);
        return;
    }
    
    /* Update the false branch target */
    intermediate_table[i-1]->parameters[3] = temp + 1;
}

/**
 * @brief Processes a JUMP instruction
 * 
 * @param param Parameter for the instruction
 * @param instruction_no Current instruction number
 */
void jump_func(char *param, int instruction_no) {
    int found = 0;
    
    /* Set up instruction */
    intermediate_table[intermediate_index]->instruc_no = instruction_no;
    intermediate_table[intermediate_index]->opcode = OP_JUMP;
    
    /* Look up the target label */
    for (int i = 0; i < blocks_index; i++) {
        if (strcmp(block_tab[i]->name, param) == 0) {
            intermediate_table[intermediate_index]->parameters[0] = block_tab[i]->instr_no;
            intermediate_table[intermediate_index]->parameters[1] = -1;  /* End marker */
            found = 1;
            break;
        }
    }
    
    if (!found) {
        fprintf(stderr, "Error: Label '%s' not found for JUMP at line %d\n", param, instruction_no);
        intermediate_table[intermediate_index]->parameters[0] = 0;  /* Default to start */
        intermediate_table[intermediate_index]->parameters[1] = -1;  /* End marker */
    }
    
    intermediate_index++;
}

/**
 * @brief Main function
 * 
 * @return int Exit code
 */
int main(void) {
    int stack[STACK_SIZE], top = -1;
    int memory_array[MEMORY_SIZE];
    int memory_index = VARIABLE_MEMORY_START - 1;  /* 0 to 7 are reserved for registers */
    
    /* Allocate memory for tables */
    symbol_tab = (symbol_table**)malloc(sizeof(symbol_table*) * 25);
    if (symbol_tab == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for symbol table\n");
        return 1;
    }
    
    for (int i = 0; i < 25; i++) {
        symbol_tab[i] = (symbol_table*)malloc(sizeof(symbol_table));
        if (symbol_tab[i] == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for symbol table entry\n");
            return 1;
        }
    }
    
    intermediate_table = (intermediate_lang**)malloc(sizeof(intermediate_lang*) * 50);
    if (intermediate_table == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for intermediate table\n");
        return 1;
    }
    
    for (int i = 0; i < 50; i++) {
        intermediate_table[i] = (intermediate_lang*)malloc(sizeof(intermediate_lang));
        if (intermediate_table[i] == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for intermediate table entry\n");
            return 1;
        }
    }
    
    block_tab = (blocks_table**)malloc(sizeof(blocks_table*) * 50);
    if (block_tab == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for block table\n");
        return 1;
    }
    
    for (int i = 0; i < 50; i++) {
        block_tab[i] = (blocks_table*)malloc(sizeof(blocks_table));
        if (block_tab[i] == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for block table entry\n");
            return 1;
        }
    }
    
    /* Get input file */
    char filename[25];
    printf("Enter the filename: ");
    if (scanf("%24s", filename) != 1) {
        fprintf(stderr, "Error: Invalid filename\n");
        return 1;
    }
    
    /* Check file extension */
    char *extension = strrchr(filename, '.');
    if (extension == NULL || strcmp(extension, ".asm") != 0) {
        fprintf(stderr, "Error: File extension expected .asm, found %s\n", 
                extension ? extension : "none");
        return 1;
    }
    
    /* Open input file */
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return 1;
    }
    
    char line[LINE_SIZE];
    char tokens[10][10];
    char *buffer = (char*)malloc(10 * sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for buffer\n");
        fclose(fp);
        return 1;
    }
    
    /* Process declarations before START */
    printf("Processing declarations...\n");
    while (fgets(line, LINE_SIZE, fp)) {
        if (strcmp(line, "START:\n") == 0) {
            break;
        }
        
        int row = 0, buffer_index = 0;
        
        /* Tokenize the line */
        for (int i = 0; line[i] != '\0'; i++) {
            if (line[i] == ' ' || line[i] == '\n') {
                buffer[buffer_index] = '\0';
                buffer_index = 0;
                
                if (buffer[0] != '\0') {  /* Skip empty tokens */
                    strncpy(tokens[row++], buffer, 9);
                    tokens[row-1][9] = '\0';
                }
                
                free(buffer);
                buffer = (char*)malloc(10 * sizeof(char));
                if (buffer == NULL) {
                    fprintf(stderr, "Error: Memory allocation failed for buffer\n");
                    fclose(fp);
                    return 1;
                }
            } else {
                buffer[buffer_index++] = line[i];
            }
        }
        
        /* Process tokens */
        if (row > 0) {
            if (strcmp(tokens[0], "DATA") == 0) {
                data_func(tokens, memory_array, &memory_index);
            } else if (strcmp(tokens[0], "CONST") == 0) {
                const_func(tokens, memory_array, &memory_index);
            } else {
                fprintf(stderr, "Warning: Unknown declaration: %s\n", tokens[0]);
            }
        }
    }
    
    /* Process instructions after START */
    printf("Processing instructions...\n");
    char instruction[INSTRUCTION_LENGTH], param[PARAMETERS_LENGTH];
    int opcode = -1, instruction_no = 0;
    
    while (!feof(fp)) {
        instruction_no++;
        
        if (fgets(line, LINE_SIZE, fp) == NULL) {
            break;
        }
        
        /* Remove trailing newline */
        line[strcspn(line, "\n")] = '\0';
        
        /* Check for label */
        if (line[strlen(line) - 1] == ':') {
            line[strlen(line) - 1] = '\0';
            
            if (blocks_index >= 50) {
                fprintf(stderr, "Error: Too many labels\n");
                continue;
            }
            
            strncpy(block_tab[blocks_index]->name, line, LABEL_LENGTH - 1);
            block_tab[blocks_index]->name[LABEL_LENGTH - 1] = '\0';
            block_tab[blocks_index]->instr_no = instruction_no;
            blocks_index++;
            
            instruction_no--;  /* Label doesn't count as an instruction */
            continue;
        }
        
        /* Parse instruction and parameters */
        if (sscanf(line, "%5s %[^*]", instruction, param) != 2) {
            /* Handle instructions with no parameters */
            if (sscanf(line, "%5s", instruction) != 1) {
                fprintf(stderr, "Error: Invalid instruction format at line %d\n", instruction_no);
                continue;
            }
            param[0] = '\0';
        }
        
        opcode = generate_opcode(instruction);
        
        /* Process instruction */
        switch (opcode) {
            case OP_MOV_MEM_TO_REG:
                mov_func(param, instruction_no);
                break;
                
            case OP_ADD:
            case OP_SUB:
            case OP_MUL:
                binaryOperations_func(opcode, param, instruction_no);
                break;
                
            case OP_JUMP:
                if (strcmp(instruction, "ELSE") == 0) {
                    else_func(instruction_no, stack, &top);
                } else {
                    jump_func(param, instruction_no);
                }
                break;
                
            case OP_IF:
                if_func(param, instruction_no, stack, &top);
                break;
                
            case OP_PRINT:
                print_func(param, instruction_no);
                break;
                
            case OP_READ:
                read_func(param, instruction_no);
                break;
                
            case OP_ENDIF:
                endif_func(instruction_no, stack, &top);
                instruction_no--;  /* ENDIF doesn't generate code */
                break;
                
            case OP_END:
                goto ending;  /* End of program */
                
            default:
                fprintf(stderr, "Warning: Unknown instruction '%s' at line %d\n", 
                        instruction, instruction_no);
                break;
        }
    }
    
ending:
    /* Check for unmatched control structures */
    if (top >= 0) {
        fprintf(stderr, "Error: Unmatched IF/ELSE statements\n");
    }
    
    /* Clean up */
    free(buffer);
    fclose(fp);
    
    /* Dump intermediate code to file */
    dump_to_file();
    
    /* Execute the program */
    printf("\nExecuting program...\n");
    executor(memory_array, memory_index);
    
    /* Free allocated memory */
    for (int i = 0; i < 25; i++) {
        free(symbol_tab[i]);
    }
    free(symbol_tab);
    
    for (int i = 0; i < 50; i++) {
        free(intermediate_table[i]);
    }
    free(intermediate_table);
    
    for (int i = 0; i < 50; i++) {
        free(block_tab[i]);
    }
    free(block_tab);
    
    printf("\nPress any key to exit...\n");
    _getch();
    return 0;
}
