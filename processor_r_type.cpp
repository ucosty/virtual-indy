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

void processor::r_type_00(int instruction)	// NOP / SLL
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;

	if (sa) // if sa==0 then NOP
		set_register_32b(rd, registers[rt] << sa);
}

void processor::r_type_01(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_01 not known");
}

void processor::r_type_02(int instruction)	// SRL / ROTR
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;

	if (IS_BIT_OFF0_SET(21, instruction))
		set_register_32b(rd, rotate_right(registers[rt] & MASK_32B, sa, 32));
	else
		set_register_32b(rd, (registers[rt] & MASK_32B) >> sa);
}

void processor::r_type_03(int instruction)	// SRA
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;

	set_register_32b(rd, sign_extend(registers[rt] >> sa, 32 - sa));
}

void processor::r_type_04(int instruction)	// SLLV
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;

	set_register_32b(rd, registers[rt] << sa);
}

void processor::r_type_05(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_05 not known");
}

void processor::r_type_06(int instruction)	// SRLV / ROTRV
{
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	if (IS_BIT_OFF0_SET(21, instruction))
		set_register_32b(rd, rotate_right(registers[rt] & MASK_32B, registers[rs] & MASK_5B, 32));
	else
		set_register_32b(rd, (registers[rt] & MASK_32B) >> (registers[rs] & MASK_5B));
}

void processor::r_type_07(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_07 not known");
}

void processor::r_type_08(int instruction)	// JR
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	tick();	// need to execute the next instruction - what if it is a JR as well? FIXME

	PC = registers[rs];
}

void processor::r_type_09(int instruction)	// JALR
{
	int rd = (instruction >> 11) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	tick();

	set_register_32b(rd, PC + 4);

	PC = registers[rs];
}

void processor::r_type_0a(int instruction)	// MOVZ
{
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	if (registers[rt] == 0)
		set_register_32b(rd, registers[rs]);
}

void processor::r_type_0b(int instruction)	// MOVN
{
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	if (registers[rt])
		set_register_32b(rd, registers[rs]);
}

void processor::r_type_0c(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_0c not known");
}

void processor::r_type_0d(int instruction)	// BREAK for debugging FIXME
{
	pdc -> log("BREAK");
}

void processor::r_type_0e(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_0e not known");
}

void processor::r_type_0f(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_0f not known");
}

void processor::r_type_10(int instruction)	// MFHI
{
	int rd = (instruction >> 11) & MASK_5B;

	set_register_32b(rd, HI);
}

void processor::r_type_11(int instruction)	// MTHI
{
	int rs = (instruction >> 21) & MASK_5B;

	HI = registers[rs];
}

void processor::r_type_12(int instruction)	// MFLO
{
	int rd = (instruction >> 11) & MASK_5B;

	set_register_32b(rd, LO);
}

void processor::r_type_13(int instruction)	// MTLO
{
	int rs = (instruction >> 21) & MASK_5B;

	LO = registers[rs];
}

void processor::r_type_14(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_14 not known");
}

void processor::r_type_15(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_15 not known");
}

void processor::r_type_16(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_16 not known");
}

void processor::r_type_17(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_17 not known");
}

void processor::r_type_18(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_18 not known");
}

void processor::r_type_19(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_19 not known");
}

void processor::r_type_1a(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_1a not known");
}

void processor::r_type_1b(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_1b not known");
}

void processor::r_type_1c(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_1c not known");
}

void processor::r_type_1d(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_1d not known");
}

void processor::r_type_1e(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_1e not known");
}

void processor::r_type_1f(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_1f not known");
}
void processor::r_type_20(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_20 not known");
}

void processor::r_type_21(int instruction)	// ADDU
{
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	set_register_32b(rd, (registers[rs] + registers[rt]) & MASK_32B);
}

void processor::r_type_22(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_22 not known");
}

void processor::r_type_23(int instruction)	// SUBU
{
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	set_register_32b(rd, (registers[rs] - registers[rt]) & MASK_32B);
}

void processor::r_type_24(int instruction)	// AND
{
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	set_register_32b(rd, registers[rs] & registers[rt]);
}

void processor::r_type_25(int instruction)	// OR
{
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	set_register_32b(rd, registers[rs] | registers[rt]);
}

void processor::r_type_26(int instruction)	// XOR
{
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	set_register_32b(rd, registers[rs] ^ registers[rt]);
}

void processor::r_type_27(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_27 not known");
}

void processor::r_type_28(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_28 not known");
}

void processor::r_type_29(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_29 not known");
}

void processor::r_type_2a(int instruction)	// SLT
{
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	if (untwos_complement(registers[rs], 32) < untwos_complement(registers[rt], 32))
		set_register_32b(rd, 1);
	else
		set_register_32b(rd, 0);
}

void processor::r_type_2b(int instruction)	// SLTU
{
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	if (registers[rs] < registers[rt])
		set_register_32b(rd, 1);
	else
		set_register_32b(rd, 0);
}

void processor::r_type_2c(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_2c not known");
}

void processor::r_type_2d(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_2d not known");
}

void processor::r_type_2e(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_2e not known");
}

void processor::r_type_2f(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_2f not known");
}

void processor::r_type_30(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_30 not known");
}

void processor::r_type_31(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_31 not known");
}

void processor::r_type_32(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_32 not known");
}

void processor::r_type_33(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_33 not known");
}

void processor::r_type_34(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_34 not known");
}

void processor::r_type_35(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_35 not known");
}

void processor::r_type_36(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_36 not known");
}

void processor::r_type_37(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_37 not known");
}

void processor::r_type_38(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_38 not known");
}

void processor::r_type_39(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_39 not known");
}

void processor::r_type_3a(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_3a not known");
}

void processor::r_type_3b(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_3b not known");
}

void processor::r_type_3c(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_3c not known");
}

void processor::r_type_3d(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_3d not known");
}

void processor::r_type_3e(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_3e not known");
}

void processor::r_type_3f(int instruction)
{
	int sa = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	pdc -> log("r_type_3f not known");
}

