#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "debug_console.h"
#include "processor.h"
#include "processor_utils.h"
#include "utils.h"

processor::processor(debug_console *pdc_in, memory_bus *pmb_in) : pdc(pdc_in), pmb(pmb_in)
{
	cycles = 0;

	init_i_type();
	init_r_type();

	reset();
}

processor::~processor()
{
}

void processor::reset()
{
	memset(registers, 0x00, sizeof registers);

	status_register = HI = LO = PC = 0;

	memset(C0_registers, 0x00, sizeof C0_registers);
}

void processor::tick()
{
	if (PC & 0x03)
	{
		// address exception
	}

	uint32_t instruction = -1;
	if (!pmb -> read_32b(PC, &instruction))
	{
		// processor exception FIXME
	}

	PC += 4;

	uint8_t opcode = (instruction >> 26) & MASK_6B;

	// the other methods are really i_types with the opcode set to a certain value
	// well maybe not in the cpu but logically they are
	(((processor*)this)->*processor::i_type_methods[opcode])(instruction);
}

int32_t processor::get_register_32b(int nr) const
{
	ASSERT(nr >= 0 && nr <= 31);

	return registers[nr] & MASK_32B;
}

uint32_t get_register_32b_signed(int nr) const;
{
	// FIXME
}

int64_t get_register_64b_signed(int nr) const
{
	// FIXME
}

uint64_t get_register_64b_unsigned(int nr) const
{
	// FIXME
}

void processor::set_register_32b(int nr, uint32_t value)
{
	ASSERT(nr >= 0 && nr <= 31);

	if (nr == 0)
		pdc -> log("(32b) trying to alter register 0! (%d)", nr);
	else
		registers[nr] = (registers[nr] & ~MASK_32B) | value;
}

void processor::set_register_32b_se(int nr, int32_t value)
{
	ASSERT(nr >= 0 && nr <= 31);

	if (nr == 0)
		pdc -> log("(32bse) trying to alter register 0! (%d)", nr);
	else
		registers[nr] = sign_extend_32b((unsigned int)value);
}

void processor::set_register_64b(int nr, uint64_t value)
{
	ASSERT(nr >= 0 && nr <= 31);

	if (nr == 0)
		pdc -> log("(64b) trying to alter register 0! (%d)", nr);
	else
		registers[nr] = value;
}

bool processor::get_mem_32b(int offset, uint32_t *value) const
{
	return pmb -> read_32b(offset, value);
}

uint32_t processor::get_C0_register(int nr, int sel)
{
	ASSERT(nr >=0 && nr <= 31);

// what to do with `sel'?
// FIXME verify cpu mode? (privileged) and log msg

	return C0_registers[nr];
}

void processor::set_C0_register(int nr, int sel, uint32_t value)
{
	ASSERT(nr >=0 && nr <= 31);

// what to do with `sel'?
// FIXME verify cpu mode? (privileged) and log msg

	C0_registers[nr] = value;
}

void processor::interrupt(int nr)
{
	// 00 0 external interrupt
	// 01 1 instruction bus error (invalid instruction)
	// 10 2 arithmetic error
	// 11 3 system call

	// bits 8...15 of status register are the interrupt mask
	if (IS_BIT_OFF0_SET(8 + nr, status_register))
	{
		set_C0_register(14, 0, PC);	// EPC (FIXME: sel 0?)
		PC = 0x80000080;	// interrupt service routine
		// cause = (?)
		status_register <<= 4;

		// FIXME
	}
}
