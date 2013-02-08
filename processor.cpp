#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "debug_console.h"
#include "processor.h"
#include "processor_utils.h"
#include "utils.h"

processor::processor(debug_console *pdc_in, memory_bus *pmb_in) : pdc(pdc_in), pmb(pmb_in)
{
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
	int instruction = -1;

	if (PC & 0x03)
	{
		// address exception
	}

	if (!pmb -> read_32b(PC, &instruction))
	{
		// processor exception FIXME
	}

	PC += 4;

	int opcode = (instruction >> 26) & MASK_6B;

	if (opcode == 0)			// R-type / SPECIAL
	{
		r_type(instruction);
	}
	else if (opcode == 2 || opcode == 3)
	{
		j_type(opcode, instruction);
	}
	else if (opcode < 16 || opcode > 19) // I-type
	{
		i_type(opcode, instruction);
	}
	else if (opcode == 16)	// COP0
	{
		COP0(instruction);
	}
	else
	{
		pdc -> log("tick: unsupported opcode %02x", opcode);
	}
}

void processor::j_type(int opcode, int instruction)
{
	ASSERT(opcode == 2 || opcode == 3);

	if (opcode == 3)	// JAL
		set_register(31, PC);

	int new_PC = ((instruction & MASK_26B) << 2) | (PC & 0xFC000000);

	tick(); // delay slot

	PC = new_PC;
}

void processor::COP0(int instruction)
{
	if (IS_BIT_OFF0_SET(21, instruction))	//
	{
		int function = instruction & MASK_6B;

		switch(function)
		{
			case 0x18:	// ERET
				PC = get_C0_register(14, 0);	// EPC (FIXME: sel 0?)
				status_register >>= 4;
				// FIXME
				break;

			default:
				pdc -> log("cop0: don't know how to handle function %02x (1)", function);
		}
	}
	else
	{
		int function = (instruction >> 21) & MASK_5B;
		int rd = (instruction >> 11) & MASK_5B;
		int rt = (instruction >> 16) & MASK_5B;

		int sel = instruction & MASK_3B;

		switch(function)
		{
			case 0x00:	// MFC0
				set_register(rt, get_C0_register(registers[rd], sel));
				break;

			case 0x04:	// MTC0
				set_C0_register(registers[rd], sel, registers[rt]);
				break;

			case 0x0b:	// DI/EI
				if (IS_BIT_OFF0_SET(5, instruction))
				{
					set_register(rt, status_register);
					SET_BIT(SR_EI, status_register);
				}
				else
				{
					set_register(rt, status_register);
					RESET_BIT(SR_EI, status_register);
				}
				break;

			default:
				pdc -> log("cop0: don't know how to handle function %02x (0)", function);
		}
	}
}

void processor::r_type(int instruction) // SPECIAL
{
	int function = instruction & MASK_6B;
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	switch(function)
	{
		case 0x00:		// NOP / SLL
			if (sa) // if sa==0 then NOP
				set_register(rd, registers[rt] << sa);
			break;

		case 0x02:		// SRL / ROTR
			if (IS_BIT_OFF0_SET(21, instruction))
				set_register(rd, rotate_right(registers[rt] & MASK_32B, sa, 32));
			else
				set_register(rd, (registers[rt] & MASK_32B) >> sa);
			break;

		case 0x03:		// SRA
			set_register(rd, sign_extend(registers[rt] >> sa, 32 - sa));
			break;

		case 0x04:		// SLLV
			set_register(rd, registers[rt] << sa);
			break;

		case 0x06:		// SRLV / ROTRV
			if (IS_BIT_OFF0_SET(21, instruction))
				set_register(rd, rotate_right(registers[rt] & MASK_32B, registers[rs] & MASK_5B, 32));
			else
				set_register(rd, (registers[rt] & MASK_32B) >> (registers[rs] & MASK_5B));
			break;

		case 0x08:		// JR
			tick();	// need to execute the next instruction - what if it is a JR as well? FIXME
			PC = registers[rs];
			break;

		case 0x09:		// JALR
			tick();
			set_register(rd, PC + 4);
			PC = registers[rs];
			break;

		case 0x0a:		// MOVZ
			if (registers[rt] == 0)
				set_register(rd, registers[rs]);
			break;

		case 0x0b:		// MOVN
			if (registers[rt])
				set_register(rd, registers[rs]);
			break;

		case 0x0d:		// BREAK for debugging FIXME
			pdc -> log("BREAK");
			break;

		case 0x10:		// MFHI
			set_register(rd, HI);
			break;

		case 0x11:		// MTHI
			HI = registers[rs];
			break;

		case 0x12:		// MFLO
			set_register(rd, LO);
			break;

		case 0x13:		// MTLO
			LO = registers[rs];
			break;

		case 0x21:		// ADDU
			set_register(rd, (registers[rs] + registers[rt]) & MASK_32B);
			break;

		case 0x23:		// SUBU
			set_register(rd, (registers[rs] - registers[rt]) & MASK_32B);
			break;

		case 0x24:		// AND
			set_register(rd, registers[rs] & registers[rt]);
			break;

		case 0x25:		// OR
			set_register(rd, registers[rs] | registers[rt]);
			break;

		case 0x26:		// XOR
			set_register(rd, registers[rs] ^ registers[rt]);
			break;

		case 0x2a:		// SLT
			if (untwos_complement(registers[rs], 32) < untwos_complement(registers[rt], 32))
				set_register(rd, 1);
			else
				set_register(rd, 0);
			break;

		case 0x2b:		// SLTU
			if (registers[rs] < registers[rt])
				set_register(rd, 1);
			else
				set_register(rd, 0);
			break;

		default:
			// throw invalid
			pdc -> log("r-type unsupported function %02x", function);
			break;
	}
}

