#include "vm_core.h"
#include "../types/types.h"
#include "../vm_utils/vm_utils.h"
#include "../verify/verify.h"


HaltReason vm_run(VMState* state){
    uint8_t overflow_flag = 0;

    if (state->input_len > MAX_INPUT_CELLS){
        printf("number of inputs exceeds maximum number of allowed input cells: (%zu>%d)\n", state->input_len, MAX_INPUT_CELLS);
        return -1;
    }

    /* clamp program values if values are out of bounds */
    for (size_t i=0; i<=(state->prog_len-1); i++){
        clamp_int64_to_MAXINT(state->prog_tape[i], &overflow_flag);    // omit return value, only care about detecting overflow, not correcting it
        if(overflow_flag == 1){
            state->halt = HALT_FAILURE_OVERFLOW;
            break;
        }
    }

    while (state->halt == HALT_RUNNING){
        vm_fetch_execute(state, &overflow_flag);
    }


    return state->halt;
}

HaltReason vm_step(VMState* state){
    uint8_t overflow_flag = 0;

    if (state->input_len > MAX_INPUT_CELLS){
        printf("number of inputs exceeds maximum number of allowed input cells: (%zu>%d)\n", state->input_len, MAX_INPUT_CELLS);
        return -1;
    }

    /* clamp program values if values are out of bounds */
    for (size_t i=0; i<=(state->prog_len-1); i++){
        clamp_int64_to_MAXINT(state->prog_tape[i], &overflow_flag);    // omit return value, only care about detecting overflow, not correcting it
        if(overflow_flag == 1){
            state->halt = HALT_FAILURE_OVERFLOW;
            break;
        }
    }

    if (state->halt == HALT_RUNNING){
        vm_fetch_execute(state, &overflow_flag);
    }


    return state->halt;
}

void vm_fetch_execute(VMState* state, uint8_t* overflow_flag){
    int64_t tmp;
    Word mem_requested;
    VerifiedInsn insn;

    if (++state->runtime > TIMELIMIT) {
        // time limit exceeded --> halt
        state->halt = HALT_FAILURE_TIME;
        return ;
    }
    state->ver_res=verify_insn(state->prog_tape, state->prog_len, state->input_len, state->pc, state->min_mem_addr, state->max_mem_addr, &insn);
    if (state->ver_res != VERIFY_SUCCESS){     // instruction verification
        state->halt = HALT_FAILURE_VERIFY;
        return ;
    }
    
    switch (insn.op){
        case INS_JUMPLEQ:
            
            if (state->work_tape[insn.args[0]] <= state->work_tape[insn.args[1]]){
                state->pc = insn.args[2];
            }
            else{
                state->pc += insn.arity+1;
            }
            
            break;

        case INS_OUTPUT:

            if (state->output_len < MAX_OUTPUT_CELLS){
                state->output_tape[state->output_len++] = state->work_tape[insn.args[0]];
                state->pc += insn.arity + 1;
            }
            else{
                state->halt = HALT_FAILURE_OTHER;      // output buffer full
            }
            
            break;

        case INS_JUMP:

            state->pc = insn.args[0];     
            break;

        case INS_STOP:

            state->halt = HALT_SUCCESS;
            break;

        case INS_ADD:

            tmp = (int64_t)state->work_tape[insn.args[0]] + (int64_t)state->work_tape[insn.args[1]];
            state->work_tape[insn.args[2]] = clamp_int64_to_MAXINT(tmp, overflow_flag);
            state->pc += insn.arity+1;
            
            break;

        case INS_GET_INPUT:

            tmp = (int64_t)state->input_tape[insn.args[0]];
            state->work_tape[insn.args[1]] = clamp_int64_to_MAXINT(tmp, overflow_flag);       // should not happen, ensure beforehand that input tape values are in valid range
            state->pc += insn.arity+1;
            
            break;

        case INS_MOVE:

            state->work_tape[insn.args[1]] = state->work_tape[insn.args[0]];
            state->pc += insn.arity+1;
            
            break;

        case INS_ALLOCATE:

            mem_requested = state->work_tape[insn.args[0]];
            if ((mem_requested >= 0) && (mem_requested <= MAXMEMALLOC) && ((state->max_mem_addr+(size_t)mem_requested) < (MAX_WORK_CELLS))){   // semantic constraint: check VM policy violation
                state->max_mem_addr += mem_requested;   // update max_addr
                if (state->max_mem_addr < state->min_mem_addr) {  // state invariant: ensure VM state correctness
                    state->halt = HALT_FAILURE_ADDR;
                    break;
                }

                state->pc += insn.arity+1;
            }
            else{   // if allocated memory would exceed memory bounds, halt
                state->halt = HALT_FAILURE_ADDR;
            }
            
            break;

        case INS_INCREMENT:

            tmp = (int64_t)state->work_tape[insn.args[0]] + 1;
            state->work_tape[insn.args[0]] = clamp_int64_to_MAXINT(tmp, overflow_flag);
            state->pc += insn.arity+1;

            break;

        case INS_DECREMENT:

            tmp = (int64_t)state->work_tape[insn.args[0]] - 1;
            state->work_tape[insn.args[0]] = clamp_int64_to_MAXINT(tmp, overflow_flag);
            state->pc += insn.arity+1;
            
            break;

        case INS_SUBTRACT:

            tmp = (int64_t)state->work_tape[insn.args[1]] - (int64_t)state->work_tape[insn.args[0]];
            state->work_tape[insn.args[2]] = clamp_int64_to_MAXINT(tmp, overflow_flag);
            state->pc += insn.arity+1;

            break;

        case INS_MULTIPLY:
                
            tmp = (int64_t)state->work_tape[insn.args[0]] * (int64_t)state->work_tape[insn.args[1]];
            state->work_tape[insn.args[2]] = clamp_int64_to_MAXINT(tmp, overflow_flag);
            state->pc += insn.arity+1;

            break;

        case INS_FREE:

            mem_requested = state->work_tape[insn.args[0]];
            if ((mem_requested >= 0) && (mem_requested <= MAXMEMALLOC) && ((state->max_mem_addr-(size_t)mem_requested) >= (state->min_mem_addr))){   // semantic constraint: check VM policy violation
                state->max_mem_addr -= mem_requested;   // update max_addr
                if (state->max_mem_addr < state->min_mem_addr) {  // state invariant: ensure VM state correctness
                    state->halt = HALT_FAILURE_ADDR;
                    break;
                }
                state->pc += insn.arity+1;
            }
            else{   // if allocated memory would exceed memory bounds, halt
                state->halt = HALT_FAILURE_ADDR;
            }
            
            break;
            
        default:
            state->halt = HALT_FAILURE_OPCODE;
            break;
    }

}
