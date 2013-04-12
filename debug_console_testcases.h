#ifndef __DEBUG_TESTCASES__H__
#define __DEBUG_TESTCASES__H__

#include "debug_console.h"

class processor;

class debug_console_testcases : public debug_console
{
public:
	debug_console_testcases();
	virtual ~debug_console_testcases();

	void tick(processor *p);

	void dc_log(const char *fmt, ...);

	void dc_term(const char *fmt, ...);
};

#endif
