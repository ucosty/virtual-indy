#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#define __STDC_LIMIT_MACROS // for INT32_MIN
#include <stdint.h>

#include "error.h"
#include "log.h"
#include "debug_console_testcases.h"
#include "processor.h"
#include "processor_utils.h"
#include "exceptions.h"

#define TEST_VAL_1 0x12345678abcdefff
#define TEST_VAL_2 0x87654321beefdead

bool single_step = false; // not applicable
debug_console *dc = new debug_console_testcases();

const char *logfile = "testcases.log";

void error_exit(const char *format, ...)
{
	char buffer[4096] = { 0 };
	va_list ap;

	va_start(ap, format);
	(void)vsnprintf(buffer, sizeof buffer, format, ap);
	va_end(ap);

	fprintf(stderr, "%s\n", buffer);
	dc -> dc_log("%s", buffer);

	exit(1);
}

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

		try
		{
			mb -> write_32b(offset, instructions.at(index));
		}
		catch(processor_exception & pe)
		{
			error_exit("failed to write to memory EXCEPTION %d at/for %016llx, PC: %016llx (1), sr: %08x", pe.get_cause(), pe.get_BadVAddr(), pe.get_EPC(), pe.get_status());
		}
	}

	tick(p);
}

void create_system(memory_bus **mb, memory **m1, memory **m2, memory **m3 = NULL, processor **p = NULL, int *m1s = NULL, int *m2s = NULL, int *m3s = NULL, uint64_t *po1 = NULL, uint64_t *po2 = NULL, uint64_t *po3 = NULL)
{
	*mb = new memory_bus(dc);

	int mem_size1 = 0x100000; // FIXME verify power2
	*m1 = new memory(mem_size1, true);
	uint64_t o1 = 0;
	(*mb) -> register_memory(o1, mem_size1 - 1, *m1);

	int mem_size2 = 0x200; // verify power2
	*m2 = new memory(mem_size2, true);
	uint64_t o2 = 0xf00000;
	(*mb) -> register_memory(o2, mem_size2 - 1, *m2);

	int mem_size3 = 128 * 1024;
	uint64_t o3 = 0xffffffffbfc00000;
	if (m3)
	{
		*m3 = new memory(mem_size3, true);
		(*mb) -> register_memory(o3, mem_size3 - 1, *m3);
	}

	*p = new processor(dc, *mb);

	if (m1s)
		*m1s = mem_size1;
	if (m2s)
		*m2s = mem_size2;
	if (m3s)
		*m3s = mem_size3;
	if (po1)
		*po1 = o1;
	if (po2)
		*po2 = o2;
	if (po3)
		*po3 = o3;
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
	dolog(" + test_make_instruction");
	uint8_t rs = 13;
	uint8_t rt = 4;
	uint8_t function = 0x31;
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
	uint8_t sa = 17;
	uint8_t extra = rs;
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
	dolog(" + test_count_leading");
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
	dolog(" + test_rotate_right");
	int value = 2;
	uint8_t n = 2; 
	uint8_t width = 3;
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
	dolog(" + test_processor");
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &m3, &p);

	// 32bit
	{
		int cmp_val = 0xdeadbeef;

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
			int32_t val = p -> get_register_32b_signed(reg);
			if (val != cmp_val)
				error_exit("register %d has invalid value %x", val);
		}

		dolog("ignore the \"register 0 changed\" message");
		p -> set_register_32b(0, cmp_val);
		if (p -> get_register_32b_signed(0))
			error_exit("register 0 changed value!");
	}

	// 64bit
	{
		uint64_t cmp_val = 0xdeadbeef12345678;

		p -> reset();
		p -> set_PC(0);

		for(int reg=1; reg<32; reg++)
			p -> set_register_64b(reg, cmp_val);

		p -> reset();

		for(int reg=0; reg<32; reg++)
		{
			if (p -> get_register_64b_unsigned(reg))
				error_exit("register %d has a value after a reset", reg);
		}

		for(int reg=1; reg<32; reg++)
			p -> set_register_64b(reg, cmp_val);

		for(int reg=1; reg<32; reg++)
		{
			uint64_t val = p -> get_register_64b_unsigned(reg);
			if (val != cmp_val)
				error_exit("register %d has invalid value %x", val);
		}

		dolog("ignore the \"register 0 changed\" message");
		p -> set_register_64b(0, cmp_val);
		if (p -> get_register_64b_unsigned(0))
			error_exit("register 0 changed value!");
	}

	free_system(mb, m1, m2, p);
}

