#include <stdio.h>

#include "error.h"
#include "debug_console_testcases.h"
#include "processor.h"
#include "processor_utils.h"

bool single_step = false; // not applicable
debug_console *dc = new debug_console_testcases();

const char *logfile = "testcases.log";

void tick(processor *p)
{
	dc -> tick(p);
	p -> tick();
}

void exec(memory_bus *mb, std::vector<int> instructions, processor *p)
{
	for(unsigned int index=0; index<instructions.size(); index++)
	{
		unsigned int offset = index * 4;

		if (!mb -> write_32b(offset, instructions.at(index)))
			error_exit("failed to write to offset %d in memory", offset);
	}

	tick(p);
}

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
	uint8_t rs = 13;
	uint8_t rt = 4;
	int function = 0x31;
	int immediate = 0xabcd;

	uint32_t expected = function << 26 |
			rs << 21 |
			rt << 16 |
			immediate;
	uint32_t rc = make_cmd_I_TYPE(rs, rt, function, immediate);
	if (rc != expected)
		error_exit("i-type failed: expected %x, got %x", expected, rc);

	int address = 0x3FFFFFE;
	function = 0x2;
	expected = function << 26 | address;

	rc = make_cmd_J_TYPE(function, address);
	if (rc != expected)
		error_exit("j-type failed: expected %x, got %x", expected, rc);

	uint8_t rd = 31;
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

	uint64_t reset_vector = 0xffffffffbfc00000;

	if (p -> get_PC() != reset_vector)
		error_exit("failed: pc != %016llx after reset", reset_vector);

	p -> set_PC(0);

	tick(p);
	if (p -> get_PC() != 4)
		error_exit("failed: pc != 4 after 1 tick");

	for(int reg=1; reg<32; reg++)
		p -> set_register_32b(reg, cmp_val);

	p -> reset();
	p -> set_PC(0);

	for(int reg=0; reg<32; reg++)
	{
		if (p -> get_register_32b_signed(reg))
			error_exit("register %d has a value after a reset", reg);
	}

	for(int reg=1; reg<32; reg++)
		p -> set_register_32b(reg, cmp_val);

	for(int reg=1; reg<32; reg++)
	{
		int val = p -> get_register_32b_signed(reg);
		if (val != cmp_val)
			error_exit("register %d has invalid value %x", val);
	}

	p -> set_register_32b(0, cmp_val);
	if (p -> get_register_32b_signed(0))
		error_exit("register 0 changed value!");

	free_system(mb, m1, m2, p);
}

void test_memory()
{
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL;
	processor *p = NULL;
	int size = -1;
	create_system(&mb, &m1, &m2, &p, &size);

	uint32_t value = 0x12345678;
	if (m1 -> write_32b(size - 1, value))
		error_exit("failed: write to over maximum size");

	uint64_t address = 8;
	if (!m1 -> write_32b(address, value))
		error_exit("failed to write");

	uint32_t temp_32b = -1;
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

	uint32_t value = 0x11223344;
	if (!mb -> write_32b(o1, value))
		error_exit("failed to write via bus");
	uint32_t temp_32b = -1;
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

	value = 0x8000;
	rc = int16_t(value);
	if (rc != -32768)
		error_exit("untwos_complement_16b failed (a) %d expected -32768", rc);

	value = 0xffff, bits = 16;
	rc = untwos_complement(value, bits);
	if (rc != -1)
		error_exit("untwos_complement failed 16b (a) %d expected -1", rc);

	value = 0xffff;
	rc = int16_t(value);
	if (rc != -1)
		error_exit("untwos_complement_16b failed (a) %d expected -32768", rc);

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

	value = 0x4000;
	rc = int16_t(value);
	if (rc != 16384)
		error_exit("untwos_complement_16b failed (b) %d expected 16384", rc);

	value = 0x40000000, bits = 32;
	rc = untwos_complement(value, bits);
	if (rc != 1073741824)
		error_exit("untwos_complement failed 32b (b) %d expected 1073741824", rc);
}

