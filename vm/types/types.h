#ifndef TYPES_H
#define TYPES_H

#include <limits.h>
#include <inttypes.h>   // contains macros that expand to format specifiers for the fixed-width types
#include <stdio.h>


#define MAXINT 10000    
#define INIT_WORK_CELLS 8
#define MAX_WORK_CELLS  1024
#define TIMELIMIT 1000
#define MAXMEMALLOC 5

#define MAX_INPUT_CELLS 256
#define MAX_OUTPUT_CELLS 256

#define VM_VERSION 1

enum {
    INS_JUMPLEQ,        //No. Args: 3,	If Value in Address 1 <= Value in Address 2, Jump to Address 3
    INS_OUTPUT,         //No. Args: 1,	Write the contents of Address 1 to output
    INS_JUMP,           //No. Args: 1,	Jump to Address 1
    INS_STOP,           //No. Args: 0,	Halt
    INS_ADD,            //No. Args: 3,	The sum of contents of Address 1 and Address 2 is written to Address 3
    INS_GET_INPUT,      //No. Args: 2,	Get the (Address 1)-th value of input and write to Address 2
    INS_MOVE,           //No. Args: 2,	Copy contents of Address 1 to Address 2
    INS_ALLOCATE,       //No. Args: 1,	Allocate (Content 1) new memory addresses
    INS_INCREMENT,      //No. Args: 1,	(Address 1) + 1
    INS_DECREMENT,      //No. Args: 1,	(Address 1) - 1
    INS_SUBTRACT,       //No. Args: 3,	The subtraction of Address 1 from Address 2 is written to Address 3
    INS_MULTIPLY,       //No. Args: 3,	The product of contents of Address 1 and Address 2 is written to Address 3
    INS_FREE,           //No. Args: 1,	Free / Deallocate (Content 1) memory addresses
    INS_COUNT
};

/* 1. Ensure MAXINT fits into Word (int32_t) */
_Static_assert(MAXINT < INT32_MAX, "MAXINT must be smaller than INT32_MAX");

/* 2. Ensure Word can safely hold all possible Opcode values */
_Static_assert(INT32_MAX >= INS_COUNT, "Word must be able to represent all opcodes");

/* 3. Ensure Word can safely be cast to size_t (program counter / addresses) */
_Static_assert(INT32_MAX <= SIZE_MAX, "Word must fit into size_t for safe casting");

/* 4. Ensure MAX_WORK_CELLS fits into int (for indexing or loops using int) */
_Static_assert(MAX_WORK_CELLS <= INT32_MAX, "MAX_WORK_CELLS must fit into int");

static const uint8_t instruction_arity[INS_COUNT] = {
    [INS_JUMPLEQ]   = 3,
    [INS_OUTPUT]    = 1,
    [INS_JUMP]      = 1,
    [INS_STOP]      = 0,
    [INS_ADD]       = 3,
    [INS_GET_INPUT] = 2,
    [INS_MOVE]      = 2,
    [INS_ALLOCATE]  = 1,
    [INS_INCREMENT] = 1,
    [INS_DECREMENT] = 1,
    [INS_SUBTRACT]  = 3,
    [INS_MULTIPLY]  = 3,
    [INS_FREE]      = 1
};

typedef int32_t Word;
typedef uint8_t Opcode;

typedef struct {
    Opcode op;
    size_t args[3];
    uint8_t arity;
} VerifiedInsn;

typedef struct{
    Word cells[MAX_OUTPUT_CELLS];
    size_t index;
} Output;

typedef enum {
    VERIFY_SUCCESS,
    VERIFY_FAILURE_PC_OOB,
    VERIFY_FAILURE_OPCODE,
    VERIFY_FAILURE_NEGATIVE_ADDR,
    VERIFY_FAILURE_MEM_ADDR_OOB,
    VERIFY_FAILURE_PROG_ADDR_OOB,
    VERIFY_FAILURE_INPUT_ADDR_OOB,
    VERIFY_FAILURE_ADDR_OOB         // fallback, should not happen
} VerifyResult;

typedef enum {
    FETCH_OK,            // success
    FETCH_PC_OOB,        // program counter out of bounds
    FETCH_NEGATIVE,      // negative address encountered
    FETCH_ADDR_OOB       // address outside allowed memory range
} FetchResult;

typedef enum {
    HALT_RUNNING = 0,
    HALT_SUCCESS,        // HALT_SUCCESS means program terminated normally via INS_STOP
    HALT_FAILURE_OPCODE,
    HALT_FAILURE_ADDR,
    HALT_FAILURE_TIME,
    HALT_FAILURE_VERIFY,
    HALT_FAILURE_OVERFLOW,
    HALT_FAILURE_OTHER,
} HaltReason;

typedef struct {
    Word work_tape[MAX_WORK_CELLS];
    size_t min_mem_addr;
    size_t max_mem_addr;
    Word* output_tape;
    size_t output_len;      // current number of output values
    size_t pc;
    int runtime;
    HaltReason halt;
    VerifyResult ver_res;
    const Word* prog_tape;
    size_t prog_len;
    const Word* input_tape;
    size_t input_len;       // number of valid input values
    int version;
    const char* format;
} VMState;


#endif