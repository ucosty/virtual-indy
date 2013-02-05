#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "processor.h"
#include "processor_utils.h"

processor::processor(memory_bus *pmb_in) : pmb(pmb_in)
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

		default:
			fprintf(stderr, "tick: unsupported opcode %02x\n", opcode);
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

	fprintf(stderr, "IPCO(%d) format %d function %d\n", co_processor, format, function);
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
		case 0x00:		// NOP
			break;

		case 0x02:		// SRL / ROTR
			if (IS_BIT_OFF0_SET(21, instruction))
				registers[rd] = rotate_right(registers[rt] & MASK_32B, sa, 32);
			else
				registers[rd] = (registers[rt] & MASK_32B) >> sa;
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
			fprintf(stderr, "BREAK\n");
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

		default:
			// throw invalid
			fprintf(stderr, "r-type unsupported function %02x\n", function);
			break;
	}
}

void processor::i_type(int opcode, int instruction)
{
	int immediate = instruction & MASK_16B;

	int rs = (instruction >> 21) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate << 2;
	int b18_signed_offset = untwos_complement(offset, 18);

	int bgezal = (instruction >> 16) & MASK_5B;

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
				fprintf(stderr, "i-type, opcode 0x01, bgezal 0x%02x\n", bgezal);
			}
			break;
			
		case 0x04:		// BEQ
			PC += b18_signed_offset;
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

		default:
			// throw invalid
			fprintf(stderr, "i-type unsupported function %02x\n", opcode);
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
			fprintf(stderr, "special2 clo %02x not supported\n", clo);
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
