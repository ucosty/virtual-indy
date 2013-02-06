#include <stdio.h>

#include "error.h"
#include "debug_console_simple.h"
#include "processor.h"
#include "processor_utils.h"

debug_console *dc = new debug_console_simple();

void create_system(memory_bus **mb, memory **m1, memory **m2, processor **p, int *m1s = NULL, int *m2s = NULL, int *po1 = NULL, int *po2 = NULL)
{
	*mb = new memory_bus();

	int mem_size1 = 0x100000; // FIXME verify power2
	*m1 = new memory(mem_size1, true);
	int o1 = 0;
	(*mb) -> register_memory(o1, mem_size1 - 1, *m1);

	int mem_size2 = 0x200; // verify power2
	*m2 = new memory(mem_size2, true);
	int o2 = 0xf00000;
	(*mb) -> register_memory(o2, mem_size2 - 1, *m2);

	*p = new processor(dc, *mb);

	if (m1s)
		*m1s = mem_size1;
	if (m2s)
		*m2s = mem_size2;
	if (po1)
		*po1 = o1;
	if (po2)
		*po2 = o2;
}

void free_system(memory_bus *mb, memory *m1, memory *m2, processor *p)
{
	delete p;
	delete mb;
	delete m2;
	delete m1;
}

void test_make_instruction()
{
	int rs = 13;
	int rt = 4;
	int function = 0x31;
	int immediate = 0xabcd;

	int expected = function << 26 |
			rs << 21 |
			rt << 16 |
			immediate;
	int rc = make_cmd_I_TYPE(rs, rt, function, immediate);
	if (rc != expected)
		error_exit("i-type failed: expected %x, got %x", expected, rc);

	int address = 0x3FFFFFE;
	function = 0x2;
	expected = function << 26 | address;

	rc = make_cmd_J_TYPE(function, address);
	if (rc != expected)
		error_exit("j-type failed: expected %x, got %x", expected, rc);

	int rd = 31;
	int sa = 17;
	int extra = rs;
	expected = extra << 21 |
			rt << 16 |
			rd << 11 |
			sa << 6 |
			function;
	rc = make_cmd_SPECIAL(rt, rd, sa, function, extra); // R-type
	if (rc != expected)
		error_exit("r-type failed: expected %x, got %x", expected, rc);
}

void test_count_leading()
{
	// one
	int value = 15, expected = 0;
	int rc = count_leading_ones(32, value);
	if (rc != expected)
		error_exit("count_leading_ones failed (1), expecting %d, got %d", expected, rc);

	expected = 4;
	rc = count_leading_ones(4, value);
	if (rc != expected)
		error_exit("count_leading_ones failed (2), expecting %d, got %d", expected, rc);

	expected = 0;
	rc = count_leading_ones(5, value);
	if (rc != expected)
		error_exit("count_leading_ones failed (3), expecting %d, got %d", expected, rc);

	// zero
	value = 15, expected = 28;
	rc = count_leading_zeros(32, value);
	if (rc != expected)
		error_exit("count_leading_zeros failed (1), expecting %d, got %d", expected, rc);

	expected = 0;
	rc = count_leading_zeros(4, value);
	if (rc != expected)
		error_exit("count_leading_zeros failed (2), expecting %d, got %d", expected, rc);

	expected = 0;
	rc = count_leading_zeros(3, value);
	if (rc != expected)
		error_exit("count_leading_zeros failed (3), expecting %d, got %d", expected, rc);
}

void test_rotate_right()
{
	int value = 2;
	int n = 2; 
	int width = 3;
	int expected = 4;
	int rc = rotate_right(value, n, width);
	if (rc != expected)
		error_exit("rotate_right: width %d, expected %x, got %x", width, expected, rc);

	value = 0xdeadbeef;
	n = 4;
	width = 32;
	expected = 0xfdeadbee;
	rc = rotate_right(value, n, width);
	if (rc != expected)
		error_exit("rotate_right: width %d, expected %x, got %x", width, expected, rc);
}

