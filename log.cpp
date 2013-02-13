#include <stdarg.h>
#include <stdio.h>

#include "error.h"

extern const char *logfile;

void dolog(const char *fmt, ...)
{
	if (logfile)
	{
		FILE *fh = fopen(logfile, "a+");
		if (!fh)
			error_exit("error accessing logfile");

		va_list ap;

		va_start(ap, fmt);
		(void)vfprintf(fh, fmt, ap);
		va_end(ap);

		fprintf(fh, "\n");

		fclose(fh);
	}
}
