#include <string>

std::string format(const char *fmt, ...);
void load_file(std::string filename, unsigned char **data, uint64_t *len);
double get_ts();