void test_memory()
{
	dolog(" + test_memory");
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	int size = -1;
	create_system(&mb, &m1, &m2, &m3, &p, &size);

	uint64_t address = 8;

	uint8_t value_8b = 0x0f;
	m1 -> write_8b(address, value_8b);

	uint8_t temp_8b = -1;
	m1 -> read_8b(address, &temp_8b);

	if (temp_8b != value_8b)
		error_exit("failed to verify data (8b)");

	address = 16;

	uint16_t value_16b = 0x1234;
	m1 -> write_16b(address, value_16b);

	uint16_t temp_16b = -1;
	m1 -> read_16b(address, &temp_16b);

	if (temp_16b != value_16b)
		error_exit("failed to verify data (16b)");

	address = 24;

	uint32_t value_32b = 0x12345678;
	m1 -> write_32b(address, value_32b);

	uint32_t temp_32b = -1;
	m1 -> read_32b(address, &temp_32b);

	if (temp_32b != value_32b)
		error_exit("failed to verify data (32b)");

	address = 32;

	uint64_t value_64b = 0x1234567890abcdef;
	m1 -> write_64b(address, value_64b);

	uint64_t temp_64b = -1;
	m1 -> read_64b(address, &temp_64b);

	if (temp_64b != value_64b)
		error_exit("failed to verify data (64b)");

	free_system(mb, m1, m2, p);
}

void test_memory_bus()
{
	dolog(" + test_memory_bus");
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	uint64_t o1 = -1, o2 = -1, o3 = -1;
	int dummy = -2;
	create_system(&mb, &m1, &m2, &m3, &p, &dummy, &dummy, &dummy, &o1, &o2, &o3);

	try
	{
		uint32_t value = 0x11223344;
		mb -> write_32b(o1, value);

		uint32_t temp_32b = -1;
		mb -> read_32b(o1, &temp_32b);

		if (temp_32b != value)
			error_exit("failed: write verify error");

		mb -> read_32b(o2, &temp_32b);

		if (temp_32b == value)
			error_exit("failed: segment selection failure");
	}
	catch(processor_exception pe)
	{
		error_exit("failed to access memory EXCEPTION %d at/for %016llx, PC: %016llx (1), sr: %08x", pe.get_cause(), pe.get_BadVAddr(), pe.get_EPC(), pe.get_status());
	}

	free_system(mb, m1, m2, p);
}

void test_twos_complement()
{
	dolog(" + test_twos_complement");
	uint8_t bits = 8;
	int value = -1, expected = 255;
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
	dolog(" + test_untwos_complement");
	/* int untwos_complement(int value, int bits) */
	int value = 0x80;
	uint8_t bits = 8;
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
	if (rc != INT32_MIN)
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
	dolog(" + test_sign_extend");
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
	dolog(" + test_LW");
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &m3, &p);

	// unsigned offset
	{
		p -> reset();
		p -> set_PC(0);

		uint8_t base = 1;
		uint64_t base_val = 0x1234;
		p -> set_register_64b(base, base_val);

		uint8_t rt = 2;
		uint64_t rt_val = TEST_VAL_2;
		p -> set_register_64b(rt, rt_val);

		int16_t offset = 0xf8;

		uint8_t function = 0x23;	// LW

		uint32_t instr = make_cmd_I_TYPE(base, rt, function, offset);
		m1 -> write_32b(0, instr);
		// printf("instruction: %08x\n", instr);

		uint32_t addr_val = 0xdeafbeef;
		uint64_t addr = base_val + offset;
		m1 -> write_32b(addr, addr_val);

		tick(p);

		uint32_t temp_32b = p -> get_register_32b_unsigned(rt);

		uint32_t expected = addr_val;
		if (temp_32b != expected)
			error_exit("LW: rt (%08x) != %08x", temp_32b, expected);
	}

	// signed offset
	{
		p -> reset();
		p -> set_PC(0);

		uint8_t base = 1;
		uint64_t base_val = 0xf0000;
		p -> set_register_32b(base, base_val);

		uint8_t rt = 2;
		uint64_t rt_val = TEST_VAL_1;
		p -> set_register_64b(rt, rt_val);

		uint16_t offset = 0x9014;

		uint8_t function = 0x23;	// LW

		uint32_t temp_32b = -1;

		uint32_t instr = make_cmd_I_TYPE(base, rt, function, offset);
		m1 -> write_32b(0, instr);
		// printf("instruction: %08x\n", instr);

		uint32_t addr_val = 0xdeafbeef;
		uint64_t addr = base_val + untwos_complement(offset, 16);
		m1 -> write_32b(addr, addr_val);

		tick(p);

		temp_32b = p -> get_register_32b_signed(rt);

		uint32_t expected = addr_val;
		if (temp_32b != expected)
			error_exit("LW: rt (%016llx) != %016llx", temp_32b, expected);
	}

	free_system(mb, m1, m2, p);
}

