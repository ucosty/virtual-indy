#include "memory_bus.h"

memory_bus::memory_bus()
{
}

memory_bus::~memory_bus()
{
}

void memory_bus::register_memory(int offset, int mask, memory *target)
{
	memory_segment_t segment = { offset, mask, target };

	list.push_back(segment);
}

bool memory_bus::read_32b(int offset, int *data)
{
	for(int segment = 0; segment < list.size(); segment++)
	{
		memory_segment_t *cur = &list.at(segment);

		if (offset &  cur -> mask == cur -> offset)
			return cur -> target -> read_32b(offset - cur -> offset, data);
	}

	return false;
}

bool memory_bus::write_32b(int offset, int data)
{
	for(int segment = 0; segment < list.size(); segment++)
	{
		memory_segment_t *cur = &list.at(segment);

		if (offset &  cur -> mask == cur -> offset)
			return cur -> target -> write_32b(offset - cur -> offset, data);
	}

	return false;
}
