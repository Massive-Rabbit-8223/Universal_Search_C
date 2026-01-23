#include "vm/vm_core/vm_core.h"


void print_tape(const Word* tape, size_t tape_len);
void print_prog_state(const Word* tape, size_t tape_len, size_t pc);

int main(){
    /* prog_tape will be provided by program generator, this is just a place holder */
    //size_t prog_len = 10;
    //Word prog_tape[prog_len];
    Word prog_tape[] = {
        INS_GET_INPUT, 0, 0,
        INS_GET_INPUT, 1, 1,
        INS_ADD, 0, 1, 2,
        INS_OUTPUT, 0,
        INS_OUTPUT, 1,
        INS_OUTPUT, 2,
        INS_STOP
    };  
    //Word work_tape[MAX_WORK_CELLS] = {0};
    /* The program tape is read/execute, the work tape is read/write/execute */

    /* input tape + input_len are provided externally */
    Word input_tape[MAX_INPUT_CELLS];    // filled before program execution
    input_tape[0] = 1;
    input_tape[1] = 3;
    //size_t input_len = 0;                 // number of valid input values

    Word output_tape[MAX_OUTPUT_CELLS];  // sequential output
    //size_t output_len = 0;                // current number of output values


       
    //size_t min_mem_addr = 0;        
    //size_t max_mem_addr = INIT_WORK_CELLS-1;       

    /* Legal Addresses are in the range [min_addr, max_addr] */

    //size_t pc = 0;                  // program counter
    //int current_run_time = 0;
    
    VMState state = {
        .work_tape = {0},
        .min_mem_addr = 0,
        .max_mem_addr = INIT_WORK_CELLS-1,
        .output_tape = output_tape,
        .output_len = 0,
        .pc = 0,
        .runtime = 0,
        .halt = HALT_RUNNING,
    };

    VMInput io = {
        .prog_tape = prog_tape,
        .prog_len = 17,
        .input_tape = input_tape,
        .input_len = 2
    };

    HaltReason r;
    r = vm_run(&state, &io);

    printf("halt reason: %d\n", r);
    printf("verfication result: %d\n", state.ver_res);
    /* implement vm_step */
    printf("input: ");
    print_tape(io.input_tape, io.input_len);

    printf("program: ");
    print_prog_state(io.prog_tape, io.prog_len, state.pc);

    printf("work: ");
    print_tape(state.work_tape, state.max_mem_addr+1);

    printf("output: ");
    print_tape(state.output_tape, state.output_len);


    return 0;
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




