#include <stdio.h>

#include "error.h"
#include "debug_console_simple.h"
#include "processor.h"
#include "processor_utils.h"

void test_untows_complement()
{
	/* int untwos_complement(int value, int bits) */
	int value = 0x80, bits = 8;
	int rc = untwos_complement(value, bits);
	if (rc != -128)
		error_exit("untwos_complement failed 8b (a): %d expected -128", rc);

	value = 0x8000, bits = 16;
	rc = untwos_complement(value, bits);
	if (rc != -32768)
		error_exit("untwos_complement failed 16b (a) %d expected -32768", rc);

	value = 0x80000000, bits = 32;
	rc = untwos_complement(value, bits);
	if (rc != -2147483648)
		error_exit("untwos_complement failed 32b (a) %d expected -2147483648", rc);

	value = 0x40;
	bits = 8;
	rc = untwos_complement(value, bits);
	if (rc != 64)
		error_exit("untwos_complement failed 8b (b) %d expected 64", rc);

	value = 0x4000, bits = 16;
	rc = untwos_complement(value, bits);
	if (rc != 16384)
		error_exit("untwos_complement failed 16b (b) %d expected 16384", rc);

	value = 0x40000000, bits = 32;
	rc = untwos_complement(value, bits);
	if (rc != 1073741824)
		error_exit("untwos_complement failed 32b (b) %d expected 1073741824", rc);
}

void test_SLL(memory *m, processor *p)
{
	p -> reset();

	int rd = 1;
	p -> set_register(rd, 13);

	int input_val = 0xf000;
	int rt = 2;
	p -> set_register(rt, input_val);

	int sa = 3;

	int function = 0, extra = 0;

	int instr = make_cmd_SPECIAL(rt, rd, sa, function, extra);

	// printf("instruction: %08x\n", instr);
	if (!m -> write_32b(0, instr))
		error_exit("failed to write to memory @ 0");
	int temp_32b;
	if (!m -> read_32b(0, &temp_32b))
		error_exit("failed to read from memory @ 0");
	if (temp_32b != instr)
		error_exit("read from/write to memory differ");

	p -> tick();

	int input_val_check = p -> get_register(rt);
	if (input_val_check != input_val)
		error_exit("SLL: rt changed from %d to %d", input_val, input_val_check);

	temp_32b = p -> get_register(rd);

	int expected = input_val << sa;
	if (temp_32b != expected)
		error_exit("SRL: rd (%d) != %d", temp_32b, expected);
}

void test_SRL(memory *m, processor *p)
{
	p -> reset();

	int rd = 1;
	p -> set_register(rd, 13);
	int input_val = 191;
	int rt = 2;
	p -> set_register(rt, input_val);
	int sa = 3;
	int function = 2, extra = 0;
	int instr = make_cmd_SPECIAL(rt, rd, sa, function, extra);
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
	int expected = input_val >> sa;
	if (temp_32b != expected)
		error_exit("SRL: rd (%d) != %d", temp_32b, expected);
}

int main(int argc, char *argv[])
{
	debug_console *dc = new debug_console_simple();

	dc -> init();

	memory_bus *mb = new memory_bus();
	memory *m = new memory(4 * 1024);

	mb -> register_memory(0, 0xfff, m);

	processor *p = new processor(dc, mb);

	test_untows_complement();

	test_SLL(m, p);

	test_SRL(m, p);

	delete p;
	delete mb;
	delete m;

	printf("all fine\n");

	return 0;
}
