#include <stdio.h>

#include "error.h"
#include "processor.h"
#include "processor_utils.h"

int main(int argc, char *argv[])
{
	memory_bus *mb = new memory_bus();
	memory *m = new memory(4 * 1024);

	mb -> register_memory(0, 0xfff, m);

	processor *p = new processor(mb);

	/* SRL */
	p -> set_register(1, 13);
	int input_val = 191;
	p -> set_register(2, input_val);
	int instr = make_cmd_SPECIAL(2, 1, 3, 2, 0);
	// printf("instruction: %08x\n", instr);
	if (!m -> write_32b(0, instr))
		error_exit("failed to write to memory @ 0");
	int temp_32b;
	if (!m -> read_32b(0, &temp_32b))
		error_exit("failed to read from memory @ 0");
	if (temp_32b != instr)
		error_exit("read from/write to memory differ");
	p -> tick();
	int input_val_check = p -> get_register(2);
	if (input_val_check != input_val)
		error_exit("SRL: rt changed from %d to %d", input_val, input_val_check);
	temp_32b = p -> get_register(1);
	int expected = 23;
	if (temp_32b != expected)
		error_exit("SRL: rd (%d) != %d", temp_32b, expected);


	delete p;
	delete mb;
	delete m;

	printf("all fine\n");

	return 0;
}
