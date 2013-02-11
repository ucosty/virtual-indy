#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "debug_console.h"
#include "processor.h"
#include "processor_utils.h"
#include "log.h"
#include "utils.h"

#define SCREEN_REFRESHES_PER_SECOND	3

extern bool single_step;
extern const char *logfile;

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
	nc = false;
	win_regs = win_logs = NULL;
	max_x = max_y = -1;
	n_ticks = 0;
	start_ts = get_ts();
	had_logging = false;

	refresh_limit = refresh_counter = 0;
	refresh_limit_valid = false;

	// make sure the cpu runs at 100% before measuring the
	// number of emulated instructions per second (used for
	// the screen refresh)
#ifndef _PROFILE
	while(get_ts() - start_ts < 0.1)
	{
	}
#endif
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
	log("");
	log("instruction usage counts");
	log("------------------------");
	std::map<std::string, long int>::iterator it = instruction_counts.begin();

	while(it != instruction_counts.end())
	{
		log("%s\t%ld", it -> first.c_str(), it -> second);

		it++;
	}

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
	n_ticks++;

	if (!refresh_limit_valid)
	{
		double now_ts = get_ts();

		refresh_limit++;

		if (now_ts - start_ts >= 1.0 / double(SCREEN_REFRESHES_PER_SECOND))
			refresh_limit_valid = true;
	}

	if (++refresh_counter >= refresh_limit || single_step)
	{
		double now_ts = get_ts();

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
				x = 24;
				y = registers - 16;
			}

			mvwprintw(win_regs, y, x, "%s %016llx", processor::reg_to_name(registers), p -> get_register_64b_unsigned(registers));
		}

		uint64_t PC = p -> is_delay_slot() ? p -> get_delay_slot_PC() : p -> get_PC();
		mvwprintw(win_regs, 0, 48, "PC: %016llx %c", PC, p -> is_delay_slot() ? 'D' : '.');
		mvwprintw(win_regs, 1, 48, "LO: %08x", p -> get_LO());
		mvwprintw(win_regs, 2, 48, "HI: %08x", p -> get_HI());
		mvwprintw(win_regs, 3, 48, "SR: %08x", p -> get_SR());

		uint32_t instruction = -1;
		bool r_ok = p -> get_mem_32b(PC, &instruction);
		mvwprintw(win_regs, 4, 48, "mem: %d/%08x", r_ok, instruction);

		int opcode = (instruction >> 26) & MASK_6B;
		int function = instruction & MASK_6B;
		int sa = (instruction >> 6) & MASK_5B;
		int rd = (instruction >> 11) & MASK_5B;
		int rt = (instruction >> 16) & MASK_5B;
		int rs = (instruction >> 21) & MASK_5B;
		int immediate = instruction & MASK_16B;
		int b18_signed_offset = untwos_complement(immediate << 2, 18);
		mvwprintw(win_regs,  5, 48, "op: %02x", opcode);
		mvwprintw(win_regs,  6, 48, "rs: %02x", rs);
		mvwprintw(win_regs,  7, 48, "rt: %02x", rt);
		mvwprintw(win_regs,  8, 48, "rd: %02x", rd);
		mvwprintw(win_regs,  9, 48, "sa: %02x", sa);
		mvwprintw(win_regs, 10, 48, "fu: %02x", function);
		mvwprintw(win_regs, 11, 48, "im: %04x", immediate);
		mvwprintw(win_regs, 12, 48, "of: %d", b18_signed_offset);

		std::string decoded = p -> decode_to_text(instruction);
		mvwprintw(win_regs, 13, 48, "  :                       ");
		mvwprintw(win_regs, 13, 48, "  : %s", decoded.c_str());

		unsigned int space = decoded.find(' ');
		if (space == std::string::npos)
			space = decoded.length();

		std::string instruction_name = decoded.substr(0, space);

		std::map<std::string, long int>::iterator found = instruction_counts.find(instruction_name);

		if (found != instruction_counts.end())
			found -> second++;
		else
			instruction_counts.insert(std::pair<std::string, long int>(instruction_name, 1));

		double t_diff = now_ts - start_ts;
		if (t_diff)
			mvwprintw(win_regs, 14, 48, "I/S: %f", double(n_ticks) / t_diff);
		mvwprintw(win_regs, 15, 48, "cnt: %lld", n_ticks);

		std::string logline = p -> da_logline(instruction);
		dolog(logline.c_str());

		if (had_logging)
		{
			wnoutrefresh(win_logs);
			had_logging = false;
		}

		wnoutrefresh(win_regs);
		doupdate();

		refresh_counter = 0;
	}
}

void debug_console::log(const char *fmt, ...)
{
	if (logfile)
	{
		char buffer[4096];
		va_list ap;

		va_start(ap, fmt);
		(void)vsnprintf(buffer, sizeof buffer, fmt, ap);
		va_end(ap);

		wprintw(win_logs, "%s\n", buffer);

		dolog("%s", buffer);

		had_logging = true;
	}
}