void processor::i_type(int opcode, int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement(immediate, 16);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement(offset << 2, 18);

	int temp_32b = -1, address = -1;

	switch(opcode)
	{
		case 0x01:		// BGEZAL
			regimm(instruction);
			break;
			
		case 0x04:		// BEQ
			if (registers[rs] == registers[rt])
			{
				int new_PC = PC + b18_signed_offset;
				tick();
				PC = new_PC;
			}
			break;

		case 0x05:		// BNE
		case 0x15:		// BNEL
			if (registers[rs] != registers[rt])
			{
				int new_PC = PC + b18_signed_offset;
				tick();
				PC = new_PC;
			}
			break;

		case 0x08:		// ADDI
			set_register(rt, registers[rs] + immediate_s);
			// FIXME handle overflows
			break;

		case 0x09:		// ADDIU
			set_register(rt, registers[rs] + immediate_s);
			break;

		case 0x0a:		// SLTI
			if (untwos_complement(registers[rs], 32) < untwos_complement(immediate, 16))
				set_register(rt, 1);
			else
				set_register(rt, 0);
			break;

		case 0x0b:		// SLTIU
			if (registers[rs] < sign_extend_16b(immediate))
				set_register(rt, 1);
			else
				set_register(rt, 0);
			break;

		case 0x0c:		// ANDI
			set_register(rt, registers[rs] & immediate);
			break;

		case 0x0d:		// ORI
			set_register(rt, registers[rs] | immediate);
			break;

		case 0x0e:		// XORI
			set_register(rt, registers[rs] ^ immediate);
			break;

		case 0x0f:		// LUI
			set_register(rt, immediate << 16);
			break;

		case 0x1c:		// SPECIAL2
			special2(instruction);
			break;

		case 0x1f:		// SPECIAL3
			special3(instruction);
			break;


		case 0x20:		// LB
		case 0x24:		// LBU
			address = registers[base] + offset_s;
			if (!pmb -> read_8b(address, &temp_32b))
				pdc -> log("i-type read 8b from %08x failed", address);
			if (opcode == 0x24)
				set_register(rt, temp_32b);
			else
				set_register(rt, sign_extend_8b(temp_32b));
			break;

		case 0x21:		// LH
		case 0x25:		// LHU
			address = registers[base] + offset_s;
			if (address & 1)
			{
				pdc -> log("i-type read 16b from %08x: unaligned", address);
				// FIXME throw address error exception
			}
			else
			{
				if (!pmb -> read_16b(address, &temp_32b))
					pdc -> log("i-type read 16b from %08x failed", address);
				if (opcode == 0x25)
					set_register(rt, temp_32b);
				else
					set_register(rt, sign_extend_16b(temp_32b));
			}
			break;

		case 0x23:		// LW
		case 0x30:		// LL
			address = registers[base] + offset_s;
			if (address & 3)
			{
				pdc -> log("i-type read 32b from %08x: unaligned", address);
				// FIXME throw address error exception
			}
			else
			{
				if (!pmb -> read_32b(address, &temp_32b))
					pdc -> log("i-type read 32b from %08x failed", address);
				set_register(rt, temp_32b);
			}
			break;

		case 0x28:		// SB
			address = registers[base] + offset_s;
			temp_32b = registers[rt];
			if (!pmb -> write_8b(address, temp_32b))
				pdc -> log("i-type write 8b %02x to %08x failed", registers[rt] & 0xff, address);
			break;

		case 0x29:		// SH
			address = registers[base] + offset_s;
			if (address & 1)
			{
				pdc -> log("i-type write 16b %04x to %08x: unaligned", registers[rt] & 0xffff, address);
				// FIXME throw address error exception
			}
			else
			{
				temp_32b = registers[rt];
				if (!pmb -> write_16b(address, temp_32b))
					pdc -> log("i-type write 16b %04x to %08x failed", registers[rt] & 0xffff, address);
			}
			break;

		case 0x2b:		// SW
			address = registers[base] + offset_s;
			if (address & 3)
			{
				pdc -> log("i-type write 32b %08x to %08x: unaligned", registers[rt], address);
				// FIXME throw address error exception
			}
			else
			{
				temp_32b = registers[rt];
				if (!pmb -> write_32b(address, temp_32b))
					pdc -> log("i-type write 32b %08x to %08x failed", registers[rt], address);
			}
			break;

		default:
			// throw invalid
			pdc -> log("i-type unsupported function %02x", opcode);
			break;
	}
}

