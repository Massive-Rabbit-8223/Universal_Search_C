#ifndef VM_UTILS_H
#define VM_UTILS_H

#include "../types/types.h"

Word clamp_int64_to_MAXINT(int64_t value, uint8_t* overflow_flag);
void print_tape(const Word* tape, size_t tape_len);
void print_prog_state(const Word* tape, size_t tape_len, size_t pc);
void print_VMState(const VMState* state);
int save_VMState_to_json(VMState* state, const char* file_path, size_t format_flag);
int load_VMState_from_json(VMState* state, const char* file_path, size_t flag);
int eval_output(Word* target_output, Word* vm_output, size_t target_output_len, size_t vm_output_len, int strict);

#endif