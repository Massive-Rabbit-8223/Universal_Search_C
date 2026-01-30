#include "vm/vm_core/vm_core.h"
#include "vm/vm_utils/vm_utils.h"


 
/* TODO:


- create tests folder inside vm folder with tests.c and tests.h file

- Write unit tests, write program tape, and expected output tape, 
if after running the program tape the ouput tape matches the expected ouput tape, then, success

- Start documenting code!!!

- Use circular linked list to store all VMs currently under execution. When a new VM is spawned, insert,
when one has finished, remove. After steps are exceuted for current iteration, continue execution of next element in list

- implement instruction fusion
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
    size_t prog_len = 17;
    Word input_tape[] = {1, 3};
    size_t input_len = 2;
    int id = 0;  
    
    Word target_output[] = {1, 3};
    size_t target_output_len = 2;
    int strict = 0;



    VMState state;
    if (!vm_init(&state, prog_tape, prog_len, input_tape, input_len, id)){
        printf("VM initialization failed, aborting execution!");
        return -1;
    }


    vm_step(&state);
    vm_step(&state);
    vm_step(&state);

    print_VMState(&state);

    save_VMState_to_json(&state, "test.json", 4);

    VMState state_new;
    
    load_VMState_from_json(&state_new, "test.json", 0);

    
    printf("\n--- VMState Loaded ---\n");
    print_VMState(&state_new);
    printf("\n--- Continue Execution ---\n");
    vm_run(&state_new);
    printf("\n--- Execution Finished ---\n");
    print_VMState(&state_new);

    int success = eval_output(target_output, state_new.output_tape, target_output_len, state_new.output_len, strict);

    if (success == 1){
        printf("Output match detected!\n");
    }
    else{
        printf("Output do not match!\n");
    }

    save_VMState_to_json(&state_new, "test2.json", 4);
    

    return 0;
}