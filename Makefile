VERSION=0.1

DEBUG=-g -D_DEBUG=1 -pg
CXXFLAGS+=-O3 -pedantic -Wall -Wno-unused-variable -DVERSION=\"$(VERSION)\" $(DEBUG)
LDFLAGS=$(DEBUG) -lncurses

OBJS=memory_bus.o memory.o processor.o graphics_lg1.o processor_utils.o error.o utils.o debug_console.o debug_console_simple.o log.o processor_r_type.o processor_i_type.o processor_COP0.o processor_j_type.o processor_special2.o processor_regimm.o processor_special3.o processor_disassembler.o
OBJStest=testcases.o debug_console_testcases.o
OBJSmain=main.o

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
