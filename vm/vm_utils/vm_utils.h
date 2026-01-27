#ifndef VM_UTILS_H
#define VM_UTILS_H

#include "../types/types.h"

Word clamp_int64_to_MAXINT(int64_t value, uint8_t* overflow_flag);
void print_tape(const Word* tape, size_t tape_len);
void print_prog_state(const Word* tape, size_t tape_len, size_t pc);
void print_VMState(const VMState* state);

#endif