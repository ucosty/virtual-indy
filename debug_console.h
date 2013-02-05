#include <ncurses.h>

#include "processor.h"

typedef enum { C_WHITE = 0, C_GREEN, C_YELLOW, C_BLUE, C_MAGENTA, C_CYAN, C_RED } dc_color_t;

class debug_console
{
private:
	processor *p;
	WINDOW *win;
	int max_x, max_y;

	void update_terminal();
	void recreate_terminal();
	void create_windows();

public:
	debug_console(processor *p_in);
	~debug_console();

	void tick();
};
