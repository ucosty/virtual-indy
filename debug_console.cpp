#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "easylogging++.h"
#include "debug_console.h"
#include "processor.h"
#include "processor_utils.h"
#include "exceptions.h"
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
	win_term = win_regs = win_logs = NULL;
	max_x = max_y = -1;
	n_ticks = 0;
	start_ts = get_ts();
	had_logging = false;

	refresh_limit = refresh_counter = 0;
	refresh_limit_valid = false;
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

	LOG(INFO) << "Calibrating...";
	wnoutrefresh(win_logs);
	doupdate();

	nc = true;
}

debug_console::~debug_console()
{
#ifdef _DEBUG
	LOG(INFO) << "instruction usage counts";
	LOG(INFO) << "------------------------";
	std::map<std::string, long int>::iterator it = instruction_counts.begin();

	while(it != instruction_counts.end())
	{
		LOG(INFO) << it->first << "\t" << it->second;
		it++;
	}
#endif

	if (nc)
	{
		delwin(win_regs);
		delwin(win_logs);
		delwin(win_term);

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

	if (win_term)
		delwin(win_term);

	win_regs = newwin(16, 80,  0, 0);
	scrollok(win_regs, false);

	win_logs = NULL;
	if (max_x > 81)
	{
		win_logs = newwin(16, max_x - 81, 0, 81);
		scrollok(win_logs, true);
	}

        win_term = newwin(max_y - 16, 80, 16, 0); 
        scrollok(win_term, true);
}

void debug_console::tick(processor *p)
{
	n_ticks++;

	if (!refresh_limit_valid)
	{
		double now_ts = get_ts();
		double t_diff = now_ts - start_ts;

		// first second: ignore as the cpu might need to come out of
		// reduced clock frequency (linux: "ondemand" scaling governor)
		if (t_diff >= 1.0)
			refresh_limit++;

		if (t_diff >= 1.0 + 1.0 / double(SCREEN_REFRESHES_PER_SECOND))
		{
			dc_log("Refresh screen every %d instructions", refresh_limit);
			refresh_limit_valid = true;
		}
	}

	uint64_t PC = p -> is_delay_slot() ? p -> get_delay_slot_PC() : p -> get_PC();

	uint32_t instruction = -1;
	bool r_ok = true;
	try
	{
		p -> get_mem_32b(PC, &instruction);
	}
	catch(processor_exception & pe)
	{
		dc_log("EXCEPTION %d at/for %016llx, PC: %016llx (3), sr: %08x", pe.get_cause(), pe.get_BadVAddr(), pe.get_EPC(), pe.get_status());

		r_ok = false;
	}

	std::string logline = p -> da_logline(instruction);
	LOG(INFO) << logline;

	std::string decoded = p -> decode_to_text(instruction);

#ifdef _DEBUG
	unsigned int space = decoded.find(' ');
	if (space == std::string::npos)
		space = decoded.length();

	std::string instruction_name = decoded.substr(0, space);

	std::map<std::string, long int>::iterator found = instruction_counts.find(instruction_name);

	if (found != instruction_counts.end())
		found -> second++;
	else
		instruction_counts.insert(std::pair<std::string, long int>(instruction_name, 1));
#endif

	if ((++refresh_counter >= refresh_limit && refresh_limit_valid == true) || single_step)
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
				x = 22;
				y = registers - 16;
			}

			mvwprintw(win_regs, y, x, "%s %016llx", registers == 0 ? "0  " : processor::reg_to_name(registers), p -> get_register_64b_unsigned(registers));
		}

		mvwprintw(win_regs, 0, 44, "PC: %016llx %c", PC, p -> is_delay_slot() ? 'D' : '.');
		mvwprintw(win_regs, 1, 44, "LO: %016llx", p -> get_LO());
		mvwprintw(win_regs, 2, 44, "HI: %016llx", p -> get_HI());
		mvwprintw(win_regs, 3, 44, "SR: %016llx", p -> get_SR());

		mvwprintw(win_regs, 4, 44, "mem: %d/%08x", r_ok, instruction);

		int opcode = (instruction >> 26) & MASK_6B;
		int function = instruction & MASK_6B;
		int sa = processor::get_SA(instruction);
		int rd = processor::get_RD(instruction);
		int rt = processor::get_RT(instruction);
		int rs = processor::get_RS(instruction);
		int immediate = processor::get_immediate(instruction);
		int b18_signed_offset = processor::get_SB18(instruction);
		mvwprintw(win_regs,  5, 44, "op: %02x  rs: %02x", opcode, rs);
		mvwprintw(win_regs,  6, 44, "rt: %02x  rd: %02x", rt, rd);
		mvwprintw(win_regs,  7, 44, "sa: %02x  fu: %02d", sa, function);
		mvwprintw(win_regs,  8, 44, "im: %04x  of: %d", immediate, b18_signed_offset);

		mvwprintw(win_regs,  9, 44, ">                       ");
		mvwprintw(win_regs,  9, 44, "> %s", decoded.c_str());

		double t_diff = now_ts - start_ts;
		if (t_diff)
		{
			double i_per_s = double(n_ticks) / t_diff;

			if (i_per_s >= 1000000000.0)
				mvwprintw(win_regs, 10, 44, "I/S: %6.2fG", i_per_s / 1000000000.0);
			else if (i_per_s >= 1000000.0)
				mvwprintw(win_regs, 10, 44, "I/S: %6.2fM", i_per_s / 1000000.0);
			else if (i_per_s >= 1000.0)
				mvwprintw(win_regs, 10, 44, "I/S: %6.2fk", i_per_s / 1000.0);
			else
				mvwprintw(win_regs, 10, 44, "I/S: %6.2f", i_per_s);
		}

		if (n_ticks >= 1000000000)
			mvwprintw(win_regs, 11, 44, "cnt: %7.2fG", double(n_ticks) / 1000000000.0);
		else if (n_ticks >= 1000000)
			mvwprintw(win_regs, 11, 44, "cnt: %7.2fM", double(n_ticks) / 1000000.0);
		else if (n_ticks >= 1000)
			mvwprintw(win_regs, 11, 44, "cnt: %7.2fk", double(n_ticks) / 1000.0);
		else
			mvwprintw(win_regs, 11, 44, "cnt: %lld", n_ticks);

			mvwprintw(win_regs, 11, 44, "cnt: %lld", n_ticks);

		const memory_bus *pmb = p -> get_memory_bus();
		mvwprintw(win_regs, 12, 44, "I_S: %016x", pmb -> get_cur_segment_i());
		mvwprintw(win_regs, 13, 44, "D_S: %016x", pmb -> get_cur_segment());

		if (had_logging)
		{
			wnoutrefresh(win_logs);
			had_logging = false;
		}

		wnoutrefresh(win_regs);
		doupdate();

		if (refresh_limit_valid)
			refresh_counter = 0;
	}
}

void debug_console::dc_log(const char *fmt, ...)
{
	if (logfile || win_logs)
	{
		char buffer[4096];
		va_list ap;

		va_start(ap, fmt);
		(void)vsnprintf(buffer, sizeof buffer, fmt, ap);
		va_end(ap);

		if (win_logs)
			wprintw(win_logs, "\n%s", buffer);

		LOG(INFO) << buffer;

		had_logging = true;
	}
}

void debug_console::dc_term(const char *fmt, ...)
{
	if (win_term)
	{
		va_list ap;

		va_start(ap, fmt);
		vwprintw(win_term, fmt, ap);
		va_end(ap);

		wnoutrefresh(win_term);
		doupdate();
	}
}