void processor::special2(int instruction)
{
	int clo = instruction & MASK_6B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	long long int temp_64b = -1;

	switch(clo)
	{
		case 0x02:		// MUL
			temp_64b = int(registers[rs]) * int(registers[rt]);
			set_register(rd, temp_64b & MASK_32B);
			// LO/HI are said to be unpredictable after this command
			break;

		case 0x1C:		// CLZ
			set_register(rd, count_leading_zeros(32, registers[rs]));
			// FIXME also in rt?
			break;

		case 0x21:		// CLO
			set_register(rd, count_leading_ones(32, registers[rs]));
			// FIXME also in rt?
			break;

		default:
			pdc -> log("special2 clo %02x not supported", clo);
			break;
	}
}

void processor::regimm(int instruction)
{
	int immediate = instruction & MASK_16B;

	int rs = (instruction >> 21) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int function = rt;

	int offset = immediate;
	int b18_signed_offset = untwos_complement(offset << 2, 18);

	switch(function)
	{
		case 0x01:		// BGEZ
		case 0x03:		// BGEZL
			if (untwos_complement(registers[rs], 16) >= 0)
				PC += b18_signed_offset;
			break;

		case 0x11:		// BAL / BGEZAL
		case 0x13:		// BGEZALL
			if (rs == 31)
				pdc -> log("i-type / REGIMM / BGEZ using rs 31");
			if (untwos_complement(registers[rs], 16) >= 0)
			{
				set_register(31, PC + 4);

				PC += b18_signed_offset;
			}
			break;

		default:
			pdc -> log("i-type, opcode 0x01, REGIMM 0x%02x unknown", function);
	}
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

void processor::special3(int instruction)
{
	int function = instruction & MASK_6B;
	int sub_function = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;

	switch(function)
	{
		case 0x20:
			if (sub_function == 0x18)	// SEH
				set_register(rt, sign_extend_16b(registers[rd] & MASK_16B));
			else if (sub_function == 0x10)	// SEB
				set_register(rt, sign_extend_8b(registers[rd] & MASK_8B));
			break;

		default:
			pdc -> log("special3 function %02x not supported", function);
			break;
	}
}

const char * processor::reg_to_name(int reg)
{
	ASSERT(reg >= 0 && reg <= 31);

	switch(reg)
	{
		case 0:			// always zero
			return "$zero";
		case 1:			// reserved for assembler
			return "$at";
		case 2:			// first and second return value
			return "$v0";
		case 3:
			return "$v1";
		case 4:			// first four arguments for functions
			return "$a0";
		case 5:
			return "$a1";
		case 6:
			return "$a2";
		case 7:
			return "$a3";
		case 8:			// temporary registers
			return "$t0";
		case 9:
			return "$t1";
		case 10:
			return "$t2";
		case 11:
			return "$t3";
		case 12:
			return "$t4";
		case 13:
			return "$t5";
		case 14:
			return "$t6";
		case 15:
			return "$t7";
		case 16:		// saved registers
			return "$s0";
		case 17:
			return "$s1";
		case 18:
			return "$s2";
		case 19:
			return "$s3";
		case 20:
			return "$s4";
		case 21:
			return "$s5";
		case 22:
			return "$s6";
		case 23:
			return "$s7";
		case 24:
			return "$s8";
		case 25:
			return "$s9";
		case 26:		// reserved for kernel
			return "$k0";
		case 27:
			return "$k1";
		case 28:		// global pointer
			return "$gp";
		case 29:		// stack pointer
			return "$sp";
		case 30:		// frame pointer
			return "$fp";
		case 31:		// return address
			return "$ra";
	}

	return "??";
}

std::string processor::decode_to_text(int instruction)
{
	int opcode = (instruction >> 26) & MASK_6B;

	if (opcode == 0)			// R-type
	{
		int function = instruction & MASK_6B;
		int sa = (instruction >> 6) & MASK_5B;
		int rd = (instruction >> 11) & MASK_5B;
		int rt = (instruction >> 16) & MASK_5B;
		int rs = (instruction >> 21) & MASK_5B;

		switch(function)
		{
			case 0x00:
				if (sa == 0)
					return "NOP";

				return format("SLL %s,%s,%d", reg_to_name(rd), reg_to_name(rt), sa);

			case 0x02:
				if ((rs & 1) == 0)
					return format("SRL %s,%s,%d", reg_to_name(rd), reg_to_name(rt), sa);
				else
					return "SRL?";
			case 0x03:
				if ((rs & 1) == 0)
					return format("SRA %s,%s,%d", reg_to_name(rd), reg_to_name(rt), sa);
				else
					return "SRA?";
			case 0x04:
				return format("SLLV %s,%s,%d", reg_to_name(rd), reg_to_name(rt), reg_to_name(rs));
			case 0x06:
				return format("SRLV %s,%s,%d", reg_to_name(rd), reg_to_name(rt), reg_to_name(rs));
			case 0x07:
				return format("SRLV %s,%s,%d", reg_to_name(rd), reg_to_name(rt), reg_to_name(rs));
			case 0x08:
				return format("JR %s", reg_to_name(rs));
			case 0x09:
				return format("JALR %s,%s", reg_to_name(rd), reg_to_name(rs));
			case 0x0c:
				return "SYSCALL";
			case 0x0d:
				return "BREAK";
			case 0x10:
				return format("MFHI %s", reg_to_name(rd));
			case 0x11:
				return format("MTHI %s", reg_to_name(rs));
			case 0x12:
				return format("MFLO %s", reg_to_name(rd));
			case 0x13:
				return format("MTLO %s", reg_to_name(rs));
			case 0x18:
				return format("MULT %s,%s", reg_to_name(rs), reg_to_name(rt));
			case 0x19:
				return format("MULTU %s,%s", reg_to_name(rs), reg_to_name(rt));
			case 0x1a:
				return format("DIV %s,%s", reg_to_name(rs), reg_to_name(rt));
			case 0x1b:
				return format("DIVU %s,%s", reg_to_name(rs), reg_to_name(rt));
			case 0x20:
				return format("ADD %s,%s,%s", reg_to_name(rd), reg_to_name(rs), reg_to_name(rt));
			case 0x21:
				return format("ADDU %s,%s,%s", reg_to_name(rd), reg_to_name(rs), reg_to_name(rt));
			case 0x22:
				return format("SUB %s,%s,%s", reg_to_name(rd), reg_to_name(rs), reg_to_name(rt));
			case 0x23:
				return format("SUBU %s,%s,%s", reg_to_name(rd), reg_to_name(rs), reg_to_name(rt));
			case 0x24:
				return format("AND %s,%s,%s", reg_to_name(rd), reg_to_name(rs), reg_to_name(rt));
			case 0x25:
				return format("OR %s,%s,%s", reg_to_name(rd), reg_to_name(rs), reg_to_name(rt));
			case 0x26:
				return format("XOR %s,%s,%s", reg_to_name(rd), reg_to_name(rs), reg_to_name(rt));
			case 0x27:
				return format("NOR %s,%s,%s", reg_to_name(rd), reg_to_name(rs), reg_to_name(rt));
			case 0x2a:
				return format("SLT %s,%s,%s", reg_to_name(rd), reg_to_name(rs), reg_to_name(rt));
			case 0x2b:
				return format("SLTU %s,%s,%s", reg_to_name(rd), reg_to_name(rs), reg_to_name(rt));
			default:
				return "R/???";
		}
	}
	else if (opcode == 2 || opcode == 3)	// J-type
	{
		int offset = (instruction & MASK_26B) << 2;

		if (opcode == 2)
			format("J 0x%08x", offset);

		return format("JAL 0x%08x", offset);
	}
	else if (opcode != 16 && opcode != 17 && opcode != 18 && opcode != 19) // I-type
	{
		int immediate = instruction & MASK_16B;
		int immediate_s = untwos_complement(immediate, 16);

		int rs = (instruction >> 21) & MASK_5B;
		int rt = (instruction >> 16) & MASK_5B;

		switch(opcode)
		{
			case 0x01:
				return "BLTZ/BGEZ";
			case 0x04:
				return format("BEQ %s,%s,%d", reg_to_name(rs), reg_to_name(rt), immediate_s);
			case 0x05:
				return format("BNE %s,%s,%d", reg_to_name(rs), reg_to_name(rt), immediate_s);
			case 0x06:
				return format("BLEZ %s,%d", reg_to_name(rs), immediate_s);
			case 0x07:
				return format("BGTZ %s,%d", reg_to_name(rs), immediate_s);
			case 0x08:
				return format("ADDI %s,%s,%d", reg_to_name(rt), reg_to_name(rs), immediate_s);
			case 0x09:
				return format("ADDIU %s,%s,%d", reg_to_name(rt), reg_to_name(rs), immediate_s);
			case 0x0a:
				return format("SLTI %s,%s,%d", reg_to_name(rt), reg_to_name(rs), immediate_s);
			case 0x0b:
				return format("SLTIU %s,%s,%d", reg_to_name(rt), reg_to_name(rs), immediate_s);
			case 0x0c:
				return format("ANDI %s,%s,%d", reg_to_name(rt), reg_to_name(rs), immediate_s);
			case 0x0d:
				return format("ORI %s,%s,%d", reg_to_name(rt), reg_to_name(rs), immediate_s);
			case 0x0e:
				return format("XORI %s,%s,%d", reg_to_name(rt), reg_to_name(rs), immediate_s);
			case 0x0f:
				return format("LUI %s, 0x%08x", reg_to_name(rt), immediate << 16);
			case 0x20:
				return format("LB %s, %d(%s)", reg_to_name(rt), immediate_s, reg_to_name(rs));
			case 0x21:
				return format("LH %s, %d(%s)", reg_to_name(rt), immediate_s, reg_to_name(rs));
			case 0x23:
				return format("LW %s, %d(%s)", reg_to_name(rt), immediate_s, reg_to_name(rs));
			case 0x24:
				return format("LBU %s, %d(%s)", reg_to_name(rt), immediate_s, reg_to_name(rs));
			case 0x25:
				return format("LHU %s, %d(%s)", reg_to_name(rt), immediate_s, reg_to_name(rs));
			case 0x28:
				return format("SB %d(%s), %s", immediate_s, reg_to_name(rs), reg_to_name(rt));
			case 0x29:
				return format("SH %d(%s), %s", immediate_s, reg_to_name(rs), reg_to_name(rt));
			case 0x2b:
				return format("SW %d(%s), %s", immediate_s, reg_to_name(rs), reg_to_name(rt));
			case 0x31:
				return "LWCL";
			case 0x39:
				return "SWCL";
			default:
				return "I/???";
		}
	}
	else if (opcode == 16)	// COP0
	{
		if (IS_BIT_OFF0_SET(21, instruction))	//
		{
			int function = instruction & MASK_6B;

			switch(function)
			{
				case 0x18:
					return "ERET";
				default:
					return format("COP0_1/%02x", function);
			}
		}
		else
		{
			int function = (instruction >> 21) & MASK_5B;
			int sel = instruction & MASK_3B;
			int rd = (instruction >> 11) & MASK_5B;
			int rt = (instruction >> 16) & MASK_5B;

			switch(function)
			{
				case 0x00:
					return format("MFC0 %s,%d,%d", reg_to_name(rt), rd, sel);
				case 0x04:
					return format("MTC0 %s,%d,%d", reg_to_name(rt), rd, sel);
				case 0x0b:
					if (IS_BIT_OFF0_SET(5, instruction))
						return "EI";
					return "DI";
				default:
					return format("COP0_0/%02x", function);
			}
		}
	}

	return "???";
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