void test_SLL()
{
	dolog(" + test_SLL");
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &m3, &p);

	p -> reset();
	p -> set_PC(0);

	uint64_t rd_val = 1313121210101414;
	uint8_t rd = 1;
	p -> set_register_64b(rd, rd_val);

	uint64_t rt_val = 0xf000e000d000c000;
	uint8_t rt = 2;
	p -> set_register_64b(rt, rt_val);

	uint8_t sa = 3;

	uint8_t function = 0, extra = 0;

	uint32_t instr = make_cmd_SPECIAL(rt, rd, sa, function, extra);
	m1 -> write_32b(0, instr);

	tick(p);

	uint64_t input_val_check = p -> get_register_64b_unsigned(rt);
	if (input_val_check != rt_val)
		error_exit("SLL: rt changed from %016llx to %016llx", rt_val, input_val_check);

	uint64_t temp_64b = p -> get_register_64b_signed(rd);

	uint64_t expected = sign_extend_32b(rt_val << sa);
	if (temp_64b != expected)
		error_exit("SLL: rd (%016llx) != %016llx", temp_64b, expected);

	free_system(mb, m1, m2, p);
}

void test_SRL()
{
	dolog(" + test_SRL");
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &m3, &p);

	p -> reset();
	p -> set_PC(0);

	uint8_t rd = 1;
	p -> set_register_32b(rd, 13);

	int input_val = 191;
	uint8_t rt = 2;
	p -> set_register_32b(rt, input_val);

	uint8_t sa = 3;

	int function = 2, extra = 0;
	uint32_t instr = make_cmd_SPECIAL(rt, rd, sa, function, extra);
	// printf("instruction: %08x\n", instr);

	m1 -> write_32b(0, instr);

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
	dolog(" + test_LUI");
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &m3, &p);

	p -> reset();
	p -> set_PC(0);

	uint8_t rs = 1;
	uint64_t rs_val = TEST_VAL_1;
	p -> set_register_64b(rs, rs_val);

	uint8_t rt = 4;
	uint64_t rt_val = TEST_VAL_2;
	p -> set_register_64b(rt, rt_val);

	uint8_t function = 0x0f;
	int immediate = 0xabcd;
	uint64_t expected = sign_extend_32b(immediate << 16);
	uint32_t instr = make_cmd_I_TYPE(rs, rt, function, immediate);

	m1 -> write_32b(0, instr);

	tick(p);

	uint64_t rc = p -> get_register_64b_unsigned(rt);
	if (rc != expected)
		error_exit("LUI failed: expected %x got %x", expected, rc);

	rc = p -> get_register_64b_unsigned(rs);
	if (rc != rs_val)
		error_exit("LUI failed: register %d changed from %016llx to %016llx", rs, rs_val, rc);

	free_system(mb, m1, m2, p);
}

void test_SW()
{
	dolog(" + test_SW");
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &m3, &p);

	p -> reset();
	p -> set_PC(0);

	uint32_t verify_val = 0x1234beef;
	uint8_t rt = 1;
	p -> set_register_32b(rt, verify_val);

	int address_base = 0x1000;
	uint8_t base = 9, rs = base;
	p -> set_register_32b(base, address_base);

	int offset = 0x100, immediate = offset;

	int final_address = address_base + offset;

	uint8_t function = 0x2b;
	uint32_t instruction = make_cmd_I_TYPE(rs, rt, function, immediate);

	m1 -> write_32b(0, instruction);

	tick(p);

	uint32_t result_mem_val = -1;
	m1 -> read_32b(final_address, &result_mem_val);

	if (result_mem_val != verify_val)
		error_exit("SW: expected %08x, got %08x", verify_val, result_mem_val);

	// FIXME test that unaligned memory access causes a AddressError exception

	free_system(mb, m1, m2, p);
}

