# Assembly Language Compiler

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

A lightweight assembly language compiler and interpreter built in C. This project implements a custom assembly language with a simplified instruction set, designed for educational purposes to demonstrate the fundamentals of compiler construction and assembly language execution.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [Instruction Set](#instruction-set)
- [Memory Model](#memory-model)
- [Project Structure](#project-structure)
- [Building the Project](#building-the-project)
- [Usage](#usage)
- [Sample Programs](#sample-programs)
- [Technical Implementation](#technical-implementation)
- [Future Improvements](#future-improvements)
- [License](#license)

## Overview

This project is a complete assembly language compiler and interpreter that:

1. Parses assembly language source code from `.asm` files
2. Performs lexical analysis to tokenize the input
3. Builds symbol tables for variables and labels
4. Generates intermediate code
5. Executes the intermediate code in a virtual machine environment

The compiler supports a custom assembly language with a simplified instruction set, making it ideal for educational purposes and understanding the fundamentals of compiler design and assembly language execution.

## Features

- **Custom Assembly Language**: Simplified instruction set with essential operations
- **Variable Declaration**: Support for scalar variables and arrays
- **Constants**: Define immutable values
- **Arithmetic Operations**: Addition, subtraction, and multiplication
- **Control Flow**: Conditional statements (IF-THEN-ELSE) and jumps
- **I/O Operations**: Basic input and output capabilities
- **Register-based Architecture**: 8 general-purpose registers
- **Symbol Table Management**: Tracks variables, their addresses, and sizes
- **Intermediate Code Generation**: Translates assembly to an intermediate representation
- **Virtual Machine Execution**: Interprets and executes the intermediate code

## Architecture

The compiler follows a three-phase architecture:

1. **Front-end**: Lexical analysis and parsing of the assembly code
2. **Middle-end**: Symbol table generation and intermediate code creation
3. **Back-end**: Execution of the intermediate code in a virtual machine

### Compilation Process

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Assembly   │     │   Lexical   │     │ Intermediate│     │   Virtual   │
│  Source     │────▶│   Analysis  │────▶│    Code     │────▶│   Machine   │
│  (.asm)     │     │ & Parsing   │     │ Generation  │     │  Execution  │
└─────────────┘     └─────────────┘     └─────────────┘     └─────────────┘
                           │                   │                   │
                           ▼                   ▼                   ▼
                    ┌─────────────┐     ┌─────────────┐     ┌─────────────┐
                    │   Token     │     │  Symbol &   │     │   Program   │
                    │   Stream    │     │   Block     │     │   Output    │
                    │             │     │   Tables    │     │             │
                    └─────────────┘     └─────────────┘     └─────────────┘
```

## Instruction Set

The compiler supports the following instructions:

### Data Declaration
- `DATA <variable>` - Declare a variable
- `DATA <variable>[size]` - Declare an array
- `CONST <variable> = <value>` - Declare a constant

### Program Structure
- `START:` - Beginning of the program
- `END` - End of the program
- `<label>:` - Define a label for jumps

### Register Operations
- `MOV <dest>, <src>` - Move value from source to destination
- `ADD <dest>, <src1>, <src2>` - Add src1 and src2, store in dest
- `SUB <dest>, <src1>, <src2>` - Subtract src2 from src1, store in dest
- `MUL <dest>, <src1>, <src2>` - Multiply src1 and src2, store in dest

### Control Flow
- `JUMP <label>` - Unconditional jump to label
- `IF <operand1> <condition> <operand2> THEN` - Conditional execution
- `ELSE` - Alternative execution path
- `ENDIF` - End of conditional block

### Conditions
- `EQ` - Equal
- `LT` - Less than
- `GT` - Greater than
- `LTEQ` - Less than or equal
- `GTEQ` - Greater than or equal

### Input/Output
- `READ <operand>` - Read input into operand
- `PRINT <operand>` - Print value of operand

## Memory Model

The memory model consists of:

- **Registers**: 8 general-purpose registers (AX, BX, CX, DX, EX, FX, GX, HX)
- **Variables**: Stored in memory starting from address 8
- **Arrays**: Contiguous memory blocks with the specified size
- **Constants**: Special variables with fixed values

Memory addresses 0-7 are reserved for the registers, and variables are allocated sequentially starting from address 8.

## Project Structure

```
Assembly-Language-Compiler/
├── Assembly_compiler/
│   ├── compiler/
│   │   ├── main.c              # Main compiler implementation
│   │   ├── executor.c          # Virtual machine implementation
│   │   ├── FunctionHeaders.h   # Common header file
│   │   ├── compiler.vcxproj    # Visual Studio project file
│   │   └── sample1.asm         # Sample assembly program
├── sample.asm                  # Sample assembly program
└── README.md                   # This file
```

## Building the Project

### Prerequisites

- Windows operating system
- Visual Studio (2013 or later)
- C compiler

### Build Steps

1. Open the solution file `Assembly_compiler/compiler.sln` in Visual Studio
2. Select the build configuration (Debug/Release)
3. Build the solution (F7 or Build > Build Solution)

## Usage

1. Run the compiled executable
2. When prompted, enter the name of the assembly file (e.g., `sample.asm`)
3. The compiler will parse the file, generate intermediate code, and execute it
4. Follow the prompts for any input required by the program
5. View the output of the program in the console

## Sample Programs

### Basic Arithmetic and Conditional Logic

```assembly
DATA B
DATA A
DATA C[4]
DATA D
CONST E = 0
START:
READ AX
READ BX
MOV C[2], AX
MOV B, BX
ADD CX, AX, BX
X:
READ AX
SUB DX, AX, BX
PRINT DX
PRINT CX
IF CX EQ DX THEN
    MOV C[0], CX
    PRINT C[0]
ELSE
    MOV C[1], DX
    PRINT C[1]
    JUMP X
ENDIF
PRINT E
END
```

This program:
1. Declares variables A, B, D, and array C with 4 elements
2. Declares constant E with value 0
3. Reads values into registers AX and BX
4. Performs arithmetic operations
5. Uses conditional logic to control program flow
6. Prints results

## Technical Implementation

### Symbol Table

The compiler maintains a symbol table to track variables, their memory addresses, and sizes:

```c
struct symbol_table {
    char variable_name[variable_length];
    int address;
    int size;
};
```

### Intermediate Language

Assembly instructions are translated into an intermediate representation:

```c
struct intermediate_lang {
    int instruc_no;
    int opcode;
    int parameters[5];
};
```

### Block Table

Labels and their corresponding instruction numbers are stored in a block table:

```c
struct blocks_table {
    char name[label_length];
    int instr_no;
};
```

### Compilation Process

1. **Lexical Analysis**: The source code is tokenized into instructions and operands
2. **Symbol Table Generation**: Variables and constants are added to the symbol table
3. **Intermediate Code Generation**: Assembly instructions are converted to opcodes and parameters
4. **Execution**: The intermediate code is executed by the virtual machine

### Virtual Machine

The virtual machine:
1. Maintains a memory array for variables and registers
2. Executes instructions based on their opcodes
3. Handles control flow through jumps and conditional execution
4. Manages input/output operations

## Future Improvements

Potential enhancements for the project:

1. **Extended Instruction Set**: Add support for more operations (DIV, MOD, bitwise operations)
2. **Function Support**: Implement subroutines with call/return semantics
3. **Error Handling**: Improve error detection and reporting
4. **Optimization**: Add basic optimization techniques
5. **Cross-Platform Support**: Make the compiler compatible with non-Windows systems
6. **Assembler/Disassembler**: Add ability to convert between binary and assembly
7. **GUI Interface**: Create a graphical interface for easier interaction
8. **Debugging Tools**: Add breakpoints, memory inspection, and step execution
