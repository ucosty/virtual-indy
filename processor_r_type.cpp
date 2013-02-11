#include "processor.h"
#include "processor_utils.h"

void processor::init_r_type()
{
	r_type_methods[0] = &processor::r_type_00;
	r_type_methods[1] = &processor::r_type_01;
	r_type_methods[2] = &processor::r_type_02;
	r_type_methods[3] = &processor::r_type_03;
	r_type_methods[4] = &processor::r_type_04;
	r_type_methods[5] = &processor::r_type_05;
	r_type_methods[6] = &processor::r_type_06;
	r_type_methods[7] = &processor::r_type_07;
	r_type_methods[8] = &processor::r_type_08;
	r_type_methods[9] = &processor::r_type_09;
	r_type_methods[10] = &processor::r_type_0a;
	r_type_methods[11] = &processor::r_type_0b;
	r_type_methods[12] = &processor::r_type_0c;
	r_type_methods[13] = &processor::r_type_0d;
	r_type_methods[14] = &processor::r_type_0e;
	r_type_methods[15] = &processor::r_type_0f;
	r_type_methods[16] = &processor::r_type_10;
	r_type_methods[17] = &processor::r_type_11;
	r_type_methods[18] = &processor::r_type_12;
	r_type_methods[19] = &processor::r_type_13;
	r_type_methods[20] = &processor::r_type_14;
	r_type_methods[21] = &processor::r_type_15;
	r_type_methods[22] = &processor::r_type_16;
	r_type_methods[23] = &processor::r_type_17;
	r_type_methods[24] = &processor::r_type_18;
	r_type_methods[25] = &processor::r_type_19;
	r_type_methods[26] = &processor::r_type_1a;
	r_type_methods[27] = &processor::r_type_1b;
	r_type_methods[28] = &processor::r_type_1c;
	r_type_methods[29] = &processor::r_type_1d;
	r_type_methods[30] = &processor::r_type_1e;
	r_type_methods[31] = &processor::r_type_1f;
	r_type_methods[32] = &processor::r_type_20;
	r_type_methods[33] = &processor::r_type_21;
	r_type_methods[34] = &processor::r_type_22;
	r_type_methods[35] = &processor::r_type_23;
	r_type_methods[36] = &processor::r_type_24;
	r_type_methods[37] = &processor::r_type_25;
	r_type_methods[38] = &processor::r_type_26;
	r_type_methods[39] = &processor::r_type_27;
	r_type_methods[40] = &processor::r_type_28;
	r_type_methods[41] = &processor::r_type_29;
	r_type_methods[42] = &processor::r_type_2a;
	r_type_methods[43] = &processor::r_type_2b;
	r_type_methods[44] = &processor::r_type_2c;
	r_type_methods[45] = &processor::r_type_2d;
	r_type_methods[46] = &processor::r_type_2e;
	r_type_methods[47] = &processor::r_type_2f;
	r_type_methods[48] = &processor::r_type_30;
	r_type_methods[49] = &processor::r_type_31;
	r_type_methods[50] = &processor::r_type_32;
	r_type_methods[51] = &processor::r_type_33;
	r_type_methods[52] = &processor::r_type_34;
	r_type_methods[53] = &processor::r_type_35;
	r_type_methods[54] = &processor::r_type_36;
	r_type_methods[55] = &processor::r_type_37;
	r_type_methods[56] = &processor::r_type_38;
	r_type_methods[57] = &processor::r_type_39;
	r_type_methods[58] = &processor::r_type_3a;
	r_type_methods[59] = &processor::r_type_3b;
	r_type_methods[60] = &processor::r_type_3c;
	r_type_methods[61] = &processor::r_type_3d;
	r_type_methods[62] = &processor::r_type_3e;
	r_type_methods[63] = &processor::r_type_3f;
}

void processor::r_type_00(uint32_t instruction)	// NOP / SLL
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	if (sa) // if sa==0 then NOP
		set_register_32b(rd, get_register_32b_unsigned(rt) << sa);
}

void processor::r_type_01(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_01 not known");
}

void processor::r_type_02(uint32_t instruction)	// SRL / ROTR
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	if (IS_BIT_OFF0_SET(21, instruction))
		set_register_32b(rd, rotate_right(get_register_32b_unsigned(rt), sa, 32));
	else
		set_register_32b(rd, get_register_32b_unsigned(rt) >> sa);
}

void processor::r_type_03(uint32_t instruction)	// SRA
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	set_register_32b(rd, sign_extend(get_register_32b_unsigned(rt) >> sa, 32 - sa));
}

void processor::r_type_04(uint32_t instruction)	// SLLV
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;

	set_register_32b(rd, get_register_32b_unsigned(rt) << sa);
}

void processor::r_type_05(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_05 not known");
}

void processor::r_type_06(uint32_t instruction)	// SRLV / ROTRV
{
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	if (IS_BIT_OFF0_SET(21, instruction))
		set_register_32b(rd, rotate_right(get_register_32b_unsigned(rt), get_register_32b_unsigned(rs) & MASK_5B, 32));
	else
		set_register_32b(rd, get_register_32b_unsigned(rt) >> (get_register_32b_unsigned(rs) & MASK_5B));
}

void processor::r_type_07(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_07 not known");
}

