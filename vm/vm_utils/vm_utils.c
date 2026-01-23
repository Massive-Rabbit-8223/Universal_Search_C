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