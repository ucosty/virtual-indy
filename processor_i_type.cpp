#include "processor.h"
#include "processor_utils.h"

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

void processor::i_type_00(int instruction) // R-type / SPECIAL
{
	int function = instruction & MASK_6B;

	(((processor*)this)->*processor::r_type_methods[function])(instruction);

	cycles += 4;
}

void processor::i_type_02(int instruction)
{
	j_type(2, instruction);

	cycles += 3;
}

void processor::i_type_03(int instruction)
{
	j_type(3, instruction);

	cycles += 3;
}

void processor::i_type_10(int instruction)
{
	COP0(instruction);
}

void processor::i_type_11(int instruction)
{
	// COP1(instruction);
	pdc -> log("i_type_11 not known");
}

void processor::i_type_12(int instruction)
{
	// COP2(instruction);
	pdc -> log("i_type_12 not known");
}

void processor::i_type_13(int instruction)
{
	// COP3(instruction);
	pdc -> log("i_type_13 not known");
}

void processor::i_type_01(int instruction)	// BGEZAL
{
	regimm(instruction);
}

void processor::i_type_04(int instruction)	// BEQ
{
	int rs = (instruction >> 21) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int offset = instruction & MASK_16B;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	if (registers[rs] == registers[rt])
	{
		int new_PC = PC + b18_signed_offset;
		tick();
		PC = new_PC;
	}
}

void processor::i_type_05(int instruction)	// BNE/BNEL
{
	int rs = (instruction >> 21) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int b18_signed_offset = untwos_complement_16b(instruction & MASK_16B) << 2;

	if (registers[rs] != registers[rt])
	{
		int new_PC = PC + b18_signed_offset;
		tick();
		PC = new_PC;
	}
}

void processor::i_type_06(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_06 not known");
}

void processor::i_type_07(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_07 not known");
}

void processor::i_type_08(int instruction)	// ADDI
{
	int immediate_s = untwos_complement_16b(instruction & MASK_16B);
	int rs = (instruction >> 21) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;

	// FIXME 2's complement overflow check
	set_register_32b_se(rt, get_register_32b_signed(rs) + immediate_s);
}

void processor::i_type_09(int instruction)	// ADDIU
{
	int immediate_s = untwos_complement_16b(instruction & MASK_16B);
	int rs = (instruction >> 21) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;

	set_register_32b_se(rt, get_register_32b(rs) + immediate_s);
}

void processor::i_type_0a(int instruction)	// SLTI
{
	int immediate_s = untwos_complement_16b(instruction & MASK_16B);
	int rs = (instruction >> 21) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;

	if (get_register_32b_signed(rs) < immediate_s)
		set_register_32b(rt, 1);
	else
		set_register_32b(rt, 0);
}

void processor::i_type_0b(int instruction)	// SLTIU
{
	int immediate = instruction & MASK_16B;
	int rs = (instruction >> 21) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;

	if (get_register_32b(rs) < (unsigned int)sign_extend_16b(immediate))
		set_register_32b(rt, 1);
	else
		set_register_32b(rt, 0);
}

void processor::i_type_0c(int instruction)	// ANDI
{
	int immediate = instruction & MASK_16B;
	int rs = (instruction >> 21) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;

	set_register_32b(rt, get_register_32b(rs) & immediate);
}

void processor::i_type_0d(int instruction)	// ORI
{
	int immediate = instruction & MASK_16B;
	int rs = (instruction >> 21) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;

	set_register_32b(rt, get_register_32b(rs) | immediate);
}

void processor::i_type_0e(int instruction)	// XORI
{
	int immediate = instruction & MASK_16B;
	int rs = (instruction >> 21) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;

	set_register_32b(rt, get_register_32b(rs) ^ immediate);
}

void processor::i_type_0f(int instruction)	// LUI
{
	int immediate = instruction & MASK_16B;
	int rt = (instruction >> 16) & MASK_5B;

	set_register_32b(rt, immediate << 16);
}

void processor::i_type_14(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_14 not known");
}

void processor::i_type_16(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_16 not known");
}

void processor::i_type_17(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_17 not known");
}

void processor::i_type_18(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_18 not known");
}

void processor::i_type_19(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_19 not known");
}

void processor::i_type_1a(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_1a not known");
}

void processor::i_type_1b(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_1b not known");
}

void processor::i_type_1c(int instruction)	// SPECIAL2
{
	special2(instruction);
}

void processor::i_type_1d(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_1d not known");
}

void processor::i_type_1e(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_1e not known");
}

void processor::i_type_1f(int instruction)	// SPECIAL3
{
	special3(instruction);
}

void processor::i_type_20(int instruction)	// LB / LBU
{
	int base = (instruction >> 21) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;

	int offset_s = untwos_complement_16b(instruction & MASK_16B);

	int address = registers[base] + offset_s, temp_32b = -1;

	if (!pmb -> read_8b(address, &temp_32b))
		pdc -> log("i-type read 8b from %08x failed", address);

	int opcode = (instruction >> 26) & MASK_6B;

	if (opcode == 0x24)
		set_register_32b(rt, temp_32b);
	else
		set_register_32b(rt, sign_extend_8b(temp_32b));
}

