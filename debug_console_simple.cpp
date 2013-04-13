#include <stdarg.h>
#include <stdio.h>

#include "processor.h"
#include "debug_console_simple.h"
#include "utils.h"
#include "log.h"

debug_console_simple::debug_console_simple()
{
}

debug_console_simple::~debug_console_simple()
{
	printf("cycles/sec: %f\n", double(n_ticks) / (get_ts() - start_ts));
}

void debug_console_simple::init()
{
}

void debug_console_simple::tick(processor *p)
{
	n_ticks++;
}

void debug_console_simple::dc_log(const char *fmt, ...)
{
#ifdef _DEBUG
	char buffer[4096];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buffer, sizeof buffer, fmt, ap);
	va_end(ap);

	dolog("%s", buffer);
#endif
}

void debug_console_simple::dc_term(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}