void test_ORI()
{
	dolog(" + test_ORI");
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &m3, &p);

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

	uint8_t function = 0x0d;
	uint32_t instruction = make_cmd_I_TYPE(rs, rt, function, immediate);

	m1 -> write_32b(0, instruction);

	tick(p);

	int result = p -> get_register_32b_signed(rt);
	if (result != expected)
		error_exit("ORI: result is %08x, expected %08x", result, expected);

	free_system(mb, m1, m2, p);
}

void test_ANDI()
{
	dolog(" + test_ANDI");
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &m3, &p);

	p -> reset();
	p -> set_PC(0);

	uint64_t old_val = 0x1234beefdead5678;
	uint8_t rt = 1;
	p -> set_register_64b(rt, old_val);

	uint16_t immediate = 0x1234;

	uint64_t and_value = 0x43210123abcddead;
	uint8_t rs = 9;
	p -> set_register_64b(rs, and_value);

	int expected = and_value & immediate;

	uint8_t function = 0x0c;
	uint32_t instruction = make_cmd_I_TYPE(rs, rt, function, immediate);

	m1 -> write_32b(0, instruction);

	tick(p);

	int result = p -> get_register_32b_signed(rt);
	if (result != expected)
		error_exit("ANDI: result is %08x, expected %08x", result, expected);

	free_system(mb, m1, m2, p);
}

void test_XORI()
{
	dolog(" + test_XORI");
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &m3, &p);

	p -> reset();
	p -> set_PC(0);

	uint64_t old_val = 0x1234beefdead5678;
	uint8_t rt = 1;
	p -> set_register_64b(rt, old_val);

	uint16_t immediate = 0x1234;

	uint64_t xor_value = 0x43210123abcddead;
	uint8_t rs = 9;
	p -> set_register_64b(rs, xor_value);

	int expected = xor_value ^ immediate;

	uint8_t function = 0x0e;
	uint32_t instruction = make_cmd_I_TYPE(rs, rt, function, immediate);

	m1 -> write_32b(0, instruction);

	tick(p);

	int result = p -> get_register_32b_signed(rt);
	if (result != expected)
		error_exit("ANDI: result is %08x, expected %08x", result, expected);

	free_system(mb, m1, m2, p);
}

void test_ADDIU()
{
	dolog(" + test_ADDIU");
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &m3, &p);

	p -> reset();
	p -> set_PC(0);

	uint64_t rt_val = 0x1234beefccccdddd;
	uint8_t rt = 1;
	p -> set_register_64b(rt, rt_val);

	int immediate = 0x1234;

	uint64_t rs_val = 0x4321abcdaaaabbbb;
	uint8_t rs = 9;
	p -> set_register_64b(rs, rs_val);

	int32_t expected_s32 = rs_val + int16_t(immediate);
	uint64_t expected = expected_s32; // sign extended

	uint8_t function = 0x09;
	uint32_t instruction = make_cmd_I_TYPE(rs, rt, function, immediate);

	m1 -> write_32b(0, instruction);

	tick(p);

	uint64_t result = p -> get_register_64b_signed(rt);
	if (result != expected)
		error_exit("ADDIU: result is %08x, expected %08x", result, expected);

	free_system(mb, m1, m2, p);
}

void test_AND()
{
	dolog(" + test_AND");
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &m3, &p);

	p -> reset();
	p -> set_PC(0);

	int sa = 31;

	uint64_t rt_val = 0x1234beef5678dead;
	uint8_t rt = 1;
	p -> set_register_64b(rt, rt_val);

	uint64_t rs_val = 0xdeaf56781234beef;
	uint8_t rs = 9;
	p -> set_register_64b(rs, rs_val);

	uint64_t rd_val = 0xaaaabbbbccccdddd;
	uint8_t rd = 10;
	p -> set_register_64b(rd, rd_val);

	uint64_t expected = rs_val & rt_val;

	uint8_t function = 0x24, extra = rs;
	uint32_t instruction = make_cmd_SPECIAL(rt, rd, sa, function, extra);

	m1 -> write_32b(0, instruction);

	tick(p);

	uint64_t result = p -> get_register_64b_signed(rd);
	if (result != expected)
		error_exit("AND: result is %016llx, expected %016llx", result, expected);

	free_system(mb, m1, m2, p);
}

