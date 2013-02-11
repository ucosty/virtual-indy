#ifndef __MEMORY__H__
#define __MEMORY__H__

#include <stdint.h>

class memory
{
private:
	unsigned char *pm;
	int len;

public:
	memory(int size, bool init);
	memory(unsigned char *p, int len);
	~memory();

	bool read_64b(uint64_t offset, uint64_t *data) const;
	bool read_32b(uint64_t offset, uint32_t *data) const;
	bool read_16b(uint64_t offset, uint16_t *data) const;
	bool read_8b(uint64_t offset, uint8_t *data) const;
	bool write_64b(uint64_t offset, uint64_t data);
	bool write_32b(uint64_t offset, uint32_t data);
	bool write_16b(uint64_t offset, uint16_t data);
	bool write_8b(uint64_t offset, uint8_t data);
};

#endif
