#include "processor.h"
#include "processor_utils.h"
#include "exceptions.h"

void processor::init_i_type()
{
	i_type_methods[0] = &processor::i_type_00;
	i_type_methods[1] = &processor::i_type_01;
	i_type_methods[2] = &processor::i_type_02;
	i_type_methods[3] = &processor::i_type_03;
	i_type_methods[4] = &processor::i_type_04;
	i_type_methods[5] = &processor::i_type_05;
	i_type_methods[6] = &processor::i_type_06;
	i_type_methods[7] = &processor::i_type_07;
	i_type_methods[8] = &processor::i_type_08;
	i_type_methods[9] = &processor::i_type_09;
	i_type_methods[10] = &processor::i_type_0a;
	i_type_methods[11] = &processor::i_type_0b;
	i_type_methods[12] = &processor::i_type_0c;
	i_type_methods[13] = &processor::i_type_0d;
	i_type_methods[14] = &processor::i_type_0e;
	i_type_methods[15] = &processor::i_type_0f;
	i_type_methods[16] = &processor::i_type_10;
	i_type_methods[17] = &processor::i_type_11;
	i_type_methods[18] = &processor::i_type_12;
	i_type_methods[19] = &processor::i_type_13;
	i_type_methods[20] = &processor::i_type_14;
	i_type_methods[21] = &processor::i_type_05;	// !
	i_type_methods[22] = &processor::i_type_16;
	i_type_methods[23] = &processor::i_type_17;
	i_type_methods[24] = &processor::i_type_18;
	i_type_methods[25] = &processor::i_type_19;
	i_type_methods[26] = &processor::i_type_1a;
	i_type_methods[27] = &processor::i_type_1b;
	i_type_methods[28] = &processor::i_type_1c;
	i_type_methods[29] = &processor::i_type_1d;
	i_type_methods[30] = &processor::i_type_1e;
	i_type_methods[31] = &processor::i_type_1f;
	i_type_methods[32] = &processor::i_type_20;
	i_type_methods[33] = &processor::i_type_21;
	i_type_methods[34] = &processor::i_type_22;
	i_type_methods[35] = &processor::i_type_23;
	i_type_methods[36] = &processor::i_type_20;	// !
	i_type_methods[37] = &processor::i_type_21;	// !
	i_type_methods[38] = &processor::i_type_26;
	i_type_methods[39] = &processor::i_type_27;
	i_type_methods[40] = &processor::i_type_28;
	i_type_methods[41] = &processor::i_type_29;
	i_type_methods[42] = &processor::i_type_2a;
	i_type_methods[43] = &processor::i_type_2b;
	i_type_methods[44] = &processor::i_type_2c;
	i_type_methods[45] = &processor::i_type_2d;
	i_type_methods[46] = &processor::i_type_2e;
	i_type_methods[47] = &processor::i_type_2f;
	i_type_methods[48] = &processor::i_type_23;	// !
	i_type_methods[49] = &processor::i_type_31;
	i_type_methods[50] = &processor::i_type_32;
	i_type_methods[51] = &processor::i_type_33;
	i_type_methods[52] = &processor::i_type_34;
	i_type_methods[53] = &processor::i_type_35;
	i_type_methods[54] = &processor::i_type_36;
	i_type_methods[55] = &processor::i_type_37;
	i_type_methods[56] = &processor::i_type_38;
	i_type_methods[57] = &processor::i_type_39;
	i_type_methods[58] = &processor::i_type_3a;
	i_type_methods[59] = &processor::i_type_3b;
	i_type_methods[60] = &processor::i_type_3c;
	i_type_methods[61] = &processor::i_type_3d;
	i_type_methods[62] = &processor::i_type_3e;
	i_type_methods[63] = &processor::i_type_3f;
}

void processor::i_type_00(uint32_t instruction) // R-type / SPECIAL
{
	uint8_t function = instruction & MASK_6B;

	(((processor*)this)->*processor::r_type_methods[function])(instruction);

	cycles += 4;
}

void processor::i_type_02(uint32_t instruction)
{
	j_type(2, instruction);

	cycles += 3;
}

void processor::i_type_03(uint32_t instruction)
{
	j_type(3, instruction);

	cycles += 3;
}

void processor::i_type_10(uint32_t instruction)
{
	COP0(instruction);
}

void processor::i_type_11(uint32_t instruction)
{
	// COP1(instruction);
	pdc -> log("i_type_11 not known");
}

void processor::i_type_12(uint32_t instruction)
{
	// COP2(instruction);
	pdc -> log("i_type_12 not known");
}

void processor::i_type_13(uint32_t instruction)
{
	// COP3(instruction);
	pdc -> log("i_type_13 not known");
}

