#include <signal.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "debug_console.h"

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

debug_console::debug_console(processor *p_in) : p(p_in)
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
}

debug_console::~debug_console()
{
	delwin(win);

	endwin();
}

void debug_console::update_terminal()
{
	wmove(win, 0, 0);
	wnoutrefresh(win);
	doupdate();
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
	if (win)
		delwin(win);

	win = newwin(80, 25, 0, 0);
}

void debug_console::tick()
{
	if (term_change)
		recreate_terminal();

	// FIXME
}
