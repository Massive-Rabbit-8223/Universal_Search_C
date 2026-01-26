#include "vm/vm_core/vm_core.h"
#include "vm/vm_utils/vm_utils.h"

 
/* TODO:

- Create print VMState function

- Create save VMState function (Save as struct or JSON???)

- Write unit tests, write program tape, and expected output tape, 
if after running the program tape the ouput tape matches the expected ouput tape, then, success

- Use circular linked list to store all VMs currently under execution. When a new VM is spawned, insert,
when one has finished, remove. After steps are exceuted for current iteration, continue execution of next element in list

*/



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
    Word output_tape[MAX_OUTPUT_CELLS];  // sequential output                


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

    HaltReason r;
    r = vm_run(&state);

    printf("halt reason: %d\n", r);
    printf("verfication result: %d\n", state.ver_res);

    printf("input: ");
    print_tape(state.input_tape, state.input_len);

    printf("program: ");
    print_prog_state(state.prog_tape, state.prog_len, state.pc);

    printf("work: ");
    print_tape(state.work_tape, state.max_mem_addr+1);

    printf("output: ");
    print_tape(state.output_tape, state.output_len);


    return 0;
}
