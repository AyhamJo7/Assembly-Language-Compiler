/**
 * @file FunctionHeaders.h
 * @brief Common header file for the Assembly Language Compiler
 * 
 * This header defines the data structures and function prototypes used
 * throughout the compiler and virtual machine implementation.
 * 
 * @copyright Copyright (c) 2017 Komma Ravi Teja
 * @license MIT License
 */

#ifndef FUNCTION_HEADERS_H
#define FUNCTION_HEADERS_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

/**
 * @defgroup MemoryConstants Memory Configuration Constants
 * @{
 */
#define STACK_SIZE 100              /**< Maximum stack size for nested control structures */
#define MEMORY_SIZE 100             /**< Total memory size for the virtual machine */
#define VARIABLE_MEMORY_START 8     /**< Starting address for variables (0-7 reserved for registers) */
#define CONST_VARIABLE_SIZE 0       /**< Size indicator for constants */
/** @} */

/**
 * @defgroup ParsingConstants Parsing Configuration Constants
 * @{
 */
#define INSTRUCTION_LENGTH 6        /**< Maximum length of instruction mnemonics */
#define PARAMETERS_LENGTH 25        /**< Maximum length of instruction parameters */
#define LINE_SIZE 25                /**< Maximum length of a line in the source file */
#define VARIABLE_LENGTH 5           /**< Maximum length of variable names */
#define LABEL_LENGTH 5              /**< Maximum length of label names */
/** @} */

/**
 * @defgroup SpecialValues Special Values
 * @{
 */
#define WILDCARD_VALUE -2           /**< Special value representing a wildcard (*) */
/** @} */

/**
 * @defgroup OpCodes Instruction OpCodes
 * @{
 */
#define OP_MOV_MEM_TO_REG 1         /**< Move from memory to register */
#define OP_MOV_REG_TO_MEM 2         /**< Move from register to memory */
#define OP_ADD 3                    /**< Addition operation */
#define OP_SUB 4                    /**< Subtraction operation */
#define OP_MUL 5                    /**< Multiplication operation */
#define OP_JUMP 6                   /**< Unconditional jump */
#define OP_IF 7                     /**< Conditional execution */
#define OP_EQ 8                     /**< Equal condition */
#define OP_LT 9                     /**< Less than condition */
#define OP_GT 10                    /**< Greater than condition */
#define OP_LTEQ 11                  /**< Less than or equal condition */
#define OP_GTEQ 12                  /**< Greater than or equal condition */
#define OP_PRINT 13                 /**< Print operation */
#define OP_READ 14                  /**< Read operation */
#define OP_ENDIF 15                 /**< End of conditional block */
#define OP_END 16                   /**< End of program */
/** @} */

/**
 * @struct intermediate_lang
 * @brief Represents an instruction in the intermediate language
 * 
 * This structure stores the compiled form of an assembly instruction,
 * including its opcode and parameters.
 */
typedef struct {
    int instruc_no;                 /**< Instruction number (line number) */
    int opcode;                     /**< Operation code */
    int parameters[5];              /**< Instruction parameters (addresses, values, etc.) */
} intermediate_lang;

/**
 * @struct symbol_table
 * @brief Represents an entry in the symbol table
 * 
 * This structure stores information about variables and their memory allocation.
 */
typedef struct {
    char variable_name[VARIABLE_LENGTH]; /**< Name of the variable */
    int address;                    /**< Memory address of the variable */
    int size;                       /**< Size of the variable (1 for scalar, >1 for arrays) */
} symbol_table;

/**
 * @struct blocks_table
 * @brief Represents an entry in the blocks table
 * 
 * This structure stores information about labels and their corresponding
 * instruction numbers for implementing jumps.
 */
typedef struct {
    char name[LABEL_LENGTH];        /**< Name of the label */
    int instr_no;                   /**< Instruction number after the label */
} blocks_table;

/**
 * @brief Displays the contents of the symbol table
 * 
 * This function prints the variable names, addresses, and sizes
 * from the symbol table to the console.
 */
void display_symbol_table(void);

/**
 * @brief Displays the contents of the intermediate language table
 * 
 * This function prints the instruction numbers, opcodes, and parameters
 * from the intermediate language table to the console.
 */
void display_intermediate_table(void);

/**
 * @brief Displays the contents of the blocks table
 * 
 * This function prints the label names and instruction numbers
 * from the blocks table to the console.
 */
void display_block_table(void);

/**
 * @brief Writes the compiler output to a file
 * 
 * This function dumps the symbol table, blocks table, and intermediate
 * language table to an output file.
 */
void dump_to_file(void);

/**
 * @brief Executes the compiled program
 * 
 * This function runs the virtual machine that executes the
 * intermediate language instructions.
 * 
 * @param memory_array Pointer to the memory array
 * @param memory_index Index of the last used memory location
 */
void executor(int *memory_array, int memory_index);

#endif /* FUNCTION_HEADERS_H */
