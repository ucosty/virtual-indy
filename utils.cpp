#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include "error.h"

void load_file(const char *filename, unsigned char **data, int *len)
{
        FILE *fh = fopen(filename, "r");
        if (!fh)
                error_exit("Problem opening file %s", filename);
        int fd = fileno(fh);

        struct stat st;
        if (fstat(fd, &st) == -1)
                error_exit("Stat on file %s failed", filename);

	*len = st.st_size;
        *data = (unsigned char *)malloc(*len + 1);

        if (fread(*data, 1, *len, fh) != size_t(*len))
                error_exit("Short read on %s", filename);

        fclose(fh);

        (*data)[*len] = 0x00;
}

std::string format(const char *fmt, ...)
{
	char *buffer = NULL;
        va_list ap;

        va_start(ap, fmt);
        (void)vasprintf(&buffer, fmt, ap);
        va_end(ap);

	std::string result = buffer;
	free(buffer);

	return result;
}

double get_ts()
{
        struct timeval ts;

        if (gettimeofday(&ts, NULL) == -1)
                error_exit("gettimeofday failed");

        return double(ts.tv_sec) + double(ts.tv_usec) / 1000000.0;
}
