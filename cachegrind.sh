#! /bin/sh

valgrind --tool=cachegrind --branch-sim=yes ./miep
