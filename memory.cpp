#include <endian.h>
#include <string.h>

#include "error.h"
#include "memory.h"

memory::memory()
{
}

memory::memory(uint64_t size, bool init) : len(size)
{
	if (size < 0)
		error_exit("memory::memory invalid size %016llx", size);

	pm = new unsigned char[size];

	if (init)
		memset(pm, 0x00, size);
}

memory::memory(unsigned char *p, uint64_t size) : pm(p), len(size)
{
	if (size < 0)
		error_exit("memory::memory invalid size %016llx", size);
}

memory::~memory()
{
	delete [] pm;
}

void memory::read_64b(uint64_t offset, uint64_t *data) const
{
	unsigned char *dummy_p = &pm[offset];
	uint64_t dummy = *(uint64_t *)dummy_p;

	*data = be64toh(dummy);
}

void memory::read_32b(uint64_t offset, uint32_t *data) const
{
	unsigned char *dummy_p = &pm[offset];
	uint32_t dummy = *(uint32_t *)dummy_p;

	*data = be32toh(dummy);
}

void memory::read_16b(uint64_t offset, uint16_t *data) const
{
	unsigned char *dummy_p = &pm[offset];
	uint16_t dummy = *(uint16_t *)dummy_p;

	*data = be16toh(dummy);
}

void memory::read_8b(uint64_t offset, uint8_t *data) const
{
	*data = pm[offset];
}

void memory::write_64b(uint64_t offset, uint64_t data)
{
	unsigned char *dummy_p = &pm[offset];

	*(uint64_t *)dummy_p = htobe64(data);
}

void memory::write_32b(uint64_t offset, uint32_t data)
{
	unsigned char *dummy_p = &pm[offset];

	*(uint32_t *)dummy_p = htobe32(data);
}

void memory::write_16b(uint64_t offset, uint16_t data)
{
	unsigned char *dummy_p = &pm[offset];

	*(uint16_t *)dummy_p = htobe16(data);
}

void memory::write_8b(uint64_t offset, uint8_t data)
{
	pm[offset] = data;
}
