#ifndef VM_UTILS_H
#define VM_UTILS_H

#include "../types/types.h"

Word clamp_int64_to_MAXINT(int64_t value, uint8_t* overflow_flag);

#endif