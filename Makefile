VERSION=0.1

# 1: mode for "valgrind --tool=cachegrind ./miep"
#    no logging, tight loop that runs 600 instructions and then resets the processor
#    disables DEBUG flag
# 2: mode for "gprof ./miep"
#    standard with -pg flag added
# 3: like 1 but normal run (not 600 instructions)
# 4: like 2 but normal run (not 600 instructions)
PROFILING=0
# yes/no, will be set to no in profiling mode
# enables instruction-usage counting
# enables logging in non '-d'-mode
DEBUG=yes

DEBUG_FLAGS=-g3
ifeq ($(PROFILING),1)
	DEBUG_FLAGS+=-D_PROFILING=1
	DEBUG=no
endif
ifeq ($(PROFILING),2)
	DEBUG_FLAGS+=-D_PROFILING=2 -pg
	DEBUG=no
endif
ifeq ($(PROFILING),3)
	DEBUG_FLAGS+=-D_PROFILING=3
	DEBUG=no
endif
ifeq ($(PROFILING),4)
	DEBUG_FLAGS+=-D_PROFILING=4 -pg
	DEBUG=no
endif
ifeq ($(DEBUG),yes)
	DEBUG_FLAGS+=-D_DEBUG=1
endif

CXXFLAGS+=-O3 -pedantic -Wall -Wno-unused-variable -Wno-long-long -std=c++11 -DVERSION=\"$(VERSION)\" $(DEBUG_FLAGS)
LDFLAGS=$(DEBUG_FLAGS) -lncurses -pthread

OBJS=memory_bus.o memory.o processor.o graphics_lg1.o processor_utils.o utils.o debug_console.o debug_console_simple.o processor_r_type.o processor_i_type.o processor_COP0.o processor_j_type.o processor_special2.o processor_regimm.o processor_special3.o rom.o eprom.o hpc3.o mc.o exceptions.o processor_disassembler.o z85c30.o seeq_8003_8020.o processor_COP1.o
OBJStest=testcases.o debug_console_testcases.o
OBJSmain=error.o main.o

all: testcases miep

testcases: $(OBJS) $(OBJStest)
	$(CXX) -Wall -W $(OBJS) $(OBJStest) $(LDFLAGS) -o testcases

miep: $(OBJS) $(OBJSmain)
	$(CXX) -Wall -W $(OBJS) $(OBJSmain) $(LDFLAGS) -o miep

install: miep
	cp miep $(DESTDIR)/usr/local/bin

uninstall: clean
	rm -f $(DESTDIR)/usr/local/bin/miep

clean:
	rm -f $(OBJS) $(OBJSmain) miep $(OBJStest) testcases core gmon.out

package: clean
	# source package
	rm -rf miep-$(VERSION)
	mkdir miep-$(VERSION)
	cp *.cpp *.h Makefile readme.txt license.txt miep-$(VERSION)
	tar czf miep-$(VERSION).tgz miep-$(VERSION)
	rm -rf miep-$(VERSION)

check:
	cppcheck -v --enable=all --std=c++11 --inconclusive -I. . 2> err.txt
