#include <stdint.h>

#include "debug_console.h"

class seeq_8003_8020
{
private:

public:
	seeq_8003_8020(debug_console *pdc_in);
	~seeq_8003_8020();

        void read_32b(uint64_t offset, uint32_t *data);
        void write_32b(uint64_t offset, uint32_t data);
};
