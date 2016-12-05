#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "mc.h"
#include "utils.h"
#include "processor.h"

mc::mc(processor *const ppIn) : pp(ppIn)
{
	pm = NULL;
	len = 0;

	refresh_counter = 0;

	sys_semaphore = 0;
	memset(user_semaphores, 0x00, sizeof user_semaphores);
	pthread_mutex_init(&semaphore_lock, NULL);

	RPSS_DIVIDER = 0;

	// BASE0
	// 31 1 served
	// 30 1 number of subbanks
	// 29 1 bank 0/2 valid
	// 28/27/26/25/24 11111 sim size 8M x 36 bits, 2 subbanks
	// 23-16 00100000 (0x8000000 >> 22), 
	// 1111111100100000
	// BASE1
	// 15 1 served
	// 14 1 number of subbanks
	// 13 1 bank 0/2 valid
	// 12/11/10/09/08 11111 sim size 8M x 36 bits, 2 subbanks
	// 07-00 01000000 (0x10000000 >> 22), 
	// 1111111101000000
	// 11111111001000001111111101000000
	regs[0xc4 / REGS_DIV] = 0xFF20FF40;

	// BASE0
	// 31 1 served
	// 30 1 number of subbanks
	// 29 1 bank 0/2 valid
	// 28/27/26/25/24 11111 sim size 8M x 36 bits, 2 subbanks
	// 23-16 01000000 (0x20000000 >> 22), 
	// 1111111101000000
	// BASE1
	// 15 1 served
	// 14 1 number of subbanks
	// 13 1 bank 0/2 valid
	// 12/11/10/09/08 11111 sim size 8M x 36 bits, 2 subbanks
	// 07-00 10000000 (0x30000000 >> 22), 
	// 1111111110000000
	// 11111111010000001111111110000000
	regs[0xcc / REGS_DIV] = 0xFF40FF80;

	uint32_t CPU_MEMACC = 0;
	CPU_MEMACC |= 0x03 << 0;	// WR_COL
	CPU_MEMACC |= 0x03 << 4;	// RD_COL
	CPU_MEMACC |= 0x03 << 8;	// ROW
	CPU_MEMACC |= 0x04 << 12;	// RASH
	CPU_MEMACC |= 0x05 << 16;	// RCASL
	CPU_MEMACC |= 0x04 << 20;	// RASL
	CPU_MEMACC |= 0x01 << 24;	// CBR
	CPU_MEMACC |= 0x00 << 28;	// CAS_HALF
	regs[0xd4 / REGS_DIV] = CPU_MEMACC;

	uint32_t GIO_MEMACC = 0;
	GIO_MEMACC |= 0x03 << 0;	// WR_COL
	GIO_MEMACC |= 0x03 << 4;	// RD_COL
	GIO_MEMACC |= 0x03 << 8;	// ROW
	GIO_MEMACC |= 0x04 << 12;	// RASH
	GIO_MEMACC |= 0x00 << 16;	// CAS_HALF
	GIO_MEMACC |= 0x00 << 17;	// ADDR_HALF
	regs[0xdc / REGS_DIV] = GIO_MEMACC;
}

mc::~mc()
{
}