void test_OR()
{
	dolog(" + test_OR");
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &m3, &p);

	p -> reset();
	p -> set_PC(0);

	int sa = 31;

	uint64_t rt_val = 0x1234beef5678dead;
	uint8_t rt = 1;
	p -> set_register_64b(rt, rt_val);

	uint64_t rs_val = 0xdeaf5678beef1234;
	uint8_t rs = 10;
	p -> set_register_64b(rs, rs_val);

	uint64_t rd_val = 0xaaaabbbbccccdddd;
	uint8_t rd = 9;
	p -> set_register_64b(rd, rd_val);

	uint64_t expected = rs_val | rt_val;

	uint8_t function = 0x25, extra = rs;
	uint32_t instruction = make_cmd_SPECIAL(rt, rd, sa, function, extra);

	m1 -> write_32b(0, instruction);

	tick(p);

	uint64_t result = p -> get_register_64b_unsigned(rd);
	if (result != expected)
		error_exit("OR: result is %016llx, expected %016llx", result, expected);

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
	dolog(" + test_NOP");
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &m3, &p);

	p -> reset();
	p -> set_PC(0);

	all_registers_t *reg_copy = copy_registers(p);

	uint8_t opcode = 0;
	uint8_t function = 0;
	uint8_t sa = 0, rd = 0, rt = 0, rs = 0;
	uint32_t instruction = make_cmd_R_TYPE(opcode, sa, rd, rt, rs, function);

	m1 -> write_32b(0, instruction);

	tick(p);

	for(uint8_t nr=0; nr<32; nr++)
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

void test_Bxx(std::string which, uint8_t function, uint8_t rs, uint8_t rt, uint64_t rs_M, uint64_t rt_M, uint64_t rs_NM, uint64_t rt_NM, bool likely)
{
	// FIXME
	// invoke function with rs/rs_M, rt/rt_M
	// if branches and br == true: OK, check pc, check delay slot processing
	// LIKELY invoke function with rs/rs_M, rt/rt_M
	// LIKELY if branches and br == true: OK, check pc, check delay slot processing
	// invoke function with rs/rs_NM, rt/rt_NM
	// if NOT branches and br == true: OK, check pc, check delay slot processing
	// LIKELY invoke function with rs/rs_M, rt/rt_M
	// LIKELY if NOT branches and br == true: OK, check pc, check delay slot processing
	dolog(" + test_%s", which.c_str());
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &m3, &p);

	// DO branch positive offset
	dolog(" > DO branch");
	p -> reset();
	p -> set_PC(0);

	p -> set_register_64b(rs, rs_M);

	if (rt == 0 && rt_M != 0)
		error_exit("testcase failure: rt_M must be 0 for register 0");

	if (rt) // register 0 is always 0
		p -> set_register_64b(rt, rt_M);

	uint16_t immediate = 0x128;

	uint32_t instruction = make_cmd_I_TYPE(rs, rt, function, immediate);
	m1 -> write_32b(0, instruction);

	// If the branch is taken, the instruction in the delay slot is executed.
	if (likely)
	{
		uint8_t likely_rt = rs, likely_rs = 7;

		dolog(" >> if the branch is taken, the instruction in the delay slot is executed");
		if (rt == likely_rs || rs == likely_rs)
			error_exit("testcase failure: rs/rt cannot be %d due to testcase restrictions", likely_rs);
		p -> set_register_64b(likely_rs, 1);

		uint8_t a_function = 0x09;
		uint32_t a_instruction = make_cmd_I_TYPE(likely_rs, likely_rt, a_function, 1234);

		m1 -> write_32b(4, a_instruction);
	}

	tick(p);

	uint64_t expected_pc = 4 + (int16_t(immediate) << 2);

	if (p -> get_PC() != expected_pc)
		error_exit("%s with branch (pos): expected %016llx, got %016llx", which.c_str(), expected_pc, p -> get_PC());

	if (likely)
	{
		tick(p);

		if (p -> get_PC() != expected_pc)
			error_exit("%s with branch [1]: unexpected pc (%016llx), expecting %016llx", which.c_str(), p -> get_PC(), expected_pc);
		if (p -> get_register_64b_unsigned(rt) != rt_M)
			error_exit("%s with branch (pos/likely): delay slot changed rt (double fault!)", which.c_str());
		if (p -> get_register_64b_unsigned(rs) == rs_M)
			error_exit("%s with branch (pos/likely): delay slot did not change rs", which.c_str());
	}

	// DO branch negative offset
	dolog(" > DO branch");
	p -> reset();
	p -> set_PC(0);

	p -> set_register_64b(rs, rs_M);

	if (rt == 0 && rt_M != 0)
		error_exit("testcase failure: rt_M must be 0 for register 0");

	if (rt) // register 0 is always 0
		p -> set_register_64b(rt, rt_M);

	immediate = -0x128;

	instruction = make_cmd_I_TYPE(rs, rt, function, immediate);
	m1 -> write_32b(0, instruction);

	tick(p);

	expected_pc = 4 + (int16_t(immediate) << 2);

	if (p -> get_PC() != expected_pc)
		error_exit("%s with branch (neg): expected %016llx, got %016llx", which.c_str(), expected_pc, p -> get_PC());

	// DON'T branch
	dolog(" > DON'T branch");
	p -> reset();
	p -> set_PC(0);

	p -> set_register_64b(rs, rs_NM);

	if (rt == 0 && rt_NM != 0)
		error_exit("testcase failure: rt_NM must be 0 for register 0");

	if (rt) // register 0 is always 0
		p -> set_register_64b(rt, rt_NM);

	immediate = 0x128;

	instruction = make_cmd_I_TYPE(rs, rt, function, immediate);
	m1 -> write_32b(0, instruction);

	tick(p);

	expected_pc = 4;

	if (p -> get_PC() != expected_pc)
		error_exit("%s without branch: expected %016llx, got %016llx", which.c_str(), expected_pc, p -> get_PC());

	// If the branch is not taken, the instruction in the delay slot is not executed.
	if (likely)
	{
		dolog(" >> if the branch is not taken, the instruction in the delay slot is not executed");
		tick(p);

		uint16_t cur_expected_pc = 8;
		if (p -> get_PC() != cur_expected_pc)
			error_exit("%s without branch (likely): unexpected pc (%016llx), expecting %016llx", which.c_str(), p -> get_PC(), cur_expected_pc);

		if (p -> get_register_64b_unsigned(rs) != rs_NM)
			error_exit("%s without branch (pos/likely): delay slot changed rs %016llx (rs) %016llx (expected)", which.c_str(), p -> get_register_64b_unsigned(rs), rs_NM);
		if (p -> get_register_64b_unsigned(rt) != rt_NM)
			error_exit("%s without branch (pos/likely): delay slot executed?", which.c_str());
	}

	free_system(mb, m1, m2, p);
}

