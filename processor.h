#ifndef __PROCESSOR__H__
#define __PROCESSOR__H__

#include <stdint.h>
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
	uint64_t registers[32], PC, HI, LO;
	uint32_t status_register;
	uint32_t C0_registers[32];

	long long int cycles;

	void j_type(uint8_t opcode, uint32_t instruction);
	void special2(uint32_t instruction);
	void special3(uint32_t instruction);
	void COP0(uint32_t instruction);
	// not in a group? FIXME
	void SLTI(uint32_t instruction);
	void regimm(uint32_t instruction);
	void interrupt(int nr);

	void init_r_type();
	void r_type_00(uint32_t instruction);
	void r_type_01(uint32_t instruction);
	void r_type_02(uint32_t instruction);
	void r_type_03(uint32_t instruction);
	void r_type_04(uint32_t instruction);
	void r_type_05(uint32_t instruction);
	void r_type_06(uint32_t instruction);
	void r_type_07(uint32_t instruction);
	void r_type_08(uint32_t instruction);
	void r_type_09(uint32_t instruction);
	void r_type_0a(uint32_t instruction);
	void r_type_0b(uint32_t instruction);
	void r_type_0c(uint32_t instruction);
	void r_type_0d(uint32_t instruction);
	void r_type_0e(uint32_t instruction);
	void r_type_0f(uint32_t instruction);
	void r_type_10(uint32_t instruction);
	void r_type_11(uint32_t instruction);
	void r_type_12(uint32_t instruction);
	void r_type_13(uint32_t instruction);
	void r_type_14(uint32_t instruction);
	void r_type_15(uint32_t instruction);
	void r_type_16(uint32_t instruction);
	void r_type_17(uint32_t instruction);
	void r_type_18(uint32_t instruction);
	void r_type_19(uint32_t instruction);
	void r_type_1a(uint32_t instruction);
	void r_type_1b(uint32_t instruction);
	void r_type_1c(uint32_t instruction);
	void r_type_1d(uint32_t instruction);
	void r_type_1e(uint32_t instruction);
	void r_type_1f(uint32_t instruction);
	void r_type_20(uint32_t instruction);
	void r_type_21(uint32_t instruction);
	void r_type_22(uint32_t instruction);
	void r_type_23(uint32_t instruction);
	void r_type_24(uint32_t instruction);
	void r_type_25(uint32_t instruction);
	void r_type_26(uint32_t instruction);
	void r_type_27(uint32_t instruction);
	void r_type_28(uint32_t instruction);
	void r_type_29(uint32_t instruction);
	void r_type_2a(uint32_t instruction);
	void r_type_2b(uint32_t instruction);
	void r_type_2c(uint32_t instruction);
	void r_type_2d(uint32_t instruction);
	void r_type_2e(uint32_t instruction);
	void r_type_2f(uint32_t instruction);
	void r_type_30(uint32_t instruction);
	void r_type_31(uint32_t instruction);
	void r_type_32(uint32_t instruction);
	void r_type_33(uint32_t instruction);
	void r_type_34(uint32_t instruction);
	void r_type_35(uint32_t instruction);
	void r_type_36(uint32_t instruction);
	void r_type_37(uint32_t instruction);
	void r_type_38(uint32_t instruction);
	void r_type_39(uint32_t instruction);
	void r_type_3a(uint32_t instruction);
	void r_type_3b(uint32_t instruction);
	void r_type_3c(uint32_t instruction);
	void r_type_3d(uint32_t instruction);
	void r_type_3e(uint32_t instruction);
	void r_type_3f(uint32_t instruction);
	void (processor::*r_type_methods[64])(uint32_t);

	void init_i_type();
	void i_type_00(uint32_t instruction);
	void i_type_01(uint32_t instruction);
	void i_type_02(uint32_t instruction);
	void i_type_03(uint32_t instruction);
	void i_type_04(uint32_t instruction);
	void i_type_05(uint32_t instruction);
	void i_type_06(uint32_t instruction);
	void i_type_07(uint32_t instruction);
	void i_type_08(uint32_t instruction);
	void i_type_09(uint32_t instruction);
	void i_type_0a(uint32_t instruction);
	void i_type_0b(uint32_t instruction);
	void i_type_0c(uint32_t instruction);
	void i_type_0d(uint32_t instruction);
	void i_type_0e(uint32_t instruction);
	void i_type_0f(uint32_t instruction);
	void i_type_10(uint32_t instruction);
	void i_type_11(uint32_t instruction);
	void i_type_12(uint32_t instruction);
	void i_type_13(uint32_t instruction);
	void i_type_14(uint32_t instruction);
	void i_type_16(uint32_t instruction);
	void i_type_17(uint32_t instruction);
	void i_type_18(uint32_t instruction);
	void i_type_19(uint32_t instruction);
	void i_type_1a(uint32_t instruction);
	void i_type_1b(uint32_t instruction);
	void i_type_1c(uint32_t instruction);
	void i_type_1d(uint32_t instruction);
	void i_type_1e(uint32_t instruction);
	void i_type_1f(uint32_t instruction);
	void i_type_20(uint32_t instruction);
	void i_type_21(uint32_t instruction);
	void i_type_22(uint32_t instruction);
	void i_type_23(uint32_t instruction);
	void i_type_26(uint32_t instruction);
	void i_type_27(uint32_t instruction);
	void i_type_28(uint32_t instruction);
	void i_type_29(uint32_t instruction);
	void i_type_2a(uint32_t instruction);
	void i_type_2b(uint32_t instruction);
	void i_type_2c(uint32_t instruction);
	void i_type_2d(uint32_t instruction);
	void i_type_2e(uint32_t instruction);
	void i_type_2f(uint32_t instruction);
	void i_type_31(uint32_t instruction);
	void i_type_32(uint32_t instruction);
	void i_type_33(uint32_t instruction);
	void i_type_34(uint32_t instruction);
	void i_type_35(uint32_t instruction);
	void i_type_36(uint32_t instruction);
	void i_type_37(uint32_t instruction);
	void i_type_38(uint32_t instruction);
	void i_type_39(uint32_t instruction);
	void i_type_3a(uint32_t instruction);
	void i_type_3b(uint32_t instruction);
	void i_type_3c(uint32_t instruction);
	void i_type_3d(uint32_t instruction);
	void i_type_3e(uint32_t instruction);
	void i_type_3f(uint32_t instruction);
	void (processor::*i_type_methods[64])(uint32_t);