void mc::read_32b(uint64_t offset, uint32_t *data)
{
	uint32_t index = offset / REGS_DIV;
	if (index < 128)
		*data = regs[index];

	offset &= ~4; // 0x0c -> 0x08

    // pdc -> dc_log("MC read from %016llx", offset);

	if (offset <= 0x0f)	// CPUCTRL 0 & 1
	{
        // pdc -> dc_log("MC CPUCTRL0/1 read (%08x)", *data);
	}
	else if (offset == 0x28)	// RPSS_DIVIDER 
		*data = RPSS_DIVIDER;
	else if (offset == 0x30)	// EEROM
	{
        // pdc -> dc_log("MC EEROM read (%08x)", *data);

		// bit 1: endiannes
		// bit 2: register size (32/64)
		// *data = 0x0f;

		(*data) ^= rand();
		(*data) &= 0x1e;
	}
	else if (offset == 0x48)	// REF_CTR - refresh counter
	{
		*data = refresh_counter--;
        // pdc -> dc_log("MC REF_CTR read (%08x)", *data);
	}
	else if (offset == 0xc0)	// MEMCFG0
	{
		//*data = 0x3110;
		std::string dummy = to_bin_str(*data, 32);
        // pdc -> dc_log("MC MEMCFG0 read (%08x) %s", *data, dummy.c_str());
	}
	else if (offset == 0xc8)	// MEMCFG1
	{
		//*data = 0;
		std::string dummy = to_bin_str(*data, 32);
        // pdc -> dc_log("MC MEMCFG1 read (%08x) %s", *data, dummy.c_str());
	}
	else if (offset == 0xd0)	// CPU_MEMACC
	{
        // pdc -> dc_log("MC CPU_MEMACC read (%08x)", *data);
	}
	else if (offset == 0xd8)	// GIO_MEMACC
	{
        // pdc -> dc_log("MC GIO_MEMACC read (%08x)", *data);
	}
	else if (offset == 0xe8)	// CPU_ERROR_STAT 
	{
        // pdc -> dc_log("MC CPU_ERROR_STAT read (%08x)", *data);
	}
	else if (offset == 0xf8)	// GIO error status
	{
        // pdc -> dc_log("MC GIO_ERROR_STATUS read (%08x)", *data);
	}
		// 0x10000 - 0x1f000
	else if (offset == 0x100)	// SYS_SEMAPHORE
	{
//        DEBUG(pdc -> dc_log("MC SYS_SEMAPHORE read (%08x)", *data));
		pthread_mutex_lock(&semaphore_lock);
		*data = sys_semaphore;
		sys_semaphore = 1;
		pthread_mutex_unlock(&semaphore_lock);
	}
	else if (offset == 0x1000)	// RPSS_CTR
	{
		int div = (RPSS_DIVIDER & 0xff) + 1;
		int mul = ((RPSS_DIVIDER >> 8) & 0xff) + 1;

		unsigned long long int cycles = pp -> get_cycle_count();
		*data = (cycles / div) * mul;

        // pdc -> dc_log("MC RPSS_CTR read: %08x (%llx)", *data, cycles);
	}
	else if (offset == 0x2000 || offset == 0x2008)	// DMA_MEMADRD, like DMA_MEMADR but also sets defaults
		*data = DMA_MEMADDR;
	else if (offset == 0x2010)
		*data = DMA_SIZE;
	else if (offset == 0x2018)
		*data = DMA_STRIDE;
	else if (offset == 0x2020 || offset == 0x2028 || offset == 0x2070)
		*data = DMA_GIO_ADR;
	else if (offset == 0x2030)
		*data = DMA_MODE;
	else if (offset == 0x2038)
		*data = DMA_COUNT;
	else if (offset == 0x2040)
		*data = DMA_STDMA;
	else if (offset == 0x2048) {	// DMA_RUN
		*data = 0;

		if (vdma_state == vdma_running) {
			*data = -1;
			vdma_state = vdma_stopped;
		}
		else {
			*data = 0;
		}

        // pdc -> dc_log("MC DMA_RUN read: %x", *data);
	}
	else if (offset >= 0x100000 && offset <= 0x1ffff)	// USER_SEMAPHORES
	{
        // DEBUG( pdc -> dc_log("MC USER_SEMAPHORES read: %08x", *data));
		int nr = offset >> 13;

		pthread_mutex_lock(&semaphore_lock);
		*data = user_semaphores[nr];
		user_semaphores[nr] = 1;
		pthread_mutex_unlock(&semaphore_lock);
	}
	else
	{
        // pdc -> dc_log("MC read %016llx not implemented", offset);
	}
}

void mc::set_dma_default()
{
    // pdc -> dc_log("MC set VDMA defaults");

	DMA_SIZE = 0x1000c;
	DMA_STRIDE = 0x10000;
	DMA_MODE = 0xd0;
	DMA_COUNT = 0x10001;
}

