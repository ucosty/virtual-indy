#ifndef __PROCESSOR__H__
#define __PROCESSOR__H__

#include <string>

#include "debug_console.h"
#include "memory_bus.h"

#define SR_EI 0			// status register "EI" bit
#define SR_KERNEL_USER	1	// kernel/user mode

class processor
{
private:
	debug_console *pdc;
	memory_bus *pmb;

	// 0		zero
	// 1		$at		(assembler temporary) reserved by the assembler
	// 2-3		$v0 - $v1	(values) from expression evaluation and function results
	// 4-7		$a0 - $a3	(arguments) First four parameters for subroutine.  Not preserved across procedure calls
	// 8-15		$t0 - $t7	(temporaries) Caller saved if needed. Subroutines can use w/out saving.  Not preserved across procedure calls
	// 16-23	$s0 - $s7	(saved values) - Callee saved.  A subroutine using one of these must save original and restore it before exiting.  Preserved across procedure calls
	// 24-25	$t8 - $t9	(temporaries) Caller saved if needed. Subroutines can use w/out saving.  These are in addition to $t0 - $t7 above.  Not preserved across procedure calls.
	// 26-27	$k0 - $k1	reserved for use by the interrupt/trap handler
	// 28		$gp		global pointer.  Points to the middle of the 64K block of memory in the static data segment.
	// 29		$sp		stack pointer Points to last location on the stack.
	// 30		$s8/$fp		saved value / frame pointer Preserved across procedure calls
	// 31		$ra		return address
	int registers[32], PC, HI, LO, status_register;
	int C0_registers[32];

	long long int cycles;

	void j_type(int opcode, int instruction);
	void special2(int instruction);
	void special3(int instruction);
	void COP0(int instruction);
	// not in a group? FIXME
	void SLTI(int instruction);
	void regimm(int instruction);
	void interrupt(int nr);

	void init_r_type();
	void r_type_00(int instruction);
	void r_type_01(int instruction);
	void r_type_02(int instruction);
	void r_type_03(int instruction);
	void r_type_04(int instruction);
	void r_type_05(int instruction);
	void r_type_06(int instruction);
	void r_type_07(int instruction);
	void r_type_08(int instruction);
	void r_type_09(int instruction);
	void r_type_0a(int instruction);
	void r_type_0b(int instruction);
	void r_type_0c(int instruction);
	void r_type_0d(int instruction);
	void r_type_0e(int instruction);
	void r_type_0f(int instruction);
	void r_type_10(int instruction);
	void r_type_11(int instruction);
	void r_type_12(int instruction);
	void r_type_13(int instruction);
	void r_type_14(int instruction);
	void r_type_15(int instruction);
	void r_type_16(int instruction);
	void r_type_17(int instruction);
	void r_type_18(int instruction);
	void r_type_19(int instruction);
	void r_type_1a(int instruction);
	void r_type_1b(int instruction);
	void r_type_1c(int instruction);
	void r_type_1d(int instruction);
	void r_type_1e(int instruction);
	void r_type_1f(int instruction);
	void r_type_20(int instruction);
	void r_type_21(int instruction);
	void r_type_22(int instruction);
	void r_type_23(int instruction);
	void r_type_24(int instruction);
	void r_type_25(int instruction);
	void r_type_26(int instruction);
	void r_type_27(int instruction);
	void r_type_28(int instruction);
	void r_type_29(int instruction);
	void r_type_2a(int instruction);
	void r_type_2b(int instruction);
	void r_type_2c(int instruction);
	void r_type_2d(int instruction);
	void r_type_2e(int instruction);
	void r_type_2f(int instruction);
	void r_type_30(int instruction);
	void r_type_31(int instruction);
	void r_type_32(int instruction);
	void r_type_33(int instruction);
	void r_type_34(int instruction);
	void r_type_35(int instruction);
	void r_type_36(int instruction);
	void r_type_37(int instruction);
	void r_type_38(int instruction);
	void r_type_39(int instruction);
	void r_type_3a(int instruction);
	void r_type_3b(int instruction);
	void r_type_3c(int instruction);
	void r_type_3d(int instruction);
	void r_type_3e(int instruction);
	void r_type_3f(int instruction);
	void (processor::*r_type_methods[64])(int);

	void init_i_type();
	void i_type_00(int instruction);
	void i_type_01(int instruction);
	void i_type_02(int instruction);
	void i_type_03(int instruction);
	void i_type_04(int instruction);
	void i_type_05(int instruction);
	void i_type_06(int instruction);
	void i_type_07(int instruction);
	void i_type_08(int instruction);
	void i_type_09(int instruction);
	void i_type_0a(int instruction);
	void i_type_0b(int instruction);
	void i_type_0c(int instruction);
	void i_type_0d(int instruction);
	void i_type_0e(int instruction);
	void i_type_0f(int instruction);
	void i_type_10(int instruction);
	void i_type_11(int instruction);
	void i_type_12(int instruction);
	void i_type_13(int instruction);
	void i_type_14(int instruction);
	void i_type_16(int instruction);
	void i_type_17(int instruction);
	void i_type_18(int instruction);
	void i_type_19(int instruction);
	void i_type_1a(int instruction);
	void i_type_1b(int instruction);
	void i_type_1c(int instruction);
	void i_type_1d(int instruction);
	void i_type_1e(int instruction);
	void i_type_1f(int instruction);
	void i_type_20(int instruction);
	void i_type_21(int instruction);
	void i_type_22(int instruction);
	void i_type_23(int instruction);
	void i_type_26(int instruction);
	void i_type_27(int instruction);
	void i_type_28(int instruction);
	void i_type_29(int instruction);
	void i_type_2a(int instruction);
	void i_type_2b(int instruction);
	void i_type_2c(int instruction);
	void i_type_2d(int instruction);
	void i_type_2e(int instruction);
	void i_type_2f(int instruction);
	void i_type_31(int instruction);
	void i_type_32(int instruction);
	void i_type_33(int instruction);
	void i_type_34(int instruction);
	void i_type_35(int instruction);
	void i_type_36(int instruction);
	void i_type_37(int instruction);
	void i_type_38(int instruction);
	void i_type_39(int instruction);
	void i_type_3a(int instruction);
	void i_type_3b(int instruction);
	void i_type_3c(int instruction);
	void i_type_3d(int instruction);
	void i_type_3e(int instruction);
	void i_type_3f(int instruction);
	void (processor::*i_type_methods[64])(int);

public:
	processor(debug_console *pdc_in, memory_bus *pmb_in);
	~processor();

	int get_register(int nr) const;
	int get_PC() const { return PC; }
	int get_HI() const { return HI; }
	int get_LO() const { return LO; }
	int get_SR() const { return status_register; }
	bool get_mem_32b(int offset, int *value) const;

	int get_C0_register(int nr, int sel);

	void set_register(int nr, int value);
	void set_PC(int value) { PC = value; }
	void set_HI(int value) { HI = value; }
	void set_LO(int value) { LO = value; }

	void set_C0_register(int nr, int sel, int value);

	void reset();
	void tick();

	static const char * reg_to_name(int reg);
	static std::string decode_to_text(int instr);
	std::string da_logline(int instr);
};

#endif
