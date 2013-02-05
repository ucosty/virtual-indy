#include <signal.h>
#include <stdio.h>

#include "error.h"
#include "processor.h"
#include "processor_utils.h"
#include "utils.h"
#include "debug_console.h"

volatile bool terminate = false;

void sig_handler(int sig)
{
        terminate = true;
}

int main(int argc, char *argv[])
{
	signal(SIGHUP , SIG_IGN);
	signal(SIGTERM, sig_handler);
	signal(SIGINT , sig_handler);
	signal(SIGQUIT, sig_handler);
	signal(SIGPIPE, SIG_IGN);

	debug_console *dc = new debug_console();

	dc -> init();

	memory_bus *mb = new memory_bus();

	memory *m = new memory(4 * 1024);
	mb -> register_memory(0, 0xfff, m);

	unsigned char *prom = NULL;
	int prom_len = -1;
	load_file("ip20prom.070-8116-004.BE.bin", &prom, &prom_len);
	memory *m_prom = new memory(prom, prom_len);
	mb -> register_memory(0xbfc00000, 0x7ffff, m_prom); // IP20, 32bit

	processor *p = new processor(dc, mb);
	p -> set_PC(0xbfc00000 + 0x0b80); // offset 0xb80 entry point is a guess

	for(;!terminate;)
	{
		dc -> tick(p);
		p -> tick();
getch();
	}

	delete dc;

	delete p;
	delete mb;
	delete m;

	return 0;
}
