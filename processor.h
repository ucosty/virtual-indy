#include "memory_bus.h"

class processor
{
private:
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
	int registers[32], PC, HI, LO;

	void i_type(int opcode, int instruction);
	void j_type(int opcode, int instruction);
	void r_type(int opcode, int instruction);
	void special2(int opcode, int instruction);
	void ipco(int opcode, int instruction);
	void BNEL(int instruction);

public:
	processor(memory_bus *pmb_in);
	~processor();

	int get_register(int nr);
	int get_PC() { return PC; }
	int get_HI() { return HI; }
	int get_LO() { return LO; }

	void set_register(int nr, int value);
	void set_PC(int value) { PC = value; }
	void set_HI(int value) { HI = value; }
	void set_LO(int value) { LO = value; }

	void tick();
};
