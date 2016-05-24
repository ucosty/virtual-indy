#include <atomic>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include "error.h"
#include "processor.h"
#include "processor_utils.h"
#include "utils.h"
#include "debug_console.h"
#include "debug_console_simple.h"
#include "rom.h"
#include "hpc3.h"
#include "mc.h"
#include "log.h"

bool single_step = false;
const char *logfile = NULL;

std::atomic_bool sig_terminate(false), sig_interrupt(false);

void help()
{
	fprintf(stderr, "-d     debug (console) mode\n");
	fprintf(stderr, "-S     enable single step mode\n");
	fprintf(stderr, "-l x   logfile to write to\n");
	fprintf(stderr, "-V     show version & exit\n");
	fprintf(stderr, "-h     this help & exit\n");
}

void version()
{
	printf("miep v" VERSION ", (C) 2013-2016 by folkert@vanheusden.com\n\n");
}

void sig_handler(int sig)
{
	sig_interrupt = true;

	if (sig == SIGTERM || sig == SIGINT || sig == SIGQUIT)
		sig_terminate = true;

	// if (sig == SIGALRM)
		// sig_alarm = true;
}

int main(int argc, char *argv[])
{
	int c = -1;
	bool debug = false;

	while((c = getopt(argc, argv, "dSl:")) != -1)
	{
		switch(c)
		{
			case 'd':
				debug = true;
				break;

			case 'S':
				single_step = true;
				break;

			case 'l':
				logfile = optarg;
				break;

			case 'V':
				version();
				return 0;

			case 'h':
				version();
				help();
				return 0;

			default:
				version();
				help();
				return 1;
		}
	}

	dolog("*** START ***");

	signal(SIGHUP , SIG_IGN);
	signal(SIGTERM, sig_handler);
	signal(SIGINT , sig_handler);
	signal(SIGQUIT, sig_handler);
	signal(SIGPIPE, SIG_IGN);

#ifdef _PROFILING
	debug_console *dc = new debug_console_simple();
#else
	debug_console *dc = debug ? new debug_console() : new debug_console_simple();
#endif

	dc -> init();

	memory_bus *mb = new memory_bus(dc);

	processor *p = new processor(dc, mb);

	memory *mem1 = new memory(256 * 1024 * 1024, true);
	mb -> register_memory(0x08000000, mem1 -> get_size(), mem1);
	mb -> register_memory(0xffffffff88000000, mem1 -> get_size(), mem1); // KSEG0
	mb -> register_memory(0xffffffffa8000000, mem1 -> get_size(), mem1); // KSEG1
	mb -> register_memory(0, 512 * 1024, mem1); // needed for exception vectors
	memory *mem2 = new memory(256 * 1024 * 1024, true);
	mb -> register_memory(0x20000000, mem2 -> get_size(), mem2);

	rom *m_prom = new rom("ip24prom.070-9101-007.bin");
	mb -> register_memory(0xffffffff1fc00000, m_prom -> get_size(), m_prom);
	mb -> register_memory(0xffffffff9fc00000, m_prom -> get_size(), m_prom); // KSEG0
	mb -> register_memory(0xffffffffbfc00000, m_prom -> get_size(), m_prom); // KSEG1

	memory *pmc = new mc(p, dc);
	mb -> register_memory(0xffffffff1fa00000, pmc -> get_size(), pmc);
	mb -> register_memory(0xffffffff9fa00000, pmc -> get_size(), pmc); // KSEG0
	mb -> register_memory(0xffffffffbfa00000, pmc -> get_size(), pmc); // KSEG1

	memory *hpc = new hpc3(dc, "sram.dat");
	mb -> register_memory(0xffffffff1fb00000, hpc -> get_size(), hpc);
	mb -> register_memory(0xffffffff9fb00000, hpc -> get_size(), hpc); // KSEG0
	mb -> register_memory(0xffffffffbfb00000, hpc -> get_size(), hpc); // KSEG1

#if _PROFILING == 1 || _PROFILING == 2
	double start_ts = get_ts();
	int cnt = 0;
	for(;!sig_terminate;)
	{
		p -> reset();

		for(int nr=0; nr<600; nr++)
		{
			dc -> tick(p);
			p -> tick();
		}

		cnt++;
		if (cnt % 100 == 0)
			printf("%d\r", cnt);
	}
	double dcnt = double(cnt) * 600.0;
	printf("i/s: %f\n", dcnt / (get_ts() - start_ts));
#elif _PROFILING == 3 || _PROFILING == 4
        double start_ts = get_ts();
        int cnt = 0;
	for(;!sig_terminate;)
	{
		p -> tick();

		cnt++;
	}
        printf("i/s: %f\n", double(cnt) / (get_ts() - start_ts));
#else
	if (single_step || debug)
	{
		for(;!sig_terminate;)
		{
			dc -> tick(p);
			p -> tick();

			if (single_step)
				getch();
		}
	}
	else
	{
		for(;!sig_terminate;)
			p -> tick();
	}
#endif

	delete dc;

	delete p;
	delete mb;
	delete pmc;
	delete mem1;
	delete mem2;
	delete m_prom;
	delete hpc;

	dolog("--- END ---");

	return 0;
}
