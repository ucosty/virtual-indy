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

	set_PC(0xffffffffbfc00000);

	have_delay_slot = false;
	delay_slot_PC = -1;
}

void processor::tick()
{
	uint32_t instruction = -1;

	if (unlikely(have_delay_slot))
	{
		have_delay_slot = false;

		if (delay_slot_PC & 0x03)
		{
			// address exception
		}

		if (!pmb -> read_32b(delay_slot_PC, &instruction))
		{
			// processor exception FIXME
		}
	}
	else
	{
		if (unlikely(PC & 0x03))
		{
			// address exception
		}

		if (unlikely(!pmb -> read_32b(PC, &instruction)))
		{
			// processor exception FIXME
		}

		PC += 4;
	}

	uint8_t opcode = (instruction >> 26) & MASK_6B;

	// the other methods are really i_types with the opcode set to a certain value
	// well maybe not in the cpu but logically they are
	(((processor*)this)->*processor::i_type_methods[opcode])(instruction);
}

void processor::set_delay_slot(uint64_t offset)
{
	if (have_delay_slot)
		pdc -> log("trying to set delay slot (%016llx) while already set (%016llx)", offset, delay_slot_PC);

	have_delay_slot = true;

	delay_slot_PC = offset;
}

uint64_t processor::get_delay_slot_PC()
{
	if (!have_delay_slot)
		pdc -> log("trying to retrieve delay slot address (%016llx) while it is not valid (set)", delay_slot_PC);

	return delay_slot_PC;
}

bool processor::get_mem_32b(int offset, uint32_t *value) const
{
	return pmb -> read_32b(offset, value);
}

uint32_t processor::get_C0_register(uint8_t nr, uint8_t sel)
{
	ASSERT(nr >=0 && nr <= 31);

// what to do with `sel'?
// FIXME verify cpu mode? (privileged) and log msg

	return C0_registers[nr];
}

void processor::set_C0_register(uint8_t nr, uint8_t sel, uint32_t value)
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