void processor::r_type_08(uint32_t instruction)	// JR
{
	uint8_t hint = (instruction >> 6) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	if (hint)
		pdc -> log("r_type_08: hint is unexpected value (%d, expected 0)", hint);

	set_delay_slot(PC);

	PC = get_register_64b_unsigned(rs);
}

void processor::r_type_09(uint32_t instruction)	// JALR
{
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	set_delay_slot(PC);

	set_register_64b(rd, PC + 4);

	PC = get_register_64b_unsigned(rs);
}

void processor::r_type_0a(uint32_t instruction)	// MOVZ
{
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	if (get_register_32b_unsigned(rt) == 0)
		set_register_32b(rd, get_register_32b_unsigned(rs));
}

void processor::r_type_0b(uint32_t instruction)	// MOVN
{
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	if (get_register_32b_unsigned(rt))
		set_register_32b(rd, get_register_32b_unsigned(rs));
}

void processor::r_type_0c(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_0c not known");
}

void processor::r_type_0d(uint32_t instruction)	// BREAK for debugging FIXME
{
	pdc -> log("BREAK");
}

void processor::r_type_0e(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_0e not known");
}

void processor::r_type_0f(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_0f not known");
}

void processor::r_type_10(uint32_t instruction)	// MFHI
{
	uint8_t rd = (instruction >> 11) & MASK_5B;

	set_register_32b(rd, HI);
}

void processor::r_type_11(uint32_t instruction)	// MTHI
{
	uint8_t rs = (instruction >> 21) & MASK_5B;

	HI = get_register_64b_unsigned(rs);
}

void processor::r_type_12(uint32_t instruction)	// MFLO
{
	uint8_t rd = (instruction >> 11) & MASK_5B;

	set_register_32b(rd, LO);
}

void processor::r_type_13(uint32_t instruction)	// MTLO
{
	uint8_t rs = (instruction >> 21) & MASK_5B;

	LO = get_register_64b_unsigned(rs);
}

void processor::r_type_14(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_14 not known");
}

void processor::r_type_15(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_15 not known");
}

void processor::r_type_16(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_16 not known");
}

void processor::r_type_17(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_17 not known");
}

void processor::r_type_18(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_18 not known");
}

void processor::r_type_19(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_19 not known");
}

void processor::r_type_1a(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_1a not known");
}

void processor::r_type_1b(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_1b not known");
}

void processor::r_type_1c(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_1c not known");
}

void processor::r_type_1d(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_1d not known");
}

void processor::r_type_1e(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_1e not known");
}

void processor::r_type_1f(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_1f not known");
}
void processor::r_type_20(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_20 not known");
}

void processor::r_type_21(uint32_t instruction)	// ADDU
{
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	set_register_32b(rd, uint32_t(get_register_32b_unsigned(rs) + get_register_32b_unsigned(rt)));
}

void processor::r_type_22(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_22 not known");
}

void processor::r_type_23(uint32_t instruction)	// SUBU
{
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	set_register_32b(rd, uint32_t(get_register_32b_unsigned(rs) - get_register_32b_unsigned(rt)));
}

void processor::r_type_24(uint32_t instruction)	// AND
{
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	set_register_32b(rd, get_register_32b_unsigned(rs) & get_register_32b_unsigned(rt));
}

void processor::r_type_25(uint32_t instruction)	// OR
{
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	set_register_32b(rd, get_register_32b_unsigned(rs) | get_register_32b_unsigned(rt));
}

void processor::r_type_26(uint32_t instruction)	// XOR
{
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	set_register_32b(rd, get_register_32b_unsigned(rs) ^ get_register_32b_unsigned(rt));
}

void processor::r_type_27(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_27 not known");
}

void processor::r_type_28(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_28 not known");
}

void processor::r_type_29(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_29 not known");
}

void processor::r_type_2a(uint32_t instruction)	// SLT
{
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	if (get_register_32b_signed(rs) < get_register_32b_signed(rt))
		set_register_32b(rd, 1);
	else
		set_register_32b(rd, 0);
}

void processor::r_type_2b(uint32_t instruction)	// SLTU
{
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	if (get_register_32b_unsigned(rs) < get_register_32b_unsigned(rt))
		set_register_32b(rd, 1);
	else
		set_register_32b(rd, 0);
}

void processor::r_type_2c(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_2c not known");
}

void processor::r_type_2d(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_2d not known");
}

void processor::r_type_2e(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_2e not known");
}

void processor::r_type_2f(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_2f not known");
}

void processor::r_type_30(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_30 not known");
}

void processor::r_type_31(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_31 not known");
}

void processor::r_type_32(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_32 not known");
}

void processor::r_type_33(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_33 not known");
}

void processor::r_type_34(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_34 not known");
}

void processor::r_type_35(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_35 not known");
}

void processor::r_type_36(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_36 not known");
}

void processor::r_type_37(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_37 not known");
}

void processor::r_type_38(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_38 not known");
}

void processor::r_type_39(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_39 not known");
}

void processor::r_type_3a(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_3a not known");
}

void processor::r_type_3b(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_3b not known");
}

void processor::r_type_3c(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_3c not known");
}

void processor::r_type_3d(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_3d not known");
}

void processor::r_type_3e(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_3e not known");
}

void processor::r_type_3f(uint32_t instruction)
{
	uint8_t sa = (instruction >> 6) & MASK_5B;
	uint8_t rd = (instruction >> 11) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_3f not known");
}