void test_test_tc_overflow_32b()
{
	dolog(" + test_tc_overflow_32b");
	if (test_tc_overflow_32b(1, 1))
		error_exit("test_tc_overflow_32b: indicates overflow for 1, 1");

	if (test_tc_overflow_32b(-1, -1))
		error_exit("test_tc_overflow_32b: indicates overflow for -1, -1");

	if (test_tc_overflow_32b(1, -1))
		error_exit("test_tc_overflow_32b: indicates overflow for 1, -1");

	if (test_tc_overflow_32b(-1, 1))
		error_exit("test_tc_overflow_32b: indicates overflow for -1, 1");

	if (test_tc_overflow_32b(1, -3))
		error_exit("test_tc_overflow_32b: indicates overflow for 1, -3");

	if (test_tc_overflow_32b(-1, 3))
		error_exit("test_tc_overflow_32b: indicates overflow for -1, 3");

	if (!test_tc_overflow_32b(-2147483647, -2147483647))
		error_exit("test_tc_overflow_32b: does not indicate overflow for -2147483647, -2147483647");

	if (!test_tc_overflow_32b(INT32_MIN, -1))
		error_exit("test_tc_overflow_32b: does not indicate overflow for %d, -1", INT32_MIN);

	if (test_tc_overflow_32b(INT32_MIN, 2147483647))
		error_exit("test_tc_overflow_32b: indicates overflow for %d, 2147483647", INT32_MIN);

	if (test_tc_overflow_32b(0, 2147483647))
		error_exit("test_tc_overflow_32b: indicates overflow for 0, 2147483647");

	if (!test_tc_overflow_32b(2147483647, 1))
		error_exit("test_tc_overflow_32b: does not indicate overflow for 2147483647, 1");
}

