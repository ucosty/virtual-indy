#include "memory.h"
#include "debug_console.h"

class hpc3 : public memory
{
private:
	debug_console *pdc;

public:
	hpc3(debug_console *pdc_in);
	~hpc3();

	uint64_t get_size() const { return 0x80000; }
	uint64_t get_mask() const { return 0x7ffff; }
};
