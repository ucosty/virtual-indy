#! /bin/sh

# SVN: $Revision: 653 $

rm -f bla.txt

valgrind --show-reachable=yes --leak-check=full --read-var-info=yes --track-origins=yes --malloc-fill=93 --free-fill=b9 --error-limit=no ./miep -d -l bla.txt 2> err8.log
