#include "memory_bus.h"

class processor
{
private:
	memory_bus *pmb;
	int registers[32], pc;

	void j_type(int opcode, int instruction);
	void ipco(int opcode, int instruction);

public:
	processor(memory_bus *pmb_in);
	~processor();

	void tick();
};