public:
	processor(debug_console *pdc_in, memory_bus *pmb_in);
	~processor();

	int32_t get_register_32b_signed(uint8_t nr) const;
	uint32_t get_register_32b_unsigned(uint8_t nr) const;
	int64_t get_register_64b_signed(uint8_t nr) const;
	uint64_t get_register_64b_unsigned(uint8_t nr) const;
	uint64_t get_PC() const { return PC; }
	uint64_t get_HI() const { return HI; }
	uint64_t get_LO() const { return LO; }
	uint64_t get_SR() const { return status_register; }

	bool get_mem_32b(int offset, uint32_t *value) const;

	uint32_t get_C0_register(uint8_t nr, uint8_t sel);

	void set_register_32b(uint8_t nr, uint32_t value);
	void set_register_32b_se(uint8_t nr, int32_t value); // < 32b signed d'r in, is dan niet automatisch s-e?
	void set_register_64b(uint8_t nr, uint64_t value);
	void set_PC(uint32_t value) { PC = value; }
	void set_HI(uint32_t value) { HI = value; }
	void set_LO(uint32_t value) { LO = value; }

	void set_C0_register(uint8_t nr, uint8_t sel, uint32_t value);

	void reset();
	void tick();

	static const char * reg_to_name(uint8_t reg);
	static std::string decode_to_text(uint32_t instr);
	std::string da_logline(uint32_t instr);
};

#endif
