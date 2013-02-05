#include <stdarg.h>
#include <stdio.h>

#include "processor.h"
#include "debug_console_simple.h"

debug_console_simple::debug_console_simple()
{
}

debug_console_simple::~debug_console_simple()
{
}

void debug_console_simple::init()
{
}

void debug_console_simple::tick(processor *p)
{
}

void debug_console_simple::log(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}