void mc::write_32b(uint64_t offset, uint32_t data)
{
	offset &= ~4; // 0x0c -> 0x08

    // pdc -> dc_log("MC write @ %016llx: %016llx", offset, data);

	if (offset <= 0x0f)	// CPUCTRL 0 & 1
	{
        // pdc -> dc_log("MC CPUCTRL0/1 write: %08x", data);
	}
	else if (offset == 0x28)	// RPSS_DIVIDER 
	{
        // pdc -> dc_log("MC RPSS_DIVIDER write: %08x", data);
		RPSS_DIVIDER = data;
	}
	else if (offset == 0x30)	// EEROM
	{
        // pdc -> dc_log("MC EEROM write @ %016llx: %016llx %c%c%c%c", offset, data, data & 8?'1':'0', data&4?'1':'0', data&2?'1':'0', data&1?'1':'0');
	}
	else if (offset == 0x80)	// GIO64_ARB
	{
        // pdc -> dc_log("MC GIO64_ARB write: %08x", data);
	}
	else if (offset == 0xc0)	// MEMCFG0
	{
        // pdc -> dc_log("MC MEMCFG0 write: %08x", data);
        // pdc -> dc_log("MC   b0: %08lx", (data & 0xff) << 22);
        // pdc -> dc_log("MC   b1: %08lx", ((data & 0xff0000) >> 16) << 22);
	}
	else if (offset == 0xc8)	// MEMCFG1
	{
        // pdc -> dc_log("MC MEMCFG1 write: %08x", data);
        // pdc -> dc_log("MC   b0: %08lx", (data & 0xff) << 22);
        // pdc -> dc_log("MC   b1: %08lx", ((data & 0xff0000) >> 16) << 22);
	}
	else if (offset == 0xd0)	// CPU_MEMACC
	{
        // pdc -> dc_log("MC CPU_MEMACC write: %08x", data);
	}
	else if (offset == 0xd8)	// GIO_MEMACC
	{
        // pdc -> dc_log("MC GIO_MEMACC write: %08x", data);
	}
	else if (offset == 0xe8)	// CPU_ERROR_STAT 
	{
        // pdc -> dc_log("MC CPU_ERROR_STAT write: %08x", data);
		data = 0;
	}
	else if (offset == 0xf8)	// GIO error status
	{
        // pdc -> dc_log("MC GIO_ERROR_STATUS write: %08x", data);
		data = 0;
	}
	else if (offset == 0x100)	// SYS_SEMAPHORE
	{
		pthread_mutex_lock(&semaphore_lock);
		sys_semaphore = 0;
		pthread_mutex_unlock(&semaphore_lock);
	}
	else if (offset == 0x1000)	// RPSS_CTR
	{
        // pdc -> dc_log("MC GIO_ERROR_STATUS write: %08x", data);
	}
	else if (offset == 0x2000 || offset == 0x2008) {	// DMA_MEMADR, DMA_MEMADRD, like DMA_MEMADR but also sets defaults
		DMA_MEMADDR = data;

		if (offset == 0x2008) {
            // pdc -> dc_log("MC write DMA_MEMADRD %x", data);
			set_dma_default();
		}
		else {
            // pdc -> dc_log("MC write DMA_MEMADR %x", data);
		}
	}
	else if (offset == 0x2010) {
		DMA_SIZE = data;
        // pdc -> dc_log("MC write DMA_SIZE %x", data);

		DMA_COUNT = (DMA_COUNT & ~0xffff) | (data & 0xffff);
	}
	else if (offset == 0x2018) {
		DMA_STRIDE = data;
        // pdc -> dc_log("MC write DMA_STRIDE %x", data);

		uint16_t line_zoom = (data >> 16) & 511;
		DMA_COUNT = (DMA_COUNT & ~0xffff0000) | (line_zoom << 16);
	}
	else if (offset == 0x2020) {
		DMA_GIO_ADR = data;
        // pdc -> dc_log("MC write DMA_GIO_ADR %x", data);
	}
	else if (offset == 0x2028 || offset == 0x2040 || offset == 0x2070)
	{
		if (offset == 0x2028 || offset == 0x2070)
			DMA_GIO_ADR = data;
		else if (offset == 0x2040)
			DMA_STDMA = data;

		if (offset == 0x2070)
			set_dma_default();

		vdma_state = vdma_running;
		// FIXME start DMA

        // pdc -> dc_log("MC start VDMA");
        // pdc -> dc_log(" DMA_MEMADDR: %llx", DMA_MEMADDR);
        // pdc -> dc_log(" DMA_SIZE:    %llx", DMA_SIZE);
        // pdc -> dc_log(" DMA_STRIDE:  %llx", DMA_STRIDE);
        // pdc -> dc_log(" DMA_MODE:    %llx", DMA_MODE);
        // pdc -> dc_log(" DMA_COUNT:   %llx", DMA_COUNT);
        // pdc -> dc_log(" DMA_GIO_ADR: %llx", DMA_GIO_ADR);
        // pdc -> dc_log(" DMA_STDMA:   %llx", DMA_STDMA);
	}
	else if (offset == 0x2030) {
		DMA_MODE = data;
        // pdc -> dc_log("MC write DMA_MODE %x", data);
	}
	else if (offset == 0x2038) {
		DMA_COUNT = data;
        // pdc -> dc_log("MC write DMA_COUNT %x", data);
	}
    else if (offset == 0x2048) {
        // pdc -> dc_log("MC Should not write (%x) to DMA_RUN", data);
    } else if (offset >= 0x100000 && offset <= 0x1ffff) { // USER_SEMAPHORES
		int nr = offset >> 13;

		pthread_mutex_lock(&semaphore_lock);
		user_semaphores[nr] = 0;
		pthread_mutex_unlock(&semaphore_lock);
	}
	else
	{
        // pdc -> dc_log("MC write @ %016llx: %016llx %c%c%c%c not implemented", offset, data, data & 8?'1':'0', data&4?'1':'0', data&2?'1':'0', data&1?'1':'0');
	}

	uint32_t index = offset / REGS_DIV;
	if (index < 128)
		regs[index] = data;
}
