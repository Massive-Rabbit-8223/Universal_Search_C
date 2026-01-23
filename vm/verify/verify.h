#ifndef VERIFY_H
#define VERIFY_H

#include "../types/types.h"

VerifyResult map_fetch_error(FetchResult fr, int is_prog_addr);
VerifyResult verify_insn(const Word* prog_tape, size_t prog_len, size_t input_len, size_t pc, size_t min_mem_addr, size_t max_mem_addr, VerifiedInsn* out);
FetchResult fetch_input_addr(const Word* prog_tape, size_t prog_len, size_t input_len, size_t pc, size_t* out_addr);
FetchResult fetch_mem_addr(const Word* prog_tape, size_t prog_len, size_t pc, size_t min_mem_addr, size_t max_mem_addr, size_t* out_addr); // fetch_mem_addr: verifies writable/readable work-tape address
FetchResult fetch_prog_addr(const Word* prog_tape, size_t prog_len, size_t pc, size_t* out_addr);   // fetch_prog_addr: verifies read-only program address

#endif