void test_ADDI()
{
	dolog(" + test_ADDI");
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &m3, &p);

	p -> reset();
	// test 1
	p -> set_PC(0);

	uint64_t rt_val = 0x1234beefccccdddd;
	uint8_t rt = 1;
	p -> set_register_64b(rt, rt_val);

	uint16_t immediate = 0x1234;

	uint64_t rs_val = 0x4321abcd80000000;
	uint8_t rs = 9;
	p -> set_register_64b(rs, rs_val);

	int32_t expected_s32 = rs_val + int16_t(immediate);
	uint64_t expected = expected_s32; // sign extended

	uint8_t function = 0x08;
	uint32_t instruction = make_cmd_I_TYPE(rs, rt, function, immediate);

	m1 -> write_32b(0, instruction);

	tick(p);

	uint64_t result = p -> get_register_64b_signed(rt);
	if (result != expected)
		error_exit("ADDI: result is %016llx, expected %016llx", result, expected);

	// test 2
	p -> set_PC(0);
	p -> set_status_register(0xffffffff); // enable exceptions

	p -> set_register_64b(rt, rt_val);
	expected = rt_val;
	immediate = 0x8000;
	instruction = make_cmd_I_TYPE(rs, rt, function, immediate);
	m1 -> write_32b(0, instruction);

	tick(p);

	result = p -> get_register_64b_signed(rt);
	if (result != expected)
		error_exit("ADDI: result is %016llx, expected %016llx", result, expected);

	if (p -> get_PC() != 0x80000080)
		error_exit("ADDI: expected exception, expected PC: 0x80000080, PC is: %016llx", p -> get_PC());

	free_system(mb, m1, m2, p);
}

void test_SLT()
{
	dolog(" + test_SLT");
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &m3, &p);

	p -> reset();
	// test 1
	p -> set_PC(0);

	uint64_t rt_val = 0x1234beefccccdddd;
	uint8_t rt = 1;
	p -> set_register_64b(rt, rt_val);

	uint16_t immediate = 0x1234;

	uint64_t rs_val = 0x4321abcd80000000;
	uint8_t rs = 9;
	p -> set_register_64b(rs, rs_val);

	uint64_t expected = rs < rt ? 1 : 0;

	uint8_t opcode = 0;
	uint8_t function = 0x2a;
	uint8_t sa = 0;
	uint8_t rd = 3;
	uint32_t instruction = make_cmd_R_TYPE(opcode, sa, rd, rt, rs, function);

	m1 -> write_32b(0, instruction);

	tick(p);

	uint64_t result = p -> get_register_64b_signed(rd);
	if (result != expected)
		error_exit("SLT: result is %016llx, expected %016llx", result, expected);
}

void test_LB()
{
	dolog(" + test_LB");
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &m3, &p);

	// unsigned offset
	{
		p -> reset();
		p -> set_PC(0);

		uint8_t base = 1;
		uint64_t base_val = 0x1234;
		p -> set_register_64b(base, base_val);

		uint8_t rt = 2;
		uint64_t rt_val = TEST_VAL_2;
		p -> set_register_64b(rt, rt_val);

		int16_t offset = 0xf8;

		uint8_t function = 0x20;	// LB

		uint32_t instr = make_cmd_I_TYPE(base, rt, function, offset);
		m1 -> write_32b(0, instr);
		// printf("instruction: %08x\n", instr);

		uint8_t addr_val = 0x1d;
		uint64_t addr = base_val + offset;
		m1 -> write_8b(addr, addr_val);

		tick(p);

		uint64_t temp_64b = p -> get_register_64b_unsigned(rt);

		uint64_t expected = addr_val;
		if (temp_64b != expected)
			error_exit("LB: rt (%08x) != %08x", temp_64b, expected);
	}

	// signed offset
	{
		p -> reset();
		p -> set_PC(0);

		uint8_t base = 1;
		uint64_t base_val = 0xf0000;
		p -> set_register_32b(base, base_val);

		uint8_t rt = 2;
		uint64_t rt_val = TEST_VAL_1;
		p -> set_register_64b(rt, rt_val);

		uint16_t offset = 0x9014;

		uint8_t function = 0x20;	// LB

		uint32_t temp_32b = -1;

		uint32_t instr = make_cmd_I_TYPE(base, rt, function, offset);
		m1 -> write_32b(0, instr);
		// printf("instruction: %08x\n", instr);

		uint8_t addr_val = 0x2c;
		uint64_t addr = base_val + untwos_complement(offset, 16);
		m1 -> write_8b(addr, addr_val);

		tick(p);

		uint64_t temp_64b = p -> get_register_64b_unsigned(rt);

		uint64_t expected = addr_val;
		if (temp_64b != expected)
			error_exit("LB: rt (%016llx) != %016llx", temp_64b, expected);
	}

	free_system(mb, m1, m2, p);
}

