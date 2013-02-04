#include "memory_bus.h"

class processor
{
private:
	memory_bus *pmb;
	int registers[32], pc;

public:
	processor(memory_bus *pmb_in);
	~processor();
};
