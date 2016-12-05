#include "seeq_8003_8020.h"

seeq_8003_8020::seeq_8003_8020()
{
}

seeq_8003_8020::~seeq_8003_8020()
{
}

static uint32_t bla;

void seeq_8003_8020::read_32b(uint64_t offset, uint32_t *data)
{
	*data = bla;
}

void seeq_8003_8020::write_32b(uint64_t offset, uint32_t data)
{
	bla = data;
}
