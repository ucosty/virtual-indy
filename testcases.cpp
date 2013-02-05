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
	p -> set_register(2, 9);
	int instr = make_cmd_SPECIAL(2, 1, 3, 2, 0);
	printf("instruction: %08x\n", instr);
	if (!m -> write_32b(0, instr))
		error_exit("failed to write to memory @ 0");
	int temp_32b;
	if (!m -> read_32b(0, &temp_32b))
		error_exit("failed to read from memory @ 0");
	if (temp_32b != instr)
		error_exit("read from/write to memory differ");
	p -> tick();
	if (p -> get_register(2) != 9)
		error_exit("SRL: rt changed");
	temp_32b = p -> get_register(1);
	if (temp_32b != 72)
		error_exit("SRL: rd (%d) != 72", temp_32b);


	delete p;
	delete mb;
	delete m;

	return 0;
}
