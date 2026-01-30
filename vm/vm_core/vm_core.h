#ifndef VM_H
#define VM_H

#include "../types/types.h"


HaltReason vm_run(VMState* state);
HaltReason vm_step(VMState* state);
void vm_fetch_execute(VMState* state, uint8_t* overflow_flag);
Word clamp_int64_to_MAXINT(int64_t value, uint8_t* overflow_flag);

#endif