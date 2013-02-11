#ifndef __DEBUG_CONSOLE__H__
#define __DEBUG_CONSOLE__H__

#include <map>
#include <string>

#include <ncurses.h>

class processor;

typedef enum { C_WHITE = 0, C_GREEN, C_YELLOW, C_BLUE, C_MAGENTA, C_CYAN, C_RED } dc_color_t;

class debug_console
{
private:
	WINDOW *win_regs, *win_logs;
	int max_x, max_y;
	bool nc;
	double start_ts;
	long long int n_ticks;
	int refresh_counter, refresh_limit;
	bool refresh_limit_valid;
	bool had_logging;
	std::map<std::string, long int> instruction_counts;

	void recreate_terminal();
	void create_windows();

public:
	debug_console();
	virtual ~debug_console();

	virtual void init();

	virtual void tick(processor *p);

	virtual void log(const char *fmt, ...);
};

#endif
