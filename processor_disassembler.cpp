#include "debug.h"
#include "processor.h"
#include "processor_utils.h"
#include "exceptions.h"
#include "utils.h"

std::string processor::da_logline(uint32_t instruction)
{
	uint32_t temp_32b = -1;
	uint64_t cur_PC = is_delay_slot() ? get_delay_slot_PC() : get_PC();

	std::string exception;
	bool rc = true;

	try
	{
		pmb -> read_32b(cur_PC, &temp_32b);
	}
	catch(processor_exception & pe)
	{
        exception = format("EXCEPTION %x at/for %016llx, PC: %016llx (1), sr: %08x", pe.get_cause(), pe.get_BadVAddr(), pe.get_EPC(), pe.get_status());

		rc = false;
	}

    std::string line = format("PC: %016llx %c / %x|%08x", cur_PC, is_delay_slot() ? 'D' : '.', rc, temp_32b);

	uint8_t opcode = get_opcode(instruction);

	if (opcode == 0)			// R-type
	{
		uint8_t function = instruction & MASK_6B;
		uint8_t sa = get_SA(instruction);
		uint8_t rd = get_RD(instruction);
		uint8_t rt = get_RT(instruction);
		uint8_t rs = get_RS(instruction);

		line += format("\tfu %02x", function);
		line += format("\tsa %08x", sa);
		line += format("\trd %s|%016llx", reg_to_name(rd), registers[rd]);
		line += format("\trt %s|%016llx", reg_to_name(rt), registers[rt]);
		line += format("\trs %s|%016llx", reg_to_name(rs), registers[rs]);

	}
	else if (opcode == 2 || opcode == 3)	// J-type
	{
		uint64_t new_PC = ((instruction & MASK_26B) << 2) | (PC & 0xfffffffff0000000);

		line += format("\tnPC %016llx", new_PC);
	}
	else if (opcode != 16 && opcode != 17 && opcode != 18 && opcode != 19) // I-type
	{
		int immediate = get_immediate(instruction);
		int immediate_s = int16_t(immediate);

		uint8_t rs = get_RS(instruction);
		uint8_t rt = get_RT(instruction);

		line += format("\tim: %04x", immediate);
        line += format("\tims: %x", immediate_s);
		line += format("\trt %s|%016llx", reg_to_name(rt), registers[rt]);
		line += format("\trs %s|%016llx", reg_to_name(rs), registers[rs]);
	}
	else if (opcode == 16)	// COP0
	{
		if (IS_BIT_OFF0_SET(21, instruction))	//
		{
			uint8_t function = instruction & MASK_6B;

			line += format("\t1 fu %02x", function);
		}
		else
		{
			uint8_t function = (instruction >> 21) & MASK_5B;
			uint8_t sel = instruction & MASK_3B;
			uint8_t rd = get_RD(instruction);
			uint8_t rt = get_RT(instruction);

			line += format("\t0 fu %02x", function);

            line += format("\tsel %x", sel);
			line += format("\trd %02x", rd);
			line += format("\trt %02x", rt);
			line += format("\tC0 %08x", get_C0_register(rd, sel));
		}
	}

	line += "\t";
	line += decode_to_text(instruction);

	if (exception.length())
	{
		line += "\t";
		line += exception;
	}

	return line;
}

const char * processor::reg_to_name(uint8_t reg)
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