void processor::i_type_21(int instruction)	// LH / LHU
{
	int base = (instruction >> 21) & MASK_5B;

	int offset_s = untwos_complement_16b(instruction & MASK_16B);

	int address = registers[base] + offset_s;
	if (address & 1)
	{
		pdc -> log("i-type read 16b from %08x: unaligned", address);
		// FIXME throw address error exception
	}
	else
	{
		int temp_32b = -1;

		if (!pmb -> read_16b(address, &temp_32b))
			pdc -> log("i-type read 16b from %08x failed", address);

		int opcode = (instruction >> 26) & MASK_6B;
		int rt = (instruction >> 16) & MASK_5B;

		if (opcode == 0x25)
			set_register_32b(rt, temp_32b);
		else
			set_register_32b(rt, sign_extend_16b(temp_32b));
	}
}

void processor::i_type_22(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_22 not known");
}

void processor::i_type_23(int instruction)	// LW / LL
{
	int base = (instruction >> 21) & MASK_5B;

	int offset_s = untwos_complement_16b(instruction & MASK_16B);

	int address = registers[base] + offset_s;
	if (address & 3)
	{
		pdc -> log("i-type read 32b from %08x: unaligned", address);
		// FIXME throw address error exception
	}
	else
	{
		int temp_32b = -1;

		if (!pmb -> read_32b(address, &temp_32b))
			pdc -> log("i-type read 32b from %08x failed", address);

		int rt = (instruction >> 16) & MASK_5B;

		set_register_32b(rt, temp_32b);
	}
}

void processor::i_type_26(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_26 not known");
}

void processor::i_type_27(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_27 not known");
}

void processor::i_type_28(int instruction)	// SB
{
	int base = (instruction >> 21) & MASK_5B;

	int offset_s = untwos_complement_16b(instruction & MASK_16B);

	int address = registers[base] + offset_s;

	int rt = (instruction >> 16) & MASK_5B;
	int temp_32b = registers[rt];

	if (!pmb -> write_8b(address, temp_32b))
		pdc -> log("i-type write 8b %02x to %08x failed", registers[rt] & 0xff, address);
}

void processor::i_type_29(int instruction)	// SH
{
	int base = (instruction >> 21) & MASK_5B;

	int offset_s = untwos_complement_16b(instruction & MASK_16B);

	int rt = (instruction >> 16) & MASK_5B;

	int address = registers[base] + offset_s;

	if (address & 1)
	{
		pdc -> log("i-type write 16b %04x to %08x: unaligned", registers[rt] & 0xffff, address);
		// FIXME throw address error exception
	}
	else
	{
		int temp_32b = registers[rt];
		if (!pmb -> write_16b(address, temp_32b))
			pdc -> log("i-type write 16b %04x to %08x failed", registers[rt] & 0xffff, address);
	}

}

void processor::i_type_2a(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_2a not known");
}

void processor::i_type_2b(int instruction)	// SW
{
	int base = (instruction >> 21) & MASK_5B;

	int offset_s = untwos_complement_16b(instruction & MASK_16B);

	int rt = (instruction >> 16) & MASK_5B;

	int address = registers[base] + offset_s;

	if (address & 3)
	{
		pdc -> log("i-type write 32b %08x to %08x: unaligned", registers[rt], address);
		// FIXME throw address error exception
	}
	else
	{
		if (!pmb -> write_32b(address, registers[rt]))
			pdc -> log("i-type write 32b %08x to %08x failed", registers[rt], address);
	}
}

void processor::i_type_2c(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_2c not known");
}

void processor::i_type_2d(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_2d not known");
}

void processor::i_type_2e(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_2e not known");
}

void processor::i_type_2f(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_2f not known");
}

void processor::i_type_31(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_31 not known");
}

void processor::i_type_32(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_32 not known");
}

void processor::i_type_33(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_33 not known");
}

void processor::i_type_34(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_34 not known");
}

void processor::i_type_35(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_35 not known");
}

void processor::i_type_36(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_36 not known");
}

void processor::i_type_37(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_37 not known");
}

void processor::i_type_38(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_38 not known");
}

void processor::i_type_39(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_39 not known");
}

void processor::i_type_3a(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_3a not known");
}

void processor::i_type_3b(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_3b not known");
}

void processor::i_type_3c(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_3c not known");
}

void processor::i_type_3d(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_3d not known");
}

void processor::i_type_3e(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_3e not known");
}

void processor::i_type_3f(int instruction)
{
	int immediate = instruction & MASK_16B;
	int immediate_s = untwos_complement_16b(immediate);

	int rs = (instruction >> 21) & MASK_5B;
	int base = rs;
	int rt = (instruction >> 16) & MASK_5B;

	int offset = immediate;
	int offset_s = immediate_s;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	int temp_32b = -1, address = -1;

	pdc -> log("i_type_3f not known");
}

