#include "vm/vm_core/vm_core.h"
#include "vm/vm_utils/vm_utils.h"
#include <jansson.h>

 
/* TODO:

- Create save VMState function (Save as struct or JSON???) -> put in vm_utils

- Create read VMState function

- Add version number and format string

- Write evaluation function which compares VM output to target ouput.

- Write unit tests, write program tape, and expected output tape, 
if after running the program tape the ouput tape matches the expected ouput tape, then, success

- Use circular linked list to store all VMs currently under execution. When a new VM is spawned, insert,
when one has finished, remove. After steps are exceuted for current iteration, continue execution of next element in list

*/

int save_VMState_to_json(VMState* state, const char* file_path, size_t format_flag);

int main(){
    /* The program tape is read/execute, the work tape is read/write/execute */
    /* prog_tape will be provided by program generator, this is just a place holder */

    Word prog_tape[] = {
        INS_GET_INPUT, 0, 0,
        INS_GET_INPUT, 1, 1,
        INS_ADD, 0, 1, 2,
        INS_OUTPUT, 0,
        INS_OUTPUT, 1,
        INS_OUTPUT, 2,
        INS_STOP
    };  
    

    /* output tape, input tape + input_len are provided externally */
    Word input_tape[MAX_INPUT_CELLS];    // filled before program execution
    input_tape[0] = 1;
    input_tape[1] = 3;                 
    Word output_tape[MAX_OUTPUT_CELLS] = {0};  // sequential output                


    VMState state = {
        .work_tape = {0},
        .min_mem_addr = 0,
        .max_mem_addr = INIT_WORK_CELLS-1,
        .output_tape = output_tape,
        .output_len = 0,
        .pc = 0,
        .runtime = 0,
        .halt = HALT_RUNNING,
        .prog_tape = prog_tape,
        .prog_len = 17,
        .input_tape = input_tape,
        .input_len = 2
    };

    vm_run(&state);

    print_VMState(&state);

    save_VMState_to_json(&state, "test.json", 4);

    return 0;
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


    int status = json_dump_file(obj, file_path, format_flag); 
    json_decref(obj);

    return status;
}