void test_sign_extend()
{
	uint64_t rc = sign_extend_8b(0x80), cmp = 0xffffffffffffff80ll;
	if (rc != cmp)
		error_exit("sign_extend_8b mismatch (%x / %x)", cmp, rc);

	rc = sign_extend_16b(0x8000);
	cmp = 0xffffffffffff8000ll;
	if (rc != cmp)
		error_exit("sign_extend_16b mismatch (%x / %x)", cmp, rc);

	rc = sign_extend(8, 4), cmp = 0xfffffffffffffff8ll;
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
		p -> set_PC(0);

		int base = 1;
		int base_val = 9;
		p -> set_register_32b(base, base_val);

		uint8_t rt = 2;

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

		tick(p);

		int temp_32b = p -> get_register_32b_signed(rt);

		int expected = addr_val;
		if (temp_32b != expected)
			error_exit("LW: rt (%d) != %d", temp_32b, expected);
	}

	// signed offset
	{
		p -> reset();
		p -> set_PC(0);

		int base = 1;
		int base_val = 0xf0000;
		p -> set_register_32b(base, base_val);

		uint8_t rt = 2;

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

		tick(p);

		temp_32b = p -> get_register_32b_signed(rt);

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
	p -> set_PC(0);

	uint8_t rd = 1;
	p -> set_register_32b(rd, 13);

	int input_val = 0xf000;
	uint8_t rt = 2;
	p -> set_register_32b(rt, input_val);

	int sa = 3;

	int function = 0, extra = 0;

	int instr = make_cmd_SPECIAL(rt, rd, sa, function, extra);

	// printf("instruction: %08x\n", instr);
	if (!m1 -> write_32b(0, instr))
		error_exit("failed to write to memory @ 0");
	int temp_32b;

	tick(p);

	int input_val_check = p -> get_register_32b_signed(rt);
	if (input_val_check != input_val)
		error_exit("SLL: rt changed from %d to %d", input_val, input_val_check);

	temp_32b = p -> get_register_32b_signed(rd);

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
	p -> set_PC(0);

	uint8_t rd = 1;
	p -> set_register_32b(rd, 13);

	int input_val = 191;
	uint8_t rt = 2;
	p -> set_register_32b(rt, input_val);

	int sa = 3;

	int function = 2, extra = 0;
	int instr = make_cmd_SPECIAL(rt, rd, sa, function, extra);
	// printf("instruction: %08x\n", instr);

	if (!m1 -> write_32b(0, instr))
		error_exit("failed to write to memory @ 0");

	tick(p);

	int input_val_check = p -> get_register_32b_signed(2);
	if (input_val_check != input_val)
		error_exit("SRL: rt changed from %d to %d", input_val, input_val_check);

	int temp_32b = p -> get_register_32b_signed(1);
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
	p -> set_PC(0);

	uint8_t rt = 4;
	int function = 0x0f;
	int immediate = 0xabcd;
	int expected = immediate << 16;
	int instr = make_cmd_I_TYPE(0, rt, function, immediate);

	if (!m1 -> write_32b(0, instr))
		error_exit("failed to write to memory @ 0");
	tick(p);

	int rc = p -> get_register_32b_signed(rt);
	if (rc != expected)
		error_exit("LUI failed: expected %x got %x", expected, rc);

	free_system(mb, m1, m2, p);
}

void test_SW()
{
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &p);

	p -> reset();
	p -> set_PC(0);

	uint32_t verify_val = 0x1234beef;
	uint8_t rt = 1;
	p -> set_register_32b(rt, verify_val);

	int address_base = 0x1000;
	int base = 9, rs = base;
	p -> set_register_32b(base, address_base);

	int offset = 0x100, immediate = offset;

	int final_address = address_base + offset;

	int function = 0x2b;
	int instruction = make_cmd_I_TYPE(rs, rt, function, immediate);

	if (!m1 -> write_32b(0, instruction))
		error_exit("failed to write to memory @ 0");

	tick(p);

	uint32_t result_mem_val = -1;
	if (!m1 -> read_32b(final_address, &result_mem_val))
		error_exit("failed to read memory @ %08x", final_address);

	if (result_mem_val != verify_val)
		error_exit("SW: expected %08x, got %08x", verify_val, result_mem_val);

	// FIXME test that unaligned memory access causes a AddressError exception

	free_system(mb, m1, m2, p);
}

void test_ORI()
{
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &p);

	p -> reset();
	p -> set_PC(0);

	int old_val = 0x1234beef;
	uint8_t rt = 1;
	p -> set_register_32b(rt, old_val);

	int immediate = 0x1234;

	int or_value = 0x4321;
	uint8_t rs = 9;
	p -> set_register_32b(rs, or_value);

	int expected = p -> get_register_32b_signed(rs) | immediate;

	int function = 0x0d;
	int instruction = make_cmd_I_TYPE(rs, rt, function, immediate);

	if (!m1 -> write_32b(0, instruction))
		error_exit("failed to write to memory @ 0");

	tick(p);

	int result = p -> get_register_32b_signed(rt);
	if (result != expected)
		error_exit("ORI: result is %08x, expected %08x", result, expected);

	free_system(mb, m1, m2, p);
}

void test_ADDIU()
{
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &p);

	p -> reset();
	p -> set_PC(0);

	int old_val = 0x1234beef;
	uint8_t rt = 1;
	p -> set_register_32b(rt, old_val);

	int immediate = 0x1234;

	int addiu_value = 0x4321;
	uint8_t rs = 9;
	p -> set_register_32b(rs, addiu_value);

	int expected = (p -> get_register_32b_signed(rs) + int16_t(immediate)) & MASK_32B;

	int function = 0x09;
	int instruction = make_cmd_I_TYPE(rs, rt, function, immediate);

	if (!m1 -> write_32b(0, instruction))
		error_exit("failed to write to memory @ 0");

	tick(p);

	int result = p -> get_register_32b_signed(rt);
	if (result != expected)
		error_exit("ADDIU: result is %08x, expected %08x", result, expected);

	free_system(mb, m1, m2, p);
}

