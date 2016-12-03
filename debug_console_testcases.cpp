#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "debug_console_testcases.h"
#include "processor.h"
#include "processor_utils.h"
#include "utils.h"
#include "easylogging++.h"

extern const char *logfile;

debug_console_testcases::debug_console_testcases()
{
}

debug_console_testcases::~debug_console_testcases()
{
}

void debug_console_testcases::tick(processor *p)
{
	uint64_t PC = p -> get_PC();

	uint32_t instruction = -1;
	(void)p -> get_mem_32b(PC, &instruction);

	std::string logline = p -> da_logline(instruction);
	LOG(INFO) << logline;

}

void debug_console_testcases::dc_log(const char *fmt, ...)
{
	if (logfile)
	{
		char buffer[4096];
		va_list ap;

		va_start(ap, fmt);
		(void)vsnprintf(buffer, sizeof buffer, fmt, ap);
		va_end(ap);

		LOG(INFO) << buffer;
	}
}

void debug_console_testcases::dc_term(const char *fmt, ...)
{
}
