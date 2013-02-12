#include "memory.h"

class eprom : public memory
{
private:
	int fd;

public:
	eprom(std::string file);
	eprom(std::string file, uint64_t size);

	virtual ~eprom();
};
