#ifndef __PROCESSOR__H__
#define __PROCESSOR__H__

#include <stdint.h>
#include <string>

#include "debug.h"
#include "optimize.h"
#include "debug_console.h"
#include "processor_utils.h"
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
	uint64_t registers[32], PC, HI, LO, EPC;
	uint32_t status_register;
	uint32_t C0_registers[32];

	bool have_delay_slot;
	uint64_t delay_slot_PC;

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

	void conditional_jump(bool do_jump, uint32_t instruction, bool skip_delay_slot);

public:
	processor(debug_console *pdc_in, memory_bus *pmb_in);
	~processor();

	inline bool is_delay_slot() { return have_delay_slot; }
	void set_delay_slot(uint64_t offset);
	uint64_t get_delay_slot_PC();

	inline uint64_t get_PC() const { return PC; }
	inline uint64_t get_HI() const { return HI; }
	inline uint64_t get_LO() const { return LO; }
	inline uint64_t get_SR() const { return status_register; }

	void get_mem_32b(int offset, uint32_t *value) const;

	uint64_t get_C0_register(uint8_t nr, uint8_t sel);

	inline void set_status_register(uint32_t value) { status_register = value; }

	inline void set_PC(uint64_t value) { PC = value; }
	inline void set_HI(uint64_t value) { HI = value; }
	inline void set_LO(uint64_t value) { LO = value; }

	void set_C0_register(uint8_t nr, uint8_t sel, uint64_t value);

	void reset();
	void tick();

	static inline uint8_t get_RS(uint32_t instruction) { return (instruction >> 21) & MASK_5B; }
	static inline uint8_t get_RT(uint32_t instruction) { return (instruction >> 16) & MASK_5B; }
	static inline uint8_t get_RD(uint32_t instruction) { return (instruction >> 11) & MASK_5B; }
	static inline uint8_t get_SA(uint32_t instruction) { return (instruction >>  6) & MASK_5B; }
	static inline int32_t get_SB18(uint32_t instruction) { return int16_t(instruction) << 2; }
	static inline uint16_t get_immediate(uint32_t instruction) { return instruction; }
	static inline uint8_t get_base(uint32_t instruction) { return (instruction >> 21) & MASK_5B; }
	static inline uint8_t get_opcode(uint32_t instruction) { return (instruction >> 26) & MASK_6B; }

	inline uint64_t get_base_register_with_offset(uint32_t instruction)
	{
		uint8_t base = get_base(instruction);
		int offset_s = int16_t(instruction);

		return get_register_64b_unsigned(base) + offset_s;
	}

	static const char * reg_to_name(uint8_t reg);
	static std::string decode_to_text(uint32_t instr);
	std::string da_logline(uint32_t instr);

	inline int32_t get_register_32b_signed(uint8_t nr) const
	{
		ASSERT(nr >= 0 && nr <= 31);

if (nr == 31) pdc -> dc_log("GET 31 signed");

		return int32_t(registers[nr]);
	}

	inline uint32_t get_register_32b_unsigned(uint8_t nr) const
	{
		ASSERT(nr >= 0 && nr <= 31);

if (nr == 31) pdc -> dc_log("GET 31 unsigned");

		return uint32_t(registers[nr]);
	}

	inline int64_t get_register_64b_signed(uint8_t nr) const
	{
		ASSERT(nr >= 0 && nr <= 31);

		return int64_t(registers[nr]);
	}

	inline uint64_t get_register_64b_unsigned(uint8_t nr) const
	{
		ASSERT(nr >= 0 && nr <= 31);

		return uint64_t(registers[nr]);
	}

	inline void set_register_32b(uint8_t nr, uint32_t value)
	{
		ASSERT(nr >= 0 && nr <= 31);

		if (likely(nr))
			registers[nr] = (registers[nr] & ~MASK_32B) | value;
		else
			pdc -> dc_log("(32b) trying to alter register 0! (%d)", nr);
	}

	inline void set_register_32b_se(uint8_t nr, int32_t value)
	{
		ASSERT(nr >= 0 && nr <= 31);

		if (likely(nr))
			registers[nr] = value;
		else
			pdc -> dc_log("(32bse) trying to alter register 0! (%d)", nr);
	}

	inline void set_register_64b(uint8_t nr, uint64_t value)
	{
		ASSERT(nr >= 0 && nr <= 31);

		if (likely(nr))
			registers[nr] = value;
		else
			pdc -> dc_log("(64b) trying to alter register 0! (%d)", nr);
	}
};

#endif
