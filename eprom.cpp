#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "error.h"
#include "eprom.h"
#include "exceptions.h"
#include "utils.h"

eprom::eprom(std::string file)
{
	fd = open(file.c_str(), O_RDWR);
	if (fd == -1)
		error_exit("cannot open file %s", file.c_str());

	struct stat st;
	if (fstat(fd, &st) == -1)
		error_exit("cannot stat on file %s", file.c_str());

	if (st.st_size == 0)
		error_exit("file %s is truncted (0 bytes in size)", file.c_str());

	len = st.st_size;

	pm = (unsigned char *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (!pm)
		error_exit("failed to create mmap on %s", file.c_str());
}

eprom::eprom(std::string file, uint64_t size)
{
	fd = open(file.c_str(), O_RDWR);
	if (fd == -1)
		error_exit("cannot open file %s", file.c_str());

	if (ftruncate(fd, size) == -1)
		error_exit("cannot ftruncate(%lld) on file %s", file.c_str(), size);

	len = size;

	pm = (unsigned char *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (!pm)
		error_exit("failed to create mmap on %s", file.c_str());
}

eprom::~eprom()
{
	if (munmap(pm, len) == -1)
		error_exit("munmap om eprom failed");

	close(fd);
}
