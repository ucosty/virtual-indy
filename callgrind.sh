#! /bin/sh

valgrind --tool=callgrind --dump-instr=yes --collect-jumps=yes --collect-systime=yes --collect-bus=yes --cache-sim=yes --branch-sim=yes ./miep
