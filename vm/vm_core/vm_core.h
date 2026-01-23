#ifndef VM_H
#define VM_H

#include "../types/types.h"


HaltReason vm_run(VMState* state, const VMInput* io);
HaltReason vm_step(VMState* state, const VMInput* io);
void vm_fetch_execute(VMState* state, const VMInput* io, uint8_t* overflow_flag);

#endif