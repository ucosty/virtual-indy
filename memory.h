#ifndef __MEMORY__H__
#define __MEMORY__H__

#include <stdint.h>

class memory
{
protected:
	unsigned char *pm;
	uint64_t len;

	memory();

public:
	memory(uint64_t size, bool init);
	memory(unsigned char *p, uint64_t len);
	~memory();

	uint64_t get_size() { return len; }

	void read_64b(uint64_t offset, uint64_t *data) const;
	void read_32b(uint64_t offset, uint32_t *data) const;
	void read_16b(uint64_t offset, uint16_t *data) const;
	void read_8b(uint64_t offset, uint8_t *data) const;
	void write_64b(uint64_t offset, uint64_t data);
	void write_32b(uint64_t offset, uint32_t data);
	void write_16b(uint64_t offset, uint16_t data);
	void write_8b(uint64_t offset, uint8_t data);
};

#endif
