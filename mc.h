#include <pthread.h>

#include "memory.h"
#include "processor.h"
#define REGS_DIV 8

typedef enum { vdma_stopped, vdma_running } vdma_state_t;

class mc : public memory
{
private:
	processor *const pp;
	uint32_t refresh_counter;
	uint32_t regs[128];
	uint32_t RPSS_CTR;

	pthread_mutex_t semaphore_lock;
	uint8_t user_semaphores[16];
	uint8_t sys_semaphore;

	uint32_t RPSS_DIVIDER;

	// VDMA
	uint32_t DMA_MEMADDR;
	uint32_t DMA_SIZE;
	uint32_t DMA_STRIDE;
	uint32_t DMA_MODE;
	uint32_t DMA_COUNT;
	uint32_t DMA_GIO_ADR;
	uint32_t DMA_STDMA;
	vdma_state_t vdma_state;
	void set_dma_default();

public:
    mc(processor *const pp);
	~mc();

	uint64_t get_size() const { return 0x100000; }
	uint64_t get_mask() const { return  0xfffff; }

	void read_32b(uint64_t offset, uint32_t *data);
	void write_32b(uint64_t offset, uint32_t data);
};
