#include <ncurses.h>

#include "debug_console.h"

class processor;

class debug_console_simple : public debug_console
{
public:
	debug_console_simple();
	~debug_console_simple();

	void init();

	void tick(processor *p);

	void log(const char *fmt, ...);
};
