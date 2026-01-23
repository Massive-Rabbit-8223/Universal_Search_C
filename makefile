VM_OBJECTS :=  vm/vm_core/vm_core.o vm/vm_utils/vm_utils.o vm/verify/verify.o  #$(wildcard vm/*.o)	# get all object files from vm/
MAIN_OBJECT := main.o

OBJECTS := ${VM_OBJECTS} ${MAIN_OBJECT} 


out: ${OBJECTS}
	gcc ${OBJECTS} -o out

${OBJECTS}: %.o: %.c
	gcc -c $^ -o $@ -Wall

clean:
	rm -f ${VM_OBJECTS}
	rm -f ${MAIN_OBJECT}
	rm -f out