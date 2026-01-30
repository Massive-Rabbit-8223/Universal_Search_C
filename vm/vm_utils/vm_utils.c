#include "vm_utils.h"
#include "../types/types.h"
#include <jansson.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>



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

int save_VMState_to_json(VMState* state, const char* file_path, size_t format_flag){
    json_t *obj = json_object();
    json_t *work_arr = json_array();
    json_t *out_arr = json_array();
    json_t *prog_arr = json_array();
    json_t *in_arr = json_array();

    for(int i=0; i < (state->max_mem_addr+1); i++){
        json_array_insert_new(work_arr, i, json_integer(state->work_tape[i]));
    }
    json_object_set_new(obj, "work_tape", work_arr);

    json_object_set_new(obj, "min_mem_addr", json_integer(state->min_mem_addr));
    json_object_set_new(obj, "max_mem_addr", json_integer(state->max_mem_addr));

    for(int i=0; i < state->output_len; i++){
        json_array_insert_new(out_arr, i, json_integer(state->output_tape[i]));
    }
    json_object_set_new(obj, "output_tape", out_arr);

    json_object_set_new(obj, "output_len", json_integer(state->output_len));
    json_object_set_new(obj, "pc", json_integer(state->pc));
    json_object_set_new(obj, "runtime", json_integer(state->runtime));
    json_object_set_new(obj, "halt", json_integer(state->halt));
    json_object_set_new(obj, "ver_res", json_integer(state->ver_res));

    for(int i=0; i < state->prog_len; i++){
        json_array_insert_new(prog_arr, i, json_integer(state->prog_tape[i]));
    }
    json_object_set_new(obj, "prog_tape", prog_arr);

    json_object_set_new(obj, "prog_len", json_integer(state->prog_len));

    for(int i=0; i < state->input_len; i++){
        json_array_insert_new(in_arr, i, json_integer(state->input_tape[i]));
    }
    json_object_set_new(obj, "input_tape", in_arr);

    json_object_set_new(obj, "input_len", json_integer(state->input_len));
    json_object_set_new(obj, "version", json_integer(state->version));
    json_object_set_new(obj, "format", json_string(state->format));
    json_object_set_new(obj, "id", json_integer(state->id));


    int status = json_dump_file(obj, file_path, format_flag); 
    json_decref(obj);

    return status;
}

int load_VMState_from_json(VMState* state, const char* file_path, size_t flag){
    json_error_t err = {0};
    json_t *obj = json_object();
    obj = json_load_file(file_path, flag, &err);

    state->pc = (size_t) json_integer_value(json_object_get(obj, "pc"));
    state->min_mem_addr = (size_t) json_integer_value(json_object_get(obj, "min_mem_addr"));
    state->max_mem_addr = (size_t) json_integer_value(json_object_get(obj, "max_mem_addr"));
    state->output_len = (size_t) json_integer_value(json_object_get(obj, "output_len"));
    state->runtime = (int) json_integer_value(json_object_get(obj, "runtime"));
    state->halt = (HaltReason) json_integer_value(json_object_get(obj, "halt"));
    state->ver_res = (VerifyResult) json_integer_value(json_object_get(obj, "ver_res"));
    state->prog_len = (size_t) json_integer_value(json_object_get(obj, "prog_len"));
    state->input_len = (size_t) json_integer_value(json_object_get(obj, "input_len"));
    state->version = (int) json_integer_value(json_object_get(obj, "version"));
    state->format = (const char*) json_string_value(json_object_get(obj, "format"));
    state->id = (int) json_integer_value(json_object_get(obj, "id"));

    
    json_t *arr = json_object_get(obj, "work_tape");
    for (size_t i=0; i<json_array_size(arr); i++){
        state->work_tape[i] = (Word) json_integer_value(json_array_get(arr, i));
    }

    arr = json_object_get(obj, "output_tape");
    //Word output_tape[json_array_size(arr)];
    for (size_t i=0; i<json_array_size(arr); i++){
        state->output_tape[i] = (Word) json_integer_value(json_array_get(arr, i));
    }
    //state->output_tape = output_tape;
    
    arr = json_object_get(obj, "prog_tape");
    //Word prog_tape[json_array_size(arr)];
    for (size_t i=0; i<json_array_size(arr); i++){
        state->prog_tape[i] = (Word) json_integer_value(json_array_get(arr, i));
    }
    //state->prog_tape = prog_tape;   // dangling pointer !!!

    arr = json_object_get(obj, "input_tape");
    //Word input_tape[json_array_size(arr)];
    for (size_t i=0; i<json_array_size(arr); i++){
        state->input_tape[i] = (Word) json_integer_value(json_array_get(arr, i));
    }
    //state->input_tape = input_tape;

    return 1;
}

int eval_output(Word* target_output, Word* vm_output, size_t target_output_len, size_t vm_output_len, int strict){
    if (vm_output_len < target_output_len) return 0;

    if ((strict==1) && (vm_output_len != target_output_len)) return 0;
    
    for (int i=0; i < target_output_len; i++){
        if (vm_output[i] != target_output[i]) return 0;
    }

    return 1;
}

bool vm_init(VMState *state, const Word *program, size_t prog_len, const Word *inputs, size_t input_len, int id){
    /* Validate required pointers */
    if (!state || !program) {
        return false;
    }

    /* Reject oversized input early */
    if (input_len > MAX_INPUT_CELLS) {
        return false;
    }

    if (prog_len > MAX_PROG_CELLS){
        return false;
    }

    /* Clear entire state to known zero baseline
       (important for determinism and security) */
    memset(state, 0, sizeof *state);

    memcpy(state->input_tape, inputs, input_len * sizeof *inputs);
    memcpy(state->prog_tape, program, prog_len * sizeof *program);

    /* Initialize execution metadata */
    state->min_mem_addr = 0;
    state->max_mem_addr = INIT_WORK_CELLS - 1;

    
    state->prog_len  = prog_len;
    state->input_len  = input_len;
    state->output_len = 0;

    state->pc      = 0;
    state->runtime = 0;
    state->halt    = HALT_RUNNING;

    state->version = VM_VERSION;
    state->format  = "vm_state";
    state->id = id;

    return true;
}