void test_processor()
{
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &p);

	int cmp_val = 0xdeafbeef;

	p -> reset();

	if (p -> get_PC())
		error_exit("failed: pc != 0 after reset");

	p -> tick();
	if (p -> get_PC() != 4)
		error_exit("failed: pc != 4 after 1 tick");

	for(int reg=0; reg<32; reg++)
		p -> set_register(reg, cmp_val);

	p -> reset();

	for(int reg=0; reg<32; reg++)
	{
		if (p -> get_register(reg))
			error_exit("register %d has a value after a reset", reg);
	}

	for(int reg=0; reg<32; reg++)
		p -> set_register(reg, cmp_val);

	for(int reg=0; reg<32; reg++)
	{
		int val = p -> get_register(reg);
		if (val != cmp_val)
			error_exit("register %d has invalid value %x", val);
	}

	free_system(mb, m1, m2, p);
}

void test_memory()
{
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL;
	processor *p = NULL;
	int size = -1;
	create_system(&mb, &m1, &m2, &p, &size);

	int value = 0x12345678;
	if (m1 -> write_32b(-1, value))
		error_exit("failed: write to negative address");

	if (m1 -> write_32b(size - 1, value))
		error_exit("failed: write to over maximum size");

	int address = 8;
	if (!m1 -> write_32b(address, value))
		error_exit("failed to write");

	int temp_32b = -1;
	if (m1 -> read_32b(-1, &temp_32b))
		error_exit("failed: read from negative address");

	if (m1 -> read_32b(size - 1, &temp_32b))
		error_exit("failed: read from over maximum size");

	if (!m1 -> read_32b(address, &temp_32b))
		error_exit("failed to read");

	if (temp_32b != value)
		error_exit("failed to verify data");

	free_system(mb, m1, m2, p);
}

void test_memory_bus()
{
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL;
	processor *p = NULL;
	int o1 = -1, o2 = -1, dummy = -2;
	create_system(&mb, &m1, &m2, &p, &dummy, &dummy, &o1, &o2);

	int value = 0x11223344;
	if (!mb -> write_32b(o1, value))
		error_exit("failed to write via bus");
	int temp_32b = -1;
	if (!mb -> read_32b(o1, &temp_32b))
		error_exit("failed to read via bus");

	if (temp_32b != value)
		error_exit("failed: write verify error");

	if (!mb -> read_32b(o2, &temp_32b))
		error_exit("failed to read via bus");

	if (temp_32b == value)
		error_exit("failed: segment selection failure");

	free_system(mb, m1, m2, p);
}

void test_twos_complement()
{
	int value = -1, bits = 8, expected = 255;
	int rc = twos_complement(value, bits);
	if (rc != expected)
		error_exit("failed: twos_complement for -1/8, expected %d got %d", expected, rc);

	value = -1;
	bits = 5;
	expected = 31;
	rc = twos_complement(value, bits);
	if (rc != expected)
		error_exit("failed: twos_complement for -1/5, expected %d got %d", expected, rc);

	value = 1;
	bits = 16;
	expected = 1;
	rc = twos_complement(value, bits);
	if (rc != expected)
		error_exit("failed: twos_complement for -1/16, expected %d got %d", expected, rc);
}

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

void test_sign_extend()
{
	int rc = sign_extend_8b(0x80), cmp = 0xffffff80;
	if (rc != cmp)
		error_exit("sign_extend_8b mismatch (%x / %x)", cmp, rc);

	rc = sign_extend_16b(0x8000);
	cmp = 0xffff8000;
	if (rc != cmp)
		error_exit("sign_extend_16b mismatch (%x / %x)", cmp, rc);

	rc = sign_extend(8, 4), cmp = 0xfffffff8;
	if (rc != cmp)
		error_exit("sign_extend mismatch (%x / %x)", cmp, rc);
}

