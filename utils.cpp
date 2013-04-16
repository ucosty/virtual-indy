#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include "error.h"

void load_file(std::string filename_in, unsigned char **data, uint64_t *len)
{
	const char *filename = filename_in.c_str();
        FILE *fh = fopen(filename, "r");
        if (!fh)
                error_exit("Problem opening file %s", filename);
        int fd = fileno(fh);

        struct stat st;
        if (fstat(fd, &st) == -1)
                error_exit("Stat on file %s failed", filename);

	*len = st.st_size;
        *data = new unsigned char[*len + 1];

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

std::string to_bin_str(uint64_t value, uint8_t bits)
{
	char buffer[65] = { 0 };
	int out = 0;

	for(int bit=bits-1; bit>=0; bit--)
	{
		if (value & (1 << bit))
			buffer[out++] = '1';
		else
			buffer[out++] = '0';
	}

	return buffer;
}
