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
	i_type_methods[20] = &processor::i_type_04;	// !
	i_type_methods[21] = &processor::i_type_05;	// !
	i_type_methods[22] = &processor::i_type_06;	// !
	i_type_methods[23] = &processor::i_type_07;	// !
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

void processor::i_type_11(uint32_t instruction)	// COP1
{
	COP1(instruction);
}

void processor::i_type_12(uint32_t instruction)	// COP2
{
	// COP2(instruction);
	pdc -> dc_log("i_type_12 (COP2) not implemented");
}

void processor::i_type_13(uint32_t instruction)	// COP1X
{
	// COP3(instruction);
	COP1X(instruction);
}

void processor::i_type_01(uint32_t instruction)	// BGEZAL
{
	regimm(instruction);
}

void processor::i_type_04(uint32_t instruction)	// BEQ/BEQL
{
	uint8_t rs = get_RS(instruction);
	uint8_t rt = get_RT(instruction);
	bool likely_op = IS_BIT_OFF0_SET(4, instruction); // BEQL

	conditional_jump(get_register_64b_unsigned(rs) == get_register_64b_unsigned(rt), instruction, likely_op);
}

void processor::i_type_05(uint32_t instruction)	// BNE/BNEL
{
	uint8_t rs = get_RS(instruction);
	uint8_t rt = get_RT(instruction);
	bool likely_op = IS_BIT_OFF0_SET(4, instruction); // BNEL

	conditional_jump(get_register_64b_unsigned(rs) != get_register_64b_unsigned(rt), instruction, likely_op);
}

void processor::i_type_06(uint32_t instruction)	// BLEZ/BLEZL
{
        uint8_t rs = get_RS(instruction);
	bool likely_op = IS_BIT_OFF0_SET(4, instruction); // BLEZL

	conditional_jump(get_register_64b_signed(rs) <= 0, instruction, likely_op);
}

void processor::i_type_07(uint32_t instruction)	// BGTZ
{
        uint8_t rs = get_RS(instruction);
	bool likely_op = IS_BIT_OFF0_SET(4, instruction); // BGTZL

	conditional_jump(get_register_64b_signed(rs) >= 0, instruction, likely_op);
}

void processor::i_type_08(uint32_t instruction)	// ADDI
{
	int16_t immediate_s = instruction;
	uint8_t rs = get_RS(instruction);

	int32_t val1 = get_register_32b_signed(rs);
	int32_t val2 = immediate_s;

	if (unlikely(test_tc_overflow_32b(val1, val2)))
	{
		pdc -> dc_log("ADDI overflow");
		throw processor_exception(PC, status_register, 0, PE_OVF, PC);
	}
	else
	{
		uint8_t rt = get_RT(instruction);

		set_register_32b_se(rt, val1 + val2);
	}
}

void processor::i_type_09(uint32_t instruction)	// ADDIU
{
	int immediate_s = int16_t(instruction);
	uint8_t rs = get_RS(instruction);
	uint8_t rt = get_RT(instruction);

	set_register_32b_se(rt, get_register_32b_signed(rs) + immediate_s);
}

void processor::i_type_0a(uint32_t instruction)	// SLTI
{
	int immediate_s = int16_t(instruction);
	uint8_t rs = get_RS(instruction);
	uint8_t rt = get_RT(instruction);

	if (get_register_64b_signed(rs) < immediate_s)
		set_register_64b(rt, 1);
	else
		set_register_64b(rt, 0);
}

void processor::i_type_0b(uint32_t instruction)	// SLTIU
{
	int immediate = int16_t(instruction);
	uint8_t rs = get_RS(instruction);
	uint8_t rt = get_RT(instruction);

	if (get_register_64b_unsigned(rs) < uint64_t(immediate))
		set_register_64b(rt, 1);
	else
		set_register_64b(rt, 0);
}

void processor::i_type_0c(uint32_t instruction)	// ANDI
{
	uint16_t immediate = instruction;
	uint8_t rs = get_RS(instruction);
	uint8_t rt = get_RT(instruction);

	set_register_64b(rt, get_register_64b_unsigned(rs) & immediate);
}

void processor::i_type_0d(uint32_t instruction)	// ORI
{
	uint16_t immediate = instruction;
	uint8_t rs = get_RS(instruction);
	uint8_t rt = get_RT(instruction);

	set_register_64b(rt, get_register_64b_unsigned(rs) | immediate);
}

