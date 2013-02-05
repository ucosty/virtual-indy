#include <stdarg.h>
#include <stdio.h>

#include "error.h"

void dolog(const char *fmt, ...)
{
	va_list ap;

	FILE *fh = fopen("bla.txt", "a+");
	if (!fh)
		error_exit("error accessing logfile");

	va_start(ap, fmt);
	(void)vfprintf(fh, fmt, ap);
	va_end(ap);

	fprintf(fh, "\n");

	fclose(fh);
}
