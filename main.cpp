#include <signal.h>
#include <stdio.h>

#include "error.h"
#include "processor.h"
#include "processor_utils.h"
#include "utils.h"
#include "debug_console.h"
#include "log.h"

bool step = true;

volatile bool terminate = false;

void sig_handler(int sig)
{
        terminate = true;
}

#include <ncurses.h>
int main(int argc, char *argv[])
{
	dolog("*** START ***");

	signal(SIGHUP , SIG_IGN);
	signal(SIGTERM, sig_handler);
	signal(SIGINT , sig_handler);
	signal(SIGQUIT, sig_handler);
	signal(SIGPIPE, SIG_IGN);

	debug_console *dc = new debug_console();

	dc -> init();

	memory_bus *mb = new memory_bus();

	memory *m = new memory(64 * 1024 * 1024, true);
	mb -> register_memory(0, 0xfff, m);

	unsigned char *prom = NULL;
	int prom_len = -1;
	load_file("ip20prom.070-8116-004.BE.bin", &prom, &prom_len);
	memory *m_prom = new memory(prom, prom_len);
	mb -> register_memory(0xbfc00000, 0x7ffff, m_prom); // IP20, 32bit

	processor *p = new processor(dc, mb);
	//p -> set_PC(0xbfc00000 + 0x0b80); // offset 0xb80 entry point is a guess
	//p -> set_PC(0xbfc00000 + 0x884); // guess
	p -> set_PC(0xbfc00000 + 0x880); // guess
/*
	int prom_start_offset = -1;
	if (!m_prom -> read_32b(0x0, &prom_start_offset))
		error_exit("Failed to retrieve boot vector from PROM");
	// NOTE: it *might* be that for the PROM this all starts at 0x000000 due to address translation (phys versus virt)
	// need to verify
	p -> set_PC(0xbfc00000 + prom_start_offset);
*/

	for(;!terminate;)
	{
		dc -> tick(p);
		p -> tick();

		if (step)
			getch();
	}

	delete dc;

	delete p;
	delete mb;
	delete m;

	dolog("--- END ---");

	return 0;
}