void test_J_JAL(bool is_JAL)
{
	dolog(" + test_J");
	memory_bus *mb = NULL;
	memory *m1 = NULL, *m2 = NULL, *m3 = NULL;
	processor *p = NULL;
	create_system(&mb, &m1, &m2, &m3, &p);

	p -> reset();
	p -> set_PC(0);

	uint32_t offset_unshifted = 0x18;
	uint32_t instruction = ((is_JAL ? 3 : 2) << 26) | offset_unshifted;

	m1 -> write_32b(0, instruction);

	tick(p);

	uint64_t expected_pc = 0x04;
	if (p -> is_delay_slot() == false || p -> get_delay_slot_PC() != expected_pc)
		error_exit("J: expected PC at address %llx because of delay slot, got %s/%016llx", expected_pc, p -> is_delay_slot() ? "true" : "false", p -> get_delay_slot_PC());

	if (is_JAL)
	{
		uint64_t expected_return_address = 0x08;
		if (p -> get_register_64b_unsigned(31) != expected_return_address)
			error_exit("JAL: expected return address %016llx, got %016llx", expected_return_address, p -> get_register_64b_unsigned(31));
	}

	tick(p);

	if (is_JAL)
	{
		uint64_t expected_return_address = 0x08;
		if (p -> get_register_64b_unsigned(31) != expected_return_address)
			error_exit("JAL: expected return address %016llx, got %016llx", expected_return_address, p -> get_register_64b_unsigned(31));
	}

	expected_pc = offset_unshifted << 2;
	if (p -> get_PC() != expected_pc)
		error_exit("J: expected PC at address %llx,  got %016llx", expected_pc, p -> get_PC());

	uint64_t hm_offset = 0xffffffffbfc00000;
	mb -> write_32b(hm_offset, instruction);

	p -> reset();
	p -> set_PC(hm_offset);

	tick(p);

	expected_pc = (hm_offset & sign_extend_32b(15 << 28)) | (offset_unshifted << 2);
	if (p -> get_PC() != expected_pc)
		error_exit("J: expected PC at address %llx,  got %016llx", expected_pc, p -> get_PC());

	if (is_JAL)
	{
		uint64_t expected_return_address = hm_offset + 0x08;
		if (p -> get_register_64b_unsigned(31) != expected_return_address)
			error_exit("JAL: expected return address %016llx, got %016llx", expected_return_address, p -> get_register_64b_unsigned(31));
	}

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
	test_test_tc_overflow_32b();

	test_memory();
	test_memory_bus();
	test_processor();

	test_ADDI();
	test_ADDIU();
	test_AND();
	test_ANDI();
	test_Bxx("BEQ", 0x04, 1, 2, 0x1234, 0x1234, 0x1000, 0x2000, false);
	test_Bxx("BEQL", 0x04 | 16, 1, 2, 0x1234, 0x1234, 0x1000, 0x2000, true);
	test_Bxx("BNE", 0x05, 1, 2, 0x1000, 0x2000, 0x1234, 0x1234, false);
	test_Bxx("BNEL", 0x05 | 16, 1, 2, 0x1000, 0x2000, 0x1234, 0x1234, true);
	test_Bxx("BLEZ", 0x06, 1, 0, 0x8000000000000000ll, 0, 0x1000000000000000ll, 0, false);
	test_Bxx("BLEZL", 0x06 | 16, 1, 0, 0x8000000000000000ll, 0, 0x1000000000000000ll, 0, true);
	test_Bxx("BGTZ", 0x07, 1, 0, 0x1000000000000000ll, 0, 0x8000000000000000ll, 0, false);
	test_Bxx("BGTZL", 0x07 | 16, 1, 0, 0x1000000000000000ll, 0, 0x8000000000000000ll, 0, true);
	test_J_JAL(false);
	test_J_JAL(true);
	test_LB();
	test_LUI();
	test_LW();
	test_NOP();
	test_OR();
	test_ORI();
	test_SLL();
	test_SLT();
	test_SRL();
	test_SW();
	test_XORI();

	// FIXME test exceptions

	printf("all fine\n");

	return 0;
}