void test_AND()
{
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &p);

	p -> reset();
	p -> set_PC(0);

	int sa = 31;

	int rt_val = 0x1234beef;
	uint8_t rt = 1;
	p -> set_register_32b(rt, rt_val);

	int rs_val = 0xdeaf5678;
	uint8_t rs = 9;
	p -> set_register_32b(rs, rs_val);

	int rd_val = 0xaaaabbbb;
	uint8_t rd = 9;
	p -> set_register_32b(rd, rd_val);

	int expected = p -> get_register_32b_signed(rs) & p -> get_register_32b_signed(rt);

	int function = 0x24, extra = rs;
	int instruction = make_cmd_SPECIAL(rt, rd, sa, function, extra);

	if (!m1 -> write_32b(0, instruction))
		error_exit("failed to write to memory @ 0");

	tick(p);

	int result = p -> get_register_32b_signed(rd);
	if (result != expected)
		error_exit("AND: result is %08x, expected %08x", result, expected);

	free_system(mb, m1, m2, p);
}

typedef struct
{
	uint64_t registers[32], PC, HI, LO, status_register;
	uint32_t C0_registers[32];
}
all_registers_t;

all_registers_t * copy_registers(processor *p)
{
	all_registers_t *s = new all_registers_t;

	for(int nr=0; nr<32; nr++)
	{
		s -> registers[nr] = p -> get_register_32b_signed(nr);
		s -> C0_registers[nr] = p -> get_C0_register(nr, 0);
	}

	s -> PC = p -> get_PC();
	s -> HI = p -> get_HI();
	s -> LO = p -> get_LO();
	s -> status_register = p -> get_SR();

	return s;
}

void test_NOP()
{
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &p);

	p -> reset();
	p -> set_PC(0);

	all_registers_t *reg_copy = copy_registers(p);

	int opcode = 0;
	int function = 0;
	int sa = 0, rd = 0, rt = 0, rs = 0;
	int instruction = make_cmd_R_TYPE(opcode, sa, rd, rt, rs, function);

	if (!m1 -> write_32b(0, instruction))
		error_exit("failed to write to memory @ 0");

	tick(p);

	for(int nr=0; nr<32; nr++)
	{
		if (p -> get_register_32b_unsigned(nr) != reg_copy -> registers[nr])
			error_exit("NOP: register %s (%d) mismatch", processor::reg_to_name(nr), nr);

		if (p -> get_C0_register(nr, 0) != reg_copy -> C0_registers[nr])
			error_exit("NOP: C0 register %d mismatch", nr);
	}

	uint64_t expected_PC = reg_copy -> PC + 4;
	if (expected_PC != p -> get_PC())
		error_exit("NOP: expected PC %08x, got %08x", expected_PC, p -> get_PC());

	if (reg_copy -> HI != p -> get_HI())
		error_exit("NOP: expected HI %08x, got %08x", reg_copy -> HI, p -> get_HI());

	if (reg_copy -> LO != p -> get_LO())
		error_exit("NOP: expected LO %08x, got %08x", reg_copy -> LO, p -> get_LO());

	if (reg_copy -> status_register != p -> get_LO())
		error_exit("NOP: expected LO %08x, got %08x", reg_copy -> status_register, p -> get_LO());

	delete reg_copy;

	free_system(mb, m1, m2, p);
}

void test_BNE()
{
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &p);

	p -> reset();
	p -> set_PC(0);

	int rs_value = 0xdeadbeef;
	uint8_t rs = 3;
	p -> set_register_32b(rs, rs_value);

	int rt_value = rs_value;
	uint8_t rt = 18;
	p -> set_register_32b(rt, rt_value);

	int immediate_org = -1235;
	int immediate = immediate_org & 0xffff;

	int function = 0x05;
	int instruction = make_cmd_I_TYPE(rs, rt, function, immediate);

	if (!m1 -> write_32b(0, instruction))
		error_exit("failed to write to memory @ 0");

	tick(p);

	uint64_t expected_PC = 4;
	if (expected_PC != p -> get_PC())
		error_exit("BNE(1): expected PC %08x, got %08x", expected_PC, p -> get_PC());

	//
	rt_value = ~rs_value;
	p -> set_register_32b(rt, rt_value);

	p -> set_PC(0);

	tick(p);

	expected_PC = 8 + immediate_org * 4;

	if (expected_PC != p -> get_PC())
		error_exit("BNE(2): expected PC %08x, got %08x", expected_PC, p -> get_PC());

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

	test_ADDIU();

	test_AND();

	test_BNE();

	test_LUI();

	test_LW();

	test_ORI();

	test_SLL();

	test_SRL();

	test_SW();

	printf("all fine\n");

	return 0;
}
