#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "debug_console.h"
#include "processor.h"
#include "processor_utils.h"

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

	switch(opcode)
	{
		case 0x00:		// R-type / SPECIAL
			r_type(opcode, instruction);
			break;

		case 0x02:		// J-type
		case 0x03:
			j_type(opcode, instruction);
			break;

		case 0x01:		// I-type
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
		case 0x08:
		case 0x09:
		case 0x0b:
		case 0x0c:
		case 0x0d:
		case 0x0e:
		case 0x0f:
		case 0x20:
		case 0x21:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x28:
		case 0x29:
		case 0x2b:
		case 0x31:
		case 0x39:
			i_type(opcode, instruction);
			break;

		case 0x0a:
			SLTI(instruction);
			break;

		case 0x10:		// co processor instructions
		case 0x11:
		case 0x12:
		case 0x13:
			ipco(opcode, instruction);
			break;

		case 0x15:
			BNEL(instruction);
			break;

		case 0x1c:		// SPECIAL2
			special2(opcode, instruction);
			break;

		case 0x1f:		// SPECIAL3
			special3(opcode, instruction);
			break;

		default:
			pdc -> log("tick: unsupported opcode %02x", opcode);
			// throw invalid
			break;
	}
}

void processor::j_type(int opcode, int instruction)
{
	if (opcode == 3)	// JAL
		registers[31] = PC;

	// FIXME shift 2 bits?
	PC = (instruction & MASK_26B) | (PC & 0x3c);
}

void processor::ipco(int opcode, int instruction)
{
	int co_processor = opcode & 0x03;
	int format = (instruction >> 21) & MASK_5B;
	int function = instruction & MASK_6B;

	pdc -> log("IPCO(%d) format %d function %d", co_processor, format, function);
}

