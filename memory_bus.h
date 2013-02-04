#include <vector>

#include "memory.h"

typedef struct
{
	int offset;
	int mask;
	memory *target;
} memory_segment_t;

class memory_bus
{
private:
	std::vector<memory_segment_t> list;
public:
	memory_bus();
	~memory_bus();

	void register_memory(int offset, int length, memory *target);

	bool read_32b(int offset, int *data);
	bool write_32b(int offset, int data);
};
