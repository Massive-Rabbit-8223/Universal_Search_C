#include "vm_utils.h"
#include "../types/types.h"

Word clamp_int64_to_MAXINT(int64_t value, uint8_t* overflow_flag){
    if (value < -MAXINT){
        *overflow_flag = 1;
        return -MAXINT;
    }
    else if (value > MAXINT){
        *overflow_flag = 1;
        return MAXINT;
    }
    else{
        *overflow_flag = 0;
        return (Word)value;
    }
}

void print_tape(const Word* tape, size_t tape_len){
    printf("[");
    for(int i=0; i<tape_len; i++){
        printf("%" PRId32, tape[i]);
        if ((i+1) < tape_len){
            printf(", ");
        }
    }
    printf("]\n");
}

void print_prog_state(const Word* tape, size_t tape_len, size_t pc){
    printf("[");
    for(int i=0; i<tape_len; i++){
        if (i == pc) printf("(%" PRId32 ")", tape[i]);
        else printf("%" PRId32, tape[i]); 
        if ((i+1) < tape_len){
            printf(", ");
        }
    }
    printf("]\n");
}

void print_VMState(const VMState* state){

    printf("halt reason: %d\n", state->halt);
    printf("verfication result: %d\n", state->ver_res);

    printf("input: ");
    print_tape(state->input_tape, state->input_len);

    printf("program: ");
    print_prog_state(state->prog_tape, state->prog_len, state->pc);

    printf("work: ");
    print_tape(state->work_tape, state->max_mem_addr+1);

    printf("output: ");
    print_tape(state->output_tape, state->output_len);
}