void processor::i_type_0e(uint32_t instruction)	// XORI
{
	uint16_t immediate = instruction;
	uint8_t rs = get_RS(instruction);
	uint8_t rt = get_RT(instruction);

	set_register_64b(rt, get_register_64b_unsigned(rs) ^ immediate);
}

void processor::i_type_0f(uint32_t instruction)	// LUI
{
	uint16_t immediate = instruction;
	uint8_t rt = get_RT(instruction);

	set_register_32b_se(rt, immediate << 16);
}

void processor::i_type_18(uint32_t instruction)
{
	pdc -> dc_log("i_type_18 not implemented");
}

void processor::i_type_19(uint32_t instruction)
{
	pdc -> dc_log("i_type_19 not implemented");
}

void processor::i_type_1a(uint32_t instruction)
{
	pdc -> dc_log("i_type_1a not implemented");
}

void processor::i_type_1b(uint32_t instruction)
{
	pdc -> dc_log("i_type_1b not implemented");
}

void processor::i_type_1c(uint32_t instruction)	// SPECIAL2
{
	special2(instruction);
}

void processor::i_type_1d(uint32_t instruction)
{
	pdc -> dc_log("i_type_1d not implemented");
}

void processor::i_type_1e(uint32_t instruction)
{
	pdc -> dc_log("i_type_1e not implemented");
}

void processor::i_type_1f(uint32_t instruction)	// SPECIAL3
{
	special3(instruction);
}

void processor::i_type_20(uint32_t instruction)	// LB / LBU
{
	uint64_t address = get_base_register_with_offset(instruction);
	uint8_t temp_8b = -1;
	pmb -> read_8b(address, &temp_8b);

	uint8_t rt = get_RT(instruction);

	if (get_opcode(instruction) == 0x24)	// LBU
		set_register_64b(rt, temp_8b);
	else
		set_register_64b(rt, int8_t(temp_8b));

	cycles += 5;
}

void processor::i_type_21(uint32_t instruction)	// LH / LHU
{
	uint64_t address = get_base_register_with_offset(instruction);

	if (unlikely(address & 1))
	{
		pdc -> dc_log("i-type read 16b from %016llx: unaligned", address);

		cycles += 5; // FIXME

		throw processor_exception(address, status_register, 0, PE_ADDRL, PC);
	}
	else
	{
		uint16_t temp_16b = -1;
		pmb -> read_16b(address, &temp_16b);

		uint8_t rt = get_RT(instruction);

		if (get_opcode(instruction) == 0x25) // LHU
			set_register_64b(rt, temp_16b);
		else
			set_register_64b(rt, int16_t(temp_16b));

		cycles += 5;
		cycles += 5; // FIXME
	}
}

void processor::i_type_22(uint32_t instruction)	// LWL
{
	pdc -> dc_log("i_type_22 not implemented");
}

void processor::i_type_23(uint32_t instruction)	// LW / LL
{
	uint64_t address = get_base_register_with_offset(instruction);

	if (unlikely(address & 3))
	{
		pdc -> dc_log("i-type read 32b from %016llx: unaligned", address);

		cycles += 5; // FIXME

		throw processor_exception(address, status_register, 0, PE_ADDRL, PC);
	}
	else
	{
		uint32_t temp_32b = -1;
		pmb -> read_32b(address, &temp_32b);

		set_register_64b(get_RT(instruction), int32_t(temp_32b));

		if (get_opcode(instruction) == 0x30) // LL
			start_RMW_sequence();

		cycles += 5;
	}
}

void processor::i_type_26(uint32_t instruction)
{
	pdc -> dc_log("i_type_26 not implemented");
}

void processor::i_type_27(uint32_t instruction)
{
	pdc -> dc_log("i_type_27 not implemented");
}

void processor::i_type_28(uint32_t instruction)	// SB
{
	uint64_t address = get_base_register_with_offset(instruction);

	uint8_t rt = get_RT(instruction);

	int temp_32b = get_register_32b_unsigned(rt);

	cycles += 4;

	pmb -> write_8b(address, temp_32b);
}

