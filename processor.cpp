#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "debug_console.h"
#include "processor.h"
#include "processor_utils.h"
#include "exceptions.h"
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

	status_register = HI = LO = PC = EPC = 0;

	memset(C0_registers, 0x00, sizeof C0_registers);

	set_PC(0xffffffffbfc00000);

	have_delay_slot = false;
	delay_slot_PC = -1;

	RMW_sequence = false;
}

void processor::tick()
{
	try
	{
		uint64_t work_address = PC;
		uint32_t instruction = -1;

		try
		{
			if (unlikely(have_delay_slot))
			{
				have_delay_slot = false;

				work_address = delay_slot_PC;

				if (unlikely(delay_slot_PC & 0x03))
					throw processor_exception(delay_slot_PC, status_register, 0, PE_ADDRL, delay_slot_PC);

				pmb -> read_32b(delay_slot_PC, &instruction);
			}
			else
			{
				if (unlikely(PC & 0x03))
					throw processor_exception(PC, status_register, 0, PE_ADDRL, PC);

				pmb -> read_32b(PC, &instruction);

				PC += 4;
			}
		}
		catch(processor_exception & pe)
		{
			if (pe.get_cause_ExcCode() == PEE_MEM)
				throw processor_exception(work_address, status_register, 0, PE_IBUS, PC);

			throw pe;
		}

		uint8_t opcode = get_opcode(instruction);

		// the other methods are really i_types with the opcode set to a certain value
		// well maybe not in the cpu but logically they are
		(((processor*)this)->*processor::i_type_methods[opcode])(instruction);
	}
	catch(processor_exception & pe)
	{
		// FIXME handle PE_*
		pdc -> dc_log("EXCEPTION %d at/for %016llx, PC: %016llx (1), sr: %08x", pe.get_cause(), pe.get_BadVAddr(), pe.get_EPC(), pe.get_status());

		if (pe.get_cause_ExcCode() == PEE_MEM)
			pe = processor_exception(pe.get_BadVAddr(), status_register, 0, PE_DBUS, PC);
		else if (pe.get_cause_ExcCode() == PEE_RMEMS)
			pe = processor_exception(pe.get_BadVAddr(), status_register, 0, PE_ADDRS, PC);

		if (IS_BIT_OFF0_SET(8 + pe.get_ip(), status_register) && (status_register & 1) == 1)
		{
			status_register = (status_register & 0xFFFFFFC0) | ((status_register & 15) << 2);
			EPC = pe.get_EPC();
			PC = 0x80000080;

			// FIXME: at return, shift >> 2, do not change old state
			// ALSO INCREASE PC WITH 4
		}
	}
}

void processor::set_delay_slot(uint64_t offset)
{
	if (have_delay_slot)
		pdc -> dc_log("trying to set delay slot (%016llx) while already set (%016llx)", offset, delay_slot_PC);

	have_delay_slot = true;

	delay_slot_PC = offset;
}

uint64_t processor::get_delay_slot_PC()
{
	if (!have_delay_slot)
		pdc -> dc_log("trying to retrieve delay slot address (%016llx) while it is not valid (set)", delay_slot_PC);

	return delay_slot_PC;
}

void processor::get_mem_32b(int offset, uint32_t *value) const
{
	pmb -> read_32b(offset, value);
}

uint64_t processor::get_C0_register(uint8_t nr, uint8_t sel)
{
	ASSERT(nr >=0 && nr <= 31);

// what to do with `sel'?
// FIXME verify cpu mode? (privileged) and log msg

	return C0_registers[nr];
}

void processor::set_C0_register(uint8_t nr, uint8_t sel, uint64_t value)
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

void processor::start_RMW_sequence()
{
	if (RMW_sequence)
		pdc -> dc_log("Re(!)-starting RMW sequence");

	RMW_sequence = true;
}

void processor::conditional_jump(bool do_jump, uint32_t instruction, bool skip_delay_slot_if_not)
{
	cycles += 3;

	if (do_jump)
	{
		set_delay_slot(PC);

		PC += get_SB18(instruction);
	}
	else if (skip_delay_slot_if_not)
	{
		PC += 4;
	}
}
