#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "debug_console.h"
#include "processor.h"

bool term_change = false;

void sigh(int sig)
{
	// will always be SIGWINCH
	signal(SIGWINCH, sigh);

	term_change = true;
}

void determine_terminal_size(int *max_y, int *max_x)
{
        struct winsize size;

        *max_x = *max_y = 0;

        if (ioctl(1, TIOCGWINSZ, &size) == 0)
        {
                *max_y = size.ws_row;
                *max_x = size.ws_col;
        }

        if (!*max_x || !*max_y)
        {
                char *dummy = getenv("COLUMNS");
                if (dummy)
                        *max_x = atoi(dummy);
                else
                        *max_x = 80;

                dummy = getenv("LINES");
                if (dummy)
                        *max_x = atoi(dummy);
                else
                        *max_x = 24;
        }
}

debug_console::debug_console()
{
}

void debug_console::init()
{
        initscr();
	start_color();
        keypad(stdscr, TRUE);
        intrflush(stdscr, FALSE);
        noecho();
        nonl();
        refresh();
        nodelay(stdscr, FALSE);
        meta(stdscr, TRUE);     /* enable 8-bit input */
        idlok(stdscr, TRUE);    /* may give a little clunky screenredraw */
        idcok(stdscr, TRUE);    /* may give a little clunky screenredraw */
        leaveok(stdscr, FALSE);

	init_pair(C_WHITE, COLOR_WHITE, COLOR_BLACK);
	init_pair(C_CYAN, COLOR_CYAN, COLOR_BLACK);
	init_pair(C_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(C_BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(C_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	init_pair(C_GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(C_RED, COLOR_RED, COLOR_BLACK);

	recreate_terminal();

	nc = true;
}

debug_console::~debug_console()
{
	if (nc)
	{
		delwin(win_regs);
		delwin(win_logs);

		endwin();
	}
}

void debug_console::recreate_terminal()
{
	term_change = false;

	determine_terminal_size(&max_y, &max_x);

	resizeterm(max_y, max_x);

        endwin();
        refresh(); /* <- as specified by ncurses faq, was: doupdate(); */

	create_windows();
}

void debug_console::create_windows()
{
	if (win_regs)
		delwin(win_regs);

	if (win_logs)
		delwin(win_logs);

	win_regs = newwin(16, 80,  0, 0);
	scrollok(win_regs, false);

	win_logs = newwin( 9, 80, 16, 0);
	scrollok(win_logs, true);
}

void debug_console::tick(processor *p)
{
	if (term_change)
		recreate_terminal();

	int x = -1, y = -1;
	for(int registers=0; registers<32; registers++)
	{
		if (registers < 16)
		{
			x = 0;
			y = registers;
		}
		else
		{
			x = 22;
			y = registers - 16;
		}

		mvwprintw(win_regs, y, x, "R%02d %08x", registers, p -> get_register(registers));
	}

	mvwprintw(win_regs, 0, 44, "PC: %08x", p -> get_PC());
	mvwprintw(win_regs, 1, 44, "LO: %08x", p -> get_LO());
	mvwprintw(win_regs, 2, 44, "HI: %08x", p -> get_HI());
	mvwprintw(win_regs, 3, 44, "SR: %08x", p -> get_SR());

	wnoutrefresh(win_regs);
	doupdate();
}

void debug_console::log(const char *fmt, ...)
{
	char buffer[4096];
	va_list ap;

	va_start(ap, fmt);
	(void)vsnprintf(buffer, sizeof buffer, fmt, ap);
	va_end(ap);

	wprintw(win_logs, "%s\n", buffer);

	wnoutrefresh(win_logs);
	doupdate();
}
