VERSION=0.1

DEBUG=-g # -pg
CXXFLAGS+=-O3 -pedantic -Wall -DVERSION=\"$(VERSION)\" $(DEBUG)
LDFLAGS=$(DEBUG)

OBJS=memory_bus.o memory.o processor.o

all: miep

miep: $(OBJS)
	$(CXX) -Wall -W $(OBJS) $(LDFLAGS) -o miep

install: miep
	cp miep $(DESTDIR)/usr/local/bin

uninstall: clean
	rm -f $(DESTDIR)/usr/local/bin/miep

clean:
	rm -f $(OBJS) miep core

package: clean
	# source package
	rm -rf miep-$(VERSION)
	mkdir miep-$(VERSION)
	cp *.cpp *.h Makefile readme.txt license.txt miep-$(VERSION)
	tar czf miep-$(VERSION).tgz miep-$(VERSION)
	rm -rf miep-$(VERSION)

check:
	cppcheck -v --enable=all --std=c++11 --inconclusive -I. . 2> err.txt