void processor::i_type_01(uint32_t instruction)	// BGEZAL
{
	regimm(instruction);
}

void processor::i_type_04(uint32_t instruction)	// BEQ
{
	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	int offset = instruction & MASK_16B;
	int b18_signed_offset = int16_t(offset) << 2;

	cycles += 3;

	if (get_register_32b_unsigned(rs) == get_register_32b_unsigned(rt))
	{
		set_delay_slot(PC);

		PC += 4 + b18_signed_offset;
	}
}

void processor::i_type_05(uint32_t instruction)	// BNE/BNEL
{
	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	int b18_signed_offset = int16_t(instruction & MASK_16B) << 2;

	cycles += 3;

	if (get_register_32b_unsigned(rs) != get_register_32b_unsigned(rt))
	{
		set_delay_slot(PC);

		PC += 4 + b18_signed_offset;
	}
}

void processor::i_type_06(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_06 not known");
}

void processor::i_type_07(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_07 not known");
}

void processor::i_type_08(uint32_t instruction)	// ADDI
{
	int immediate_s = int16_t(instruction & MASK_16B);
	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int32_t val1 = get_register_32b_signed(rs);
	int32_t val2 = immediate_s;

	if (unlikely(test_tc_overflow_32b(val1, val2)))
		throw PE_ARITHMETIC_OVERFLOW;
	else
		set_register_32b_se(rt, val1 + val2);
}

void processor::i_type_09(uint32_t instruction)	// ADDIU
{
	int immediate_s = int16_t(instruction & MASK_16B);
	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	set_register_32b_se(rt, get_register_32b_signed(rs) + immediate_s);
}

void processor::i_type_0a(uint32_t instruction)	// SLTI
{
	int immediate_s = int16_t(instruction & MASK_16B);
	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	if (get_register_32b_signed(rs) < immediate_s)
		set_register_32b(rt, 1);
	else
		set_register_32b(rt, 0);
}

void processor::i_type_0b(uint32_t instruction)	// SLTIU
{
	int immediate = instruction & MASK_16B;
	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	if (get_register_32b_unsigned(rs) < (unsigned int)int(immediate))
		set_register_32b(rt, 1);
	else
		set_register_32b(rt, 0);
}

void processor::i_type_0c(uint32_t instruction)	// ANDI
{
	int immediate = instruction & MASK_16B;
	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	set_register_64b(rt, get_register_64b_unsigned(rs) & immediate);
}

void processor::i_type_0d(uint32_t instruction)	// ORI
{
	int immediate = instruction & MASK_16B;
	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	set_register_64b(rt, get_register_64b_unsigned(rs) | immediate);
}

void processor::i_type_0e(uint32_t instruction)	// XORI
{
	int immediate = instruction & MASK_16B;
	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	set_register_64b(rt, get_register_64b_unsigned(rs) ^ immediate);
}

void processor::i_type_0f(uint32_t instruction)	// LUI
{
	uint16_t immediate = instruction;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	set_register_32b_se(rt, immediate << 16);
}

void processor::i_type_14(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_14 not known");
}

void processor::i_type_16(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_16 not known");
}

void processor::i_type_17(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_17 not known");
}

void processor::i_type_18(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_18 not known");
}

void processor::i_type_19(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_19 not known");
}

void processor::i_type_1a(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_1a not known");
}

void processor::i_type_1b(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_1b not known");
}

void processor::i_type_1c(uint32_t instruction)	// SPECIAL2
{
	special2(instruction);
}

void processor::i_type_1d(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_1d not known");
}

void processor::i_type_1e(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_1e not known");
}

void processor::i_type_1f(uint32_t instruction)	// SPECIAL3
{
	special3(instruction);
}

void processor::i_type_20(uint32_t instruction)	// LB / LBU
{
	uint8_t base = (instruction >> 21) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset_s = int16_t(instruction & MASK_16B);

	uint64_t address = get_register_64b_unsigned(base) + offset_s;
	uint8_t temp_8b = -1;

	pmb -> read_8b(address, &temp_8b);

	uint8_t opcode = (instruction >> 26) & MASK_6B;

	if (opcode == 0x24)
		set_register_32b(rt, temp_8b);
	else
		set_register_32b(rt, int(temp_8b));

	cycles += 5;
}

