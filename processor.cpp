#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "debug_console.h"
#include "processor.h"
#include "processor_utils.h"
#include "utils.h"

processor::processor(debug_console *pdc_in, memory_bus *pmb_in) : pdc(pdc_in), pmb(pmb_in)
{
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

	int instruction = -1;
	if (!pmb -> read_32b(PC, &instruction))
	{
		// processor exception FIXME
	}

	PC += 4;

	int opcode = (instruction >> 26) & MASK_6B;

	// the other methods are really i_types with the opcode set to a certain value
	// well maybe not in the cpu but logically they are
	(((processor*)this)->*processor::i_type_methods[opcode])(instruction);
}

int processor::get_register(int nr) const
{
	ASSERT(nr >= 0 && nr <= 31);

	return registers[nr];
}

void processor::set_register(int nr, int value)
{
	ASSERT(nr >= 0 && nr <= 31);

	if (nr == 0)
		pdc -> log("trying to alter register 0! (%d)", nr);
	else
		registers[nr] = value;
}

bool processor::get_mem_32b(int offset, int *value) const
{
	return pmb -> read_32b(offset, value);
}

int processor::get_C0_register(int nr, int sel)
{
	ASSERT(nr >=0 && nr <= 31);

// what to do with `sel'?
// FIXME verify cpu mode? (privileged) and log msg

	return C0_registers[nr];
}

void processor::set_C0_register(int nr, int sel, int value)
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