std::string processor::decode_to_text(uint32_t instruction)
{
	uint8_t opcode = get_opcode(instruction);

	if (opcode == 0)			// R-type
	{
		uint8_t function = instruction & MASK_6B;
		uint8_t sa = get_SA(instruction);
		uint8_t rd = get_RD(instruction);
		uint8_t rt = get_RT(instruction);
		uint8_t rs = get_RS(instruction);

		switch(function)
		{
			case 0x00:
				if (sa == 0)
					return "NOP";

                return format("SLL %s,%s,%x", reg_to_name(rd), reg_to_name(rt), sa);

			case 0x02:
				if ((rs & 1) == 0)
                    return format("SRL %s,%s,%x", reg_to_name(rd), reg_to_name(rt), sa);
				else
					return "SRL?";
			case 0x03:
				if ((rs & 1) == 0)
                    return format("SRA %s,%s,%x", reg_to_name(rd), reg_to_name(rt), sa);
				else
					return "SRA?";
			case 0x04:
                return format("SLLV %s,%s,%x", reg_to_name(rd), reg_to_name(rt), reg_to_name(rs));
			case 0x06:
                return format("SRLV %s,%s,%x", reg_to_name(rd), reg_to_name(rt), reg_to_name(rs));
			case 0x07:
                return format("SRAV %s,%s,%x", reg_to_name(rd), reg_to_name(rt), reg_to_name(rs));
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
			return format("J 0x%08x", offset);

		return format("JAL 0x%08x", offset);
	}
	else if (opcode != 16 && opcode != 17 && opcode != 18 && opcode != 19) // I-type
	{
		int immediate = get_immediate(instruction);
		int immediate_s = int16_t(immediate);

		uint8_t rs = get_RS(instruction);
		uint8_t rt = get_RT(instruction);

		switch(opcode)
		{
			case 0x01:
				return "BLTZ/BGEZ";
			case 0x04:
                return format("BEQ %s,%s,%x", reg_to_name(rs), reg_to_name(rt), immediate_s);
			case 0x05:
                return format("BNE %s,%s,%x", reg_to_name(rs), reg_to_name(rt), immediate_s);
			case 0x06:
                return format("BLEZ %s,%x", reg_to_name(rs), immediate_s);
			case 0x07:
                return format("BGTZ %s,%x", reg_to_name(rs), immediate_s);
			case 0x08:
                return format("ADDI %s,%s,%x", reg_to_name(rt), reg_to_name(rs), immediate_s);
			case 0x09:
                return format("ADDIU %s,%s,%x", reg_to_name(rt), reg_to_name(rs), immediate_s);
			case 0x0a:
                return format("SLTI %s,%s,%x", reg_to_name(rt), reg_to_name(rs), immediate_s);
			case 0x0b:
                return format("SLTIU %s,%s,%x", reg_to_name(rt), reg_to_name(rs), immediate_s);
			case 0x0c:
                return format("ANDI %s,%s,%x", reg_to_name(rt), reg_to_name(rs), immediate_s);
			case 0x0d:
                return format("ORI %s,%s,%x", reg_to_name(rt), reg_to_name(rs), immediate_s);
			case 0x0e:
                return format("XORI %s,%s,%x", reg_to_name(rt), reg_to_name(rs), immediate_s);
			case 0x0f:
				return format("LUI %s, 0x%08x", reg_to_name(rt), immediate << 16);
			case 0x20:
                return format("LB %s, %x(%s)", reg_to_name(rt), immediate_s, reg_to_name(rs));
			case 0x21:
                return format("LH %s, %x(%s)", reg_to_name(rt), immediate_s, reg_to_name(rs));
			case 0x23:
                return format("LW %s, %x(%s)", reg_to_name(rt), immediate_s, reg_to_name(rs));
			case 0x24:
                return format("LBU %s, %x(%s)", reg_to_name(rt), immediate_s, reg_to_name(rs));
			case 0x25:
                return format("LHU %s, %x(%s)", reg_to_name(rt), immediate_s, reg_to_name(rs));
			case 0x28:
                return format("SB %x(%s), %s", immediate_s, reg_to_name(rs), reg_to_name(rt));
			case 0x29:
                return format("SH %x(%s), %s", immediate_s, reg_to_name(rs), reg_to_name(rt));
			case 0x2b:
                return format("SW %x(%s), %s", immediate_s, reg_to_name(rs), reg_to_name(rt));
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
			uint8_t function = instruction & MASK_6B;

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
			uint8_t function = (instruction >> 21) & MASK_5B;
			uint8_t sel = instruction & MASK_3B;
			uint8_t rd = get_RD(instruction);
			uint8_t rt = get_RT(instruction);

			switch(function)
			{
				case 0x00:
                    return format("MFC0 %s,%x,%x", reg_to_name(rt), rd, sel);
				case 0x01:
					return "TLBR";
				case 0x02:
					return "TLBWI";
				case 0x04:
                    return format("MTC0 %s,%x,%x", reg_to_name(rt), rd, sel);
				case 0x06:
					return "TLBWR";
				case 0x08:
					return "TLBP";
				case 0x0b:
					if (IS_BIT_OFF0_SET(5, instruction))
						return "EI";
					return "DI";
				default:
					return format("COP0_0/%02x", function);
			}
		}
	}
	else if (opcode == 17)	// COP1
	{
		// uint8_t rt = get_RT(instruction);

        // return format("MFC1 %s,%x", reg_to_name(rt), (instruction >> 11) & 31);
		return "COP1_command";
	}

	return "???";
}