void processor::i_type_21(uint32_t instruction)	// LH / LHU
{
	uint8_t base = (instruction >> 21) & MASK_5B;

	int offset_s = int16_t(instruction & MASK_16B);

	uint64_t address = get_register_64b_unsigned(base) + offset_s;
	if (unlikely(address & 1))
	{
		pdc -> log("i-type read 16b from %08llx: unaligned", address);

		cycles += 5; // FIXME

		throw PE_ADDRESS_ERROR;
	}
	else
	{
		uint16_t temp_16b = -1;

		pmb -> read_16b(address, &temp_16b);

		uint8_t opcode = (instruction >> 26) & MASK_6B;
		uint8_t rt = (instruction >> 16) & MASK_5B;

		if (opcode == 0x25)
			set_register_32b(rt, temp_16b);
		else
			set_register_32b(rt, int(temp_16b));

		cycles += 5;
		cycles += 5; // FIXME
	}
}

void processor::i_type_22(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_22 not known");
}

void processor::i_type_23(uint32_t instruction)	// LW / LL
{
	uint8_t base = (instruction >> 21) & MASK_5B;

	int offset_s = int16_t(instruction & MASK_16B);

	uint64_t address = get_register_64b_unsigned(base) + offset_s;
	if (unlikely(address & 3))
	{
		pdc -> log("i-type read 32b from %08llx: unaligned", address);

		cycles += 5; // FIXME

		throw PE_ADDRESS_ERROR;
	}
	else
	{
		uint32_t temp_32b = -1;

		pmb -> read_32b(address, &temp_32b);

		uint8_t rt = (instruction >> 16) & MASK_5B;

		set_register_32b(rt, temp_32b);

		cycles += 5;
	}
}

void processor::i_type_26(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_26 not known");
}

void processor::i_type_27(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_27 not known");
}

void processor::i_type_28(uint32_t instruction)	// SB
{
	uint8_t base = (instruction >> 21) & MASK_5B;

	int offset_s = int16_t(instruction & MASK_16B);

	uint64_t address = get_register_64b_unsigned(base) + offset_s;

	uint8_t rt = (instruction >> 16) & MASK_5B;
	int temp_32b = get_register_32b_unsigned(rt);

	cycles += 4;

	pmb -> write_8b(address, temp_32b);
}

void processor::i_type_29(uint32_t instruction)	// SH
{
	uint8_t base = (instruction >> 21) & MASK_5B;

	int offset_s = int16_t(instruction & MASK_16B);

	uint8_t rt = (instruction >> 16) & MASK_5B;

	uint64_t address = get_register_64b_unsigned(base) + offset_s;

	if (unlikely(address & 1))
	{
		pdc -> log("i-type write 16b %04x to %08x: unaligned", registers[rt] & 0xffff, address);

		cycles += 4; // FIXME

		throw PE_ADDRESS_ERROR;
	}
	else
	{
		int temp_32b = get_register_32b_unsigned(rt);
		pmb -> write_16b(address, temp_32b);

		cycles += 4;
	}
}

void processor::i_type_2a(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_2a not known");
}

void processor::i_type_2b(uint32_t instruction)	// SW
{
	uint8_t base = (instruction >> 21) & MASK_5B;

	int offset_s = int16_t(instruction & MASK_16B);

	uint8_t rt = (instruction >> 16) & MASK_5B;

	uint64_t address = get_register_64b_unsigned(base) + offset_s;

	if (unlikely(address & 3))
	{
		pdc -> log("i-type write 32b %08x to %08x: unaligned", registers[rt], address);

		cycles += 4; // FIXME

		throw PE_ADDRESS_ERROR;
	}
	else
	{
		pmb -> write_32b(address, get_register_32b_unsigned(rt));

		cycles += 4; // FIXME
	}
}

void processor::i_type_2c(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_2c not known");
}

void processor::i_type_2d(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_2d not known");
}

void processor::i_type_2e(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_2e not known");
}

void processor::i_type_2f(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_2f not known");
}

void processor::i_type_31(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_31 not known");
}

void processor::i_type_32(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_32 not known");
}

void processor::i_type_33(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_33 not known");
}

void processor::i_type_34(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_34 not known");
}

void processor::i_type_35(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_35 not known");
}

void processor::i_type_36(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_36 not known");
}

void processor::i_type_37(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_37 not known");
}

void processor::i_type_38(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_38 not known");
}

void processor::i_type_39(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_39 not known");
}

void processor::i_type_3a(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_3a not known");
}

void processor::i_type_3b(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = int16_t(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_3b not known");
}

void processor::i_type_3c(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = immediate_s << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_3c not known");
}

void processor::i_type_3d(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int b18_signed_offset = immediate_s << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_3d not known");
}

void processor::i_type_3e(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int b18_signed_offset = immediate_s << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_3e not known");
}

void processor::i_type_3f(uint32_t instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = int16_t(immediate);

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t base = rs;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	int b18_signed_offset = immediate_s << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_3f not known");
}

