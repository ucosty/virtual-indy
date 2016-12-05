#include <stdint.h>

class seeq_8003_8020
{
private:

public:
    seeq_8003_8020();
	~seeq_8003_8020();

        void read_32b(uint64_t offset, uint32_t *data);
        void write_32b(uint64_t offset, uint32_t data);
};
