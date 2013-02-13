#include <string.h>

#include "mc.h"

mc::mc(debug_console *pdc_in) : pdc(pdc_in)
{
	pm = NULL;
	len = 0;

	refresh_counter = 0;

	sys_semaphore = 0;
	memset(user_semaphores, 0x00, sizeof user_semaphores);
	pthread_mutex_init(&semaphore_lock, NULL);
}

mc::~mc()
{
}

void mc::read_32b(uint64_t offset, uint32_t *data)
{
	uint32_t index = offset / 4;
	if (index < 128)
		*data = regs[index];

	if (offset <= 0x0f)	// CPUCTRL 0 & 1
	{
		pdc -> dc_log("CPUCTRL0/1 read (%08x)", *data);
	}
	else if (offset == 0x2c)	// RPSS_DIVIDER 
	{
		pdc -> dc_log("RPSS_DIVIDER read (%08x)", *data);
	}
	else if (offset == 0x34)	// EEROM
	{
		pdc -> dc_log("EEROM read (%08x)", *data);

		// bit 1: endiannes
		// bit 2: register size (32/64)
		// *data = 0x0f;
	}
	else if (offset == 0x4c)	// REF_CTR - refresh counter
	{
		*data = refresh_counter--;
		pdc -> dc_log("REF_CTR read (%08x)", *data);
	}
	else if (offset == 0xc4)	// MEMCFG0
	{
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
		*data = 0xFF20FF40;
		pdc -> dc_log("MEMCFG0 read (%08x)", *data);
	}
	else if (offset == 0xcc)	// MEMCFG1
	{
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
		*data = 0xFF40FF80;
		pdc -> dc_log("MEMCFG1 read (%08x)", *data);
	}
	else if (offset == 0xd4)	// CPU_MEMACC
	{
		pdc -> dc_log("CPU_MEMACC read (%08x)", *data);
	}
	else if (offset == 0xdc)	// GIO_MEMACC
	{
		pdc -> dc_log("GIO_MEMACC read (%08x)", *data);
	}
	else if (offset == 0xec)	// CPU_ERROR_STAT 
	{
		pdc -> dc_log("CPU_ERROR_STAT read (%08x)", *data);
	}
	else if (offset == 0xf8 || offset == 0xfc)	// GIO error status
	{
		pdc -> dc_log("GIO_ERROR_STATUS read (%08x)", *data);
	}
		// 0x10000 - 0x1f000
	else if (offset == 0x104)	// SYS_SEMAPHORE
	{
		pthread_mutex_lock(&semaphore_lock);
		*data = sys_semaphore;
		sys_semaphore = 1;
		pthread_mutex_unlock(&semaphore_lock);
	}
	else if (offset == 0x1004)	// RPSS_CTR
	{
		*data = RPSS_CTR++;
		pdc -> dc_log("GIO_ERROR_STATUS read: %08x", *data);
	}
	else if (offset >= 0x100000 && offset <= 0x1ffff)	// USER_SEMAPHORES
	{
		int nr = offset >> 13;

		pthread_mutex_lock(&semaphore_lock);
		*data = user_semaphores[nr];
		user_semaphores[nr] = 1;
		pthread_mutex_unlock(&semaphore_lock);
	}
	else
	{
		pdc -> dc_log("MC read %016llx", offset);
	}
}

void mc::write_32b(uint64_t offset, uint32_t data)
{
	if (offset <= 0x0f)	// CPUCTRL 0 & 1
	{
		pdc -> dc_log("CPUCTRL0/1 write: %08x", data);
	}
	else if (offset == 0x2c)	// RPSS_DIVIDER 
	{
		pdc -> dc_log("RPSS_DIVIDER write: %08x", data);
	}
	else if (offset == 0x34)	// EEROM
	{
		pdc -> dc_log("EEROM write @ %016llx: %016llx %c%c%c%c", offset, data, data & 8?'1':'0', data&4?'1':'0', data&2?'1':'0', data&1?'1':'0');
	}
	else if (offset == 0x84)	// GIO64_ARB
	{
		pdc -> dc_log("GIO64_ARB write: %08x", data);
	}
	else if (offset == 0xc4)	// MEMCFG0
	{
		pdc -> dc_log("MEMCFG0 write: %08x", data);
		// should be r/o
	}
	else if (offset == 0xcc)	// MEMCFG1
	{
		pdc -> dc_log("MEMCFG1 write: %08x", data);
		// should be r/o
	}
	else if (offset == 0xd4)	// CPU_MEMACC
	{
		pdc -> dc_log("CPU_MEMACC write: %08x", data);
	}
	else if (offset == 0xdc)	// GIO_MEMACC
	{
		pdc -> dc_log("GIO_MEMACC write: %08x", data);
	}
	else if (offset == 0xec)	// CPU_ERROR_STAT 
	{
		pdc -> dc_log("CPU_ERROR_STAT write: %08x", data);
		data = 0;
	}
	else if (offset == 0xfc)	// GIO error status
	{
		pdc -> dc_log("GIO_ERROR_STATUS write: %08x", data);
		data = 0;
	}
	else if (offset == 0x104)	// SYS_SEMAPHORE
	{
		pthread_mutex_lock(&semaphore_lock);
		sys_semaphore = 0;
		pthread_mutex_unlock(&semaphore_lock);
	}
	else if (offset == 0x1004)	// RPSS_CTR
	{
		pdc -> dc_log("GIO_ERROR_STATUS write: %08x", data);
	}
	else if (offset >= 0x100000 && offset <= 0x1ffff)	// USER_SEMAPHORES
	{
		int nr = offset >> 13;

		pthread_mutex_lock(&semaphore_lock);
		user_semaphores[nr] = 0;
		pthread_mutex_unlock(&semaphore_lock);
	}
	else
	{
		pdc -> dc_log("MC write @ %016llx: %016llx %c%c%c%c", offset, data, data & 8?'1':'0', data&4?'1':'0', data&2?'1':'0', data&1?'1':'0');
	}

	uint32_t index = offset / 4;
	if (index < 128)
		regs[index] = data;
}
