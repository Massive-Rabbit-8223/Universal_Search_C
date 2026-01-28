#include "vm_utils.h"
#include "../types/types.h"
#include <jansson.h>

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

    for(int i=0; i < MAX_WORK_CELLS; i++){
        json_array_insert_new(work_arr, i, json_integer(state->work_tape[i]));
    }
    json_object_set_new(obj, "work_tape", work_arr);

    json_object_set_new(obj, "min_mem_addr", json_integer(state->min_mem_addr));
    json_object_set_new(obj, "max_mem_addr", json_integer(state->max_mem_addr));

    for(int i=0; i < MAX_OUTPUT_CELLS; i++){
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

    for(int i=0; i < MAX_INPUT_CELLS; i++){
        json_array_insert_new(in_arr, i, json_integer(state->input_tape[i]));
    }
    json_object_set_new(obj, "input_tape", in_arr);

    json_object_set_new(obj, "input_len", json_integer(state->input_len));
    json_object_set_new(obj, "version", json_integer(state->version));
    json_object_set_new(obj, "format", json_string(state->format));


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

    
    json_t *arr = json_object_get(obj, "work_tape");
    for (size_t i=0; i<json_array_size(arr); i++){
        state->work_tape[i] = (Word) json_integer_value(json_array_get(arr, i));
    }

    arr = json_object_get(obj, "output_tape");
    Word output_tape[json_array_size(arr)];
    for (size_t i=0; i<json_array_size(arr); i++){
        output_tape[i] = (Word) json_integer_value(json_array_get(arr, i));
    }
    state->output_tape = output_tape;
    
    arr = json_object_get(obj, "prog_tape");
    Word prog_tape[json_array_size(arr)];
    for (size_t i=0; i<json_array_size(arr); i++){
        prog_tape[i] = (Word) json_integer_value(json_array_get(arr, i));
    }
    state->prog_tape = prog_tape;

    arr = json_object_get(obj, "input_tape");
    Word input_tape[json_array_size(arr)];
    for (size_t i=0; i<json_array_size(arr); i++){
        input_tape[i] = (Word) json_integer_value(json_array_get(arr, i));
    }
    state->input_tape = input_tape;

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