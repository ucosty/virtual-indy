#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

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
