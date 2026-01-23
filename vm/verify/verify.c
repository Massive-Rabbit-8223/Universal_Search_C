#include "verify.h"
#include "../types/types.h"

VerifyResult verify_insn(const Word* prog_tape, size_t prog_len, size_t input_len, size_t pc, size_t min_mem_addr, size_t max_mem_addr, VerifiedInsn* out){
    if (pc >= prog_len) return VERIFY_FAILURE_PC_OOB;

    Word raw = prog_tape[pc];

    if (raw < 0 || raw >= INS_COUNT) return VERIFY_FAILURE_OPCODE;

    Opcode op = (Opcode) raw;

    uint8_t arity = instruction_arity[op];

    if (pc+arity >= prog_len) return VERIFY_FAILURE_PC_OOB;

    out->op = op;
    out->arity = arity;

    FetchResult fr;

    if (op == INS_JUMP){
        fr = fetch_prog_addr(prog_tape, prog_len, pc+1, &out->args[0]); 
        if (fr != FETCH_OK) return map_fetch_error(fr, 1);
    }
    else if (op == INS_JUMPLEQ){
        fr = fetch_mem_addr(prog_tape, prog_len, pc+1, min_mem_addr, max_mem_addr, &out->args[0]); 
        if (fr != FETCH_OK) return map_fetch_error(fr, 0);

        fr = fetch_mem_addr(prog_tape, prog_len, pc+2, min_mem_addr, max_mem_addr, &out->args[1]);
        if (fr != FETCH_OK) return map_fetch_error(fr, 0);

        fr = fetch_prog_addr(prog_tape, prog_len, pc+3, &out->args[2]);
        if (fr != FETCH_OK) return map_fetch_error(fr, 1);
    }
    else if (op == INS_GET_INPUT){
        fr = fetch_input_addr(prog_tape, prog_len, input_len, pc+1, &out->args[0]);
        if (fr != FETCH_OK) return map_fetch_error(fr, 2);

        fr = fetch_mem_addr(prog_tape, prog_len, pc+2, min_mem_addr, max_mem_addr, &out->args[1]);
        if (fr != FETCH_OK) return map_fetch_error(fr, 0);
    }
    else{
        for (uint8_t i=0; i<arity; i++){
            fr = fetch_mem_addr(prog_tape, prog_len, pc+i+1, min_mem_addr, max_mem_addr, &out->args[i]);
            if (fr != FETCH_OK) return map_fetch_error(fr, 0);
        }
    }

    return VERIFY_SUCCESS;
}

FetchResult fetch_input_addr(const Word* prog_tape, size_t prog_len, size_t input_len, size_t pc, size_t* out_addr){
    /* Check program tape bounds */
    if (pc >= prog_len) return FETCH_PC_OOB;

    Word raw = prog_tape[pc];

    /* Reject negative addresses */
    if (raw < 0) return FETCH_NEGATIVE;

    size_t addr = (size_t) raw;

    /* Enforce prog tape bounds */
    if (addr >= input_len) return FETCH_ADDR_OOB;

    *out_addr = addr;
    return FETCH_OK;

}

FetchResult fetch_mem_addr(const Word* prog_tape, size_t prog_len, size_t pc, size_t min_mem_addr, size_t max_mem_addr, size_t* out_addr){
    /* Check program tape bounds */
    if (pc >= prog_len) return FETCH_PC_OOB;

    Word raw = prog_tape[pc];

    /* Reject negative addresses */
    if (raw < 0) return FETCH_NEGATIVE;

    size_t addr = (size_t) raw;

    /* Enforce work tape bounds */
    if (addr < min_mem_addr || addr > max_mem_addr) return FETCH_ADDR_OOB;

    *out_addr = addr;
    return FETCH_OK;
}

FetchResult fetch_prog_addr(const Word* prog_tape, size_t prog_len, size_t pc, size_t* out_addr){
    /* Check program tape bounds */
    if (pc >= prog_len) return FETCH_PC_OOB;

    Word raw = prog_tape[pc];

    /* Reject negative addresses */
    if (raw < 0) return FETCH_NEGATIVE;

    size_t addr = (size_t) raw;

    /* Enforce prog tape bounds */
    if (addr >= prog_len) return FETCH_ADDR_OOB;

    *out_addr = addr;
    return FETCH_OK;
}

VerifyResult map_fetch_error(FetchResult fr, int addr_type) {
    switch (fr) {
        case FETCH_OK:
            return VERIFY_SUCCESS;
        case FETCH_PC_OOB:
            return VERIFY_FAILURE_PC_OOB;
        case FETCH_NEGATIVE:
            return VERIFY_FAILURE_NEGATIVE_ADDR;
        case FETCH_ADDR_OOB:
            switch (addr_type) {
                case 0: return VERIFY_FAILURE_MEM_ADDR_OOB;
                case 1: return VERIFY_FAILURE_PROG_ADDR_OOB;
                case 2: return VERIFY_FAILURE_INPUT_ADDR_OOB;
                default: return VERIFY_FAILURE_ADDR_OOB;
            }
        default:
            return VERIFY_FAILURE_ADDR_OOB; // fallback, should not happen
    }
}