void processor::r_type(int opcode, int instruction) // SPECIAL
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
				registers[rd] = registers[rt] << sa;
			break;

		case 0x02:		// SRL / ROTR
			if (IS_BIT_OFF0_SET(21, instruction))
				registers[rd] = rotate_right(registers[rt] & MASK_32B, sa, 32);
			else
				registers[rd] = (registers[rt] & MASK_32B) >> sa;
			break;

		case 0x03:		// SRA
			registers[rd] = sign_extend(registers[rt] >> sa, 32 - sa);
			break;

		case 0x04:		// SLLV
			registers[rd] = registers[rt] << sa;
			break;

		case 0x06:		// SRLV / ROTRV
			if (IS_BIT_OFF0_SET(21, instruction))
				registers[rd] = rotate_right(registers[rt] & MASK_32B, registers[rs] & MASK_5B, 32);
			else
				registers[rd] = (registers[rt] & MASK_32B) >> (registers[rs] & MASK_5B);
			break;

		case 0x08:		// JR
			tick();	// need to execute the next instruction - what if it is a JR as well? FIXME
			PC = registers[rs];
			break;

		case 0x09:		// JALR
			tick();
			registers[rd] = PC + 4;
			PC = registers[rs];
			break;

		case 0x0a:		// MOVZ
			if (registers[rt] == 0)
				registers[rd] = registers[rs];
			break;

		case 0x0b:		// MOVN
			if (registers[rt])
				registers[rd] = registers[rs];
			break;

		case 0x0d:		// BREAK for debugging FIXME
			pdc -> log("BREAK");
			break;

		case 0x10:		// MFHI
			registers[rd] = HI;
			break;

		case 0x11:		// MTHI
			HI = registers[rs];
			break;

		case 0x12:		// MFLO
			registers[rd] = LO;
			break;

		case 0x13:		// MTLO
			LO = registers[rs];
			break;

		case 0x24:		// AND
			registers[rd] = registers[rs] & registers[rt];
			break;

		case 0x25:		// OR
			registers[rd] = registers[rs] | registers[rt];
			break;

		case 0x26:		// XOR
			registers[rd] = registers[rs] ^ registers[rt];
			break;

		case 0x2a:		// SLT
			if (untwos_complement(registers[rs], 32) < untwos_complement(registers[rt], 32))
				registers[rd] = 1;
			else
				registers[rd] = 0;
			break;

		case 0x2b:		// SLTU
			if (registers[rs] < registers[rt])
				registers[rd] = 1;
			else
				registers[rd] = 0;
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

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = untwos_complement(offset, 16);
	int b18_signed_offset = untwos_complement(offset << 2, 18);

	int bgezal = (instruction >> 16) & MASK_5B;

	int temp_32b = -1, address = -1;

	switch(opcode)
	{
		case 0x01:		// BAL
			if (bgezal == 0x10)
			{
				registers[31] = PC + 4;

				PC += b18_signed_offset;
			}
			else
			{
				pdc -> log("i-type, opcode 0x01, bgezal 0x%02x", bgezal);
			}
			break;
			
		case 0x04:		// BEQ
			PC += b18_signed_offset;
			break;

		case 0x05:		// BNE
			if (registers[rs] != registers[rt])
				PC += b18_signed_offset;
			break;

		case 0x0b:		// SLTIU
			if (registers[rs] < sign_extend_16b(immediate))
				registers[rt] = 1;
			else
				registers[rt] = 0;
			break;

		case 0x0c:		// ANDI
			registers[rt] = registers[rs] & immediate;
			break;

		case 0x0d:		// ORI
			registers[rt] = registers[rs] | immediate;
			break;

		case 0x0e:		// XORI
			registers[rt] = registers[rs] ^ immediate;
			break;

		case 0x0f:		// LUI
			registers[rt] = immediate << 16;
			break;

		case 0x20:		// LB
			address = registers[base] + offset_s;
			if (!pmb -> read_8b(address, &temp_32b))
				pdc -> log("i-type read 8b from %08x failed", address);
			registers[rt] = sign_extend_8b(temp_32b);
			break;

		case 0x21:		// LH
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
				registers[rt] = sign_extend_16b(temp_32b);
			}
			break;

		case 0x24:		// LBU
			address = registers[base] + offset_s;
			if (!pmb -> read_8b(address, &temp_32b))
				pdc -> log("i-type read 8b from %08x failed", address);
			registers[rt] = temp_32b;
			break;

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
				registers[rt] = temp_32b;
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
				registers[rt] = temp_32b;
			}
			break;

		case 0x28:		// SB
			address = registers[base] + offset_s;
			temp_32b = registers[rt];
			if (!pmb -> write_8b(address, temp_32b))
				pdc -> log("i-type write 8b to %08x failed", address);
			break;

		case 0x29:		// SH
			address = registers[base] + offset_s;
			if (address & 1)
			{
				pdc -> log("i-type write 16b to %08x: unaligned", address);
				// FIXME throw address error exception
			}
			else
			{
				temp_32b = registers[rt];
				if (!pmb -> write_16b(address, temp_32b))
					pdc -> log("i-type write 16b to %08x failed", address);
			}
			break;

		case 0x2b:		// SW
			address = registers[base] + offset_s;
			if (address & 3)
			{
				pdc -> log("i-type write 32b to %08x: unaligned", address);
				// FIXME throw address error exception
			}
			else
			{
				temp_32b = registers[rt];
				if (!pmb -> write_32b(address, temp_32b))
					pdc -> log("i-type write 32b to %08x failed", address);
			}
			break;

		default:
			// throw invalid
			pdc -> log("i-type unsupported function %02x", opcode);
			break;
	}
}

void processor::special2(int opcode, int instruction)
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
			registers[rd] = temp_64b & MASK_32B;
			// LO/HI are said to be unpredictable after this command
			break;

		case 0x1C:		// CLZ
			registers[rd] = count_leading_zeros(32, registers[rs]);
			// FIXME also in rt?
			break;

		case 0x21:		// CLO
			registers[rd] = count_leading_ones(32, registers[rs]);
			// FIXME also in rt?
			break;

		default:
			pdc -> log("special2 clo %02x not supported", clo);
			break;
	}
}

void processor::BNEL(int instruction)
{
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;
	int immediate = instruction & MASK_16B;

	int offset = immediate << 2;
	int b18_signed_offset = untwos_complement(offset, 18);

	if (rt != rs)
	{
		int new_PC = PC + b18_signed_offset;

		tick();

		PC = new_PC;
	}
}

int processor::get_register(int nr)
{
	assert(nr >=0 && nr <= 31);

	return registers[nr];
}

void processor::set_register(int nr, int value)
{
	assert(nr >=0 && nr <= 31);

	registers[nr] = value;
}

bool processor::get_mem_32b(int offset, int *value)
{
	return pmb -> read_32b(offset, value);
}

void processor::special3(int opcode, int instruction)
{
	int function = instruction & MASK_6B;
	int sub_function = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;

	switch(function)
	{
		case 0x20:
			if (sub_function == 0x18)	// SEH
				registers[rt] = sign_extend_16b(registers[rd] & MASK_16B);
			else if (sub_function == 0x10)	// SEB
				registers[rt] = sign_extend_8b(registers[rd] & MASK_8B);
			break;

		default:
			pdc -> log("special3 function %02x not supported", function);
			break;
	}
}

void processor::SLTI(int instruction)
{
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;
	int immediate = instruction & MASK_16B;

	// FIXME use register as 32B?
	if (untwos_complement(registers[rs], 32) < untwos_complement(immediate, 16))
		registers[rt] = 1;
	else
		registers[rt] = 0;
}

char * processor::decode_to_text(int instr)
{
	int opcode = (instr >> 26) & MASK_26B;

	// NOTE: the conversion to a char-pointer is because in the future
	// this code might disassemble the whole instruction with parameters and all

	if (opcode == 0)			// R-type
	{
		int function = instr & MASK_6B;

		switch(function)
		{
			case 0x00:
				return (char *)"SLL/NOP";
			case 0x02:
				return (char *)"SRL";
			case 0x03:
				return (char *)"SRA";
			case 0x04:
				return (char *)"SLLV";
			case 0x06:
				return (char *)"SRLV";
			case 0x07:
				return (char *)"SRAV";
			case 0x08:
				return (char *)"JR";
			case 0x09:
				return (char *)"JALR";
			case 0x0c:
				return (char *)"SYSCALL";
			case 0x0d:
				return (char *)"BREAK";
			case 0x10:
				return (char *)"MFHI";
			case 0x11:
				return (char *)"MTHI";
			case 0x12:
				return (char *)"MFLO";
			case 0x13:
				return (char *)"MTLO";
			case 0x18:
				return (char *)"MULT";
			case 0x19:
				return (char *)"MULTU";
			case 0x1a:
				return (char *)"DIV";
			case 0x1b:
				return (char *)"DIVU";
			case 0x20:
				return (char *)"ADD";
			case 0x21:
				return (char *)"ADDU";
			case 0x22:
				return (char *)"SUB";
			case 0x23:
				return (char *)"SUBU";
			case 0x24:
				return (char *)"AND";
			case 0x25:
				return (char *)"OR";
			case 0x26:
				return (char *)"XOR";
			case 0x27:
				return (char *)"NOR";
			case 0x2a:
				return (char *)"SLT";
			case 0x2b:
				return (char *)"SLTU";
			default:
				return (char *)"R/???";
		}
	}
	else if (opcode == 2 || opcode == 3)	// J-type
	{
		if (opcode == 2)
			return (char *)"J";

		return (char *)"JAL";
	}
	else if (opcode != 16 && opcode != 17 && opcode != 18 && opcode != 19) // I-type
	{
		switch(opcode)
		{
			case 0x01:
				return (char *)"BLTZ/BGEZ";
			case 0x04:
				return (char *)"BEQ";
			case 0x05:
				return (char *)"BNE";
			case 0x06:
				return (char *)"BLEZ";
			case 0x07:
				return (char *)"BGTZ";
			case 0x08:
				return (char *)"ADDI";
			case 0x09:
				return (char *)"ADDIU";
			case 0x0a:
				return (char *)"SLTI";
			case 0x0b:
				return (char *)"SLTIU";
			case 0x0c:
				return (char *)"ANDI";
			case 0x0d:
				return (char *)"ORI";
			case 0x0e:
				return (char *)"XORI";
			case 0x0f:
				return (char *)"LUI";
			case 0x20:
				return (char *)"LB";
			case 0x21:
				return (char *)"LH";
			case 0x23:
				return (char *)"LW";
			case 0x24:
				return (char *)"LBU";
			case 0x25:
				return (char *)"LHU";
			case 0x28:
				return (char *)"SB";
			case 0x29:
				return (char *)"SH";
			case 0x2b:
				return (char *)"SW";
			case 0x31:
				return (char *)"LWCL";
			case 0x39:
				return (char *)"SWCL";
			default:
				return (char *)"I/???";
		}
	}

	return (char *)"???";
}
