#include "memory.h"

class hpc3 : public memory
{
public:
	hpc3();
	~hpc3();

	uint64_t get_size() { return 0x80000; }
	uint64_t get_mask() { return 0x7ffff; }
};