void processor::i_type_29(uint32_t instruction)	// SH
{
	uint64_t address = get_base_register_with_offset(instruction);

	uint8_t rt = get_RT(instruction);

	if (unlikely(address & 1))
	{
		pdc -> dc_log("i-type write 16b %04x to %08x: unaligned", registers[rt] & 0xffff, address);

		cycles += 4; // FIXME

		throw processor_exception(address, status_register, 0, PE_ADDRS, PC);
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
	pdc -> dc_log("i_type_2a not implemented");
}

void processor::i_type_2b(uint32_t instruction)	// SW
{
	uint64_t address = get_base_register_with_offset(instruction);

	uint8_t rt = get_RT(instruction);

	if (unlikely(address & 3))
	{
		pdc -> dc_log("i-type write 32b %08x to %08x: unaligned", registers[rt], address);

		cycles += 4; // FIXME

		throw processor_exception(address, status_register, 0, PE_ADDRS, PC);
	}
	else
	{
		pmb -> write_32b(address, get_register_32b_unsigned(rt));

		cycles += 4; // FIXME
	}
}

void processor::i_type_2c(uint32_t instruction)
{
	pdc -> dc_log("i_type_2c not implemented");
}

void processor::i_type_2d(uint32_t instruction)
{
	pdc -> dc_log("i_type_2d not implemented");
}

void processor::i_type_2e(uint32_t instruction)
{
	pdc -> dc_log("i_type_2e not implemented");
}

void processor::i_type_2f(uint32_t instruction)
{
	pdc -> dc_log("i_type_2f not implemented");
}

void processor::i_type_31(uint32_t instruction)	// LWC1
{
	uint64_t address = get_base_register_with_offset(instruction);

	uint8_t rt = get_RT(instruction);

	uint32_t dummy = -1;
	pmb -> read_32b(address, &dummy);
	C1_registers[rt] = dummy;
}

void processor::i_type_32(uint32_t instruction)	// LWC2
{
	uint64_t address = get_base_register_with_offset(instruction);

	uint8_t rt = get_RT(instruction);

	uint32_t dummy = -1;
	pmb -> read_32b(address, &dummy);
	C2_registers[rt] = dummy;
}

void processor::i_type_33(uint32_t instruction)
{
	pdc -> dc_log("i_type_33 not implemented");
}

void processor::i_type_34(uint32_t instruction)
{
	pdc -> dc_log("i_type_34 not implemented");
}

void processor::i_type_35(uint32_t instruction)	// LDC1
{
	uint64_t address = get_base_register_with_offset(instruction);

	uint8_t rt = get_RT(instruction);

	pmb -> read_64b(address, &C1_registers[rt]);
}

void processor::i_type_36(uint32_t instruction)	// LDC2
{
	uint64_t address = get_base_register_with_offset(instruction);

	uint8_t rt = get_RT(instruction);

	pmb -> read_64b(address, &C2_registers[rt]);
}

void processor::i_type_37(uint32_t instruction)
{
	pdc -> dc_log("i_type_37 not implemented");
}

void processor::i_type_38(uint32_t instruction)	// SC
{
	uint64_t address = get_base_register_with_offset(instruction);
	uint8_t rt = get_RT(instruction);

	if (!RMW_sequence)
	{
		set_register_64b(rt, 0);
	}
	else if (unlikely(address & 3))
	{
		pdc -> dc_log("i-type write 32b to %016llx: unaligned", address);

		set_register_64b(rt, 0);
		cycles += 5; // FIXME

		throw processor_exception(address, status_register, 0, PE_ADDRS, PC);
	}
	else
	{
		uint32_t value = get_register_32b_unsigned(rt);

		pmb -> write_32b(address, value);

		// AFTER the memory write as a write may cause an
		// exception
		set_register_64b(rt, 1);
	}
}

void processor::i_type_39(uint32_t instruction)	// SWC1
{
	uint64_t address = get_base_register_with_offset(instruction);

	uint8_t rt = get_RT(instruction);

	pmb -> write_32b(address, C1_registers[rt]);
}

void processor::i_type_3a(uint32_t instruction)	// SWC2
{
	uint64_t address = get_base_register_with_offset(instruction);

	uint8_t rt = get_RT(instruction);

	pmb -> write_32b(address, C2_registers[rt]);
}

void processor::i_type_3b(uint32_t instruction)
{
	pdc -> dc_log("i_type_3b not implemented");
}

void processor::i_type_3c(uint32_t instruction)
{
	pdc -> dc_log("i_type_3c not implemented");
}

void processor::i_type_3d(uint32_t instruction)	// SDC1
{
	uint64_t address = get_base_register_with_offset(instruction);

	uint8_t rt = get_RT(instruction);

	pmb -> write_64b(address, C1_registers[rt]);
}

void processor::i_type_3e(uint32_t instruction)	// SDC2
{
	uint64_t address = get_base_register_with_offset(instruction);

	uint8_t rt = get_RT(instruction);

	pmb -> write_64b(address, C2_registers[rt]);
}

void processor::i_type_3f(uint32_t instruction)
{
	pdc -> dc_log("i_type_3f not implemented");
}