void test_LW()
{
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &p);

	// unsigned offset
	{
		p -> reset();

		int base = 1;
		int base_val = 9;
		p -> set_register(base, base_val);

		int rt = 2;

		int offset = 0xff;

		int function = 0x23;	// LW

		int instr = make_cmd_I_TYPE(base, rt, function, offset);
		if (!m1 -> write_32b(0, instr))
			error_exit("LW: failed to write to offset 0 in memory");
		// printf("instruction: %08x\n", instr);

		int addr_val = 0xdeafbeef;
		int addr = base_val + untwos_complement(offset, 16);
		if (!m1 -> write_32b(addr, addr_val))
			error_exit("LW: failed to write to offset %x in memory", addr);

		p -> tick();

		int temp_32b = p -> get_register(rt);

		int expected = addr_val;
		if (temp_32b != expected)
			error_exit("LW: rt (%d) != %d", temp_32b, expected);
	}

	// signed offset
	{
		p -> reset();

		int base = 1;
		int base_val = 0xf0000;
		p -> set_register(base, base_val);

		int rt = 2;

		int offset = 0x9014;

		int function = 0x23;	// LW

		int temp_32b = -1;

		int instr = make_cmd_I_TYPE(base, rt, function, offset);
		if (!m1 -> write_32b(0, instr))
			error_exit("failed to write to memory @ 0");
		// printf("instruction: %08x\n", instr);

		int addr_val = 0xdeafbeef;
		int addr = base_val + untwos_complement(offset, 16);
		if (!m1 -> write_32b(addr, addr_val))
			error_exit("failed to write to memory @ 0");

		p -> tick();

		temp_32b = p -> get_register(rt);

		int expected = addr_val;
		if (temp_32b != expected)
			error_exit("LW: rt (%x) != %x", temp_32b, expected);
	}

	free_system(mb, m1, m2, p);
}

void test_SLL()
{
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &p);

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
	if (!m1 -> write_32b(0, instr))
		error_exit("failed to write to memory @ 0");
	int temp_32b;

	p -> tick();

	int input_val_check = p -> get_register(rt);
	if (input_val_check != input_val)
		error_exit("SLL: rt changed from %d to %d", input_val, input_val_check);

	temp_32b = p -> get_register(rd);

	int expected = input_val << sa;
	if (temp_32b != expected)
		error_exit("SLL: rd (%d) != %d", temp_32b, expected);

	free_system(mb, m1, m2, p);
}

void test_SRL()
{
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &p);

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

	if (!m1 -> write_32b(0, instr))
		error_exit("failed to write to memory @ 0");

	p -> tick();

	int input_val_check = p -> get_register(2);
	if (input_val_check != input_val)
		error_exit("SRL: rt changed from %d to %d", input_val, input_val_check);

	int temp_32b = p -> get_register(1);
	int expected = input_val >> sa;
	if (temp_32b != expected)
		error_exit("SRL: rd (%d) != %d", temp_32b, expected);

	free_system(mb, m1, m2, p);
}

void test_LUI()
{
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &p);

	p -> reset();

	int rt = 4;
	int function = 0x0f;
	int immediate = 0xabcd;
	int expected = immediate << 16;
	int instr = make_cmd_I_TYPE(0, rt, function, immediate);

	if (!m1 -> write_32b(0, instr))
		error_exit("failed to write to memory @ 0");
	p -> tick();

	int rc = p -> get_register(rt);
	if (rc != expected)
		error_exit("LUI failed: expected %x got %x", expected, rc);

	free_system(mb, m1, m2, p);
}

int main(int argc, char *argv[])
{
	test_untows_complement();

	test_twos_complement();

	test_sign_extend();

	test_count_leading();

	test_rotate_right();

	test_make_instruction();

	test_memory();

	test_memory_bus();

	test_processor();

	test_LUI();

	test_LW();

	test_SLL();

	test_SRL();

	printf("all fine\n");

	return 0;
}
