#include "memory_bus.h"

class processor
{
private:
	memory_bus *pmb;
	int registers[32], pc;

	void i_type(int opcode, int instruction);
	void j_type(int opcode, int instruction);
	void r_type(int opcode, int instruction);
	void ipco(int opcode, int instruction);

	int twos_complement(int value, int bits) const;

public:
	processor(memory_bus *pmb_in);
	~processor();

	void tick();
};
