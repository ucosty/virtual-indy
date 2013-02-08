#include <stdarg.h>
#include <stdio.h>

#include "error.h"

extern const char *logfile;

void dolog(const char *fmt, ...)
{
	va_list ap;

	if (logfile)
	{
		FILE *fh = fopen(logfile, "a+");
		if (!fh)
			error_exit("error accessing logfile");

		va_start(ap, fmt);
		(void)vfprintf(fh, fmt, ap);
		va_end(ap);

		fprintf(fh, "\n");

		fclose(fh);
	}
}
