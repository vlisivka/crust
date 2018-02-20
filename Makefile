CC=gcc
#CC=clang

CFLAGS += -std=c99
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -pedantic
CFLAGS += -Werror

VFLAGS += --quiet
VFLAGS += --tool=memcheck
VFLAGS += --leak-check=full
VFLAGS += --error-exitcode=1

all: memcheck

test: tests.out
	./tests.out

lddebug: tests.out
	LD_DEBUG=files ./tests.out

catchsegv: tests.out
	catchsegv ./tests.out

memcheck: tests.out
	valgrind $(VFLAGS) ./tests.out

clean:
	rm -rf *.o tests.out tests.debug tests.cover tests.asan *.gcda *.gcno *.gcov test-expanded.c vgcore.*

tests.out: *.c *.h
	$(CC) $(CFLAGS) -Os *.c -o tests.out


debug: tests.debug
	gdb ./tests.debug

tests.debug: *.c *.h
	$(CC) $(CFLAGS) -ggdb -O0 *.c -o tests.debug

asan: tests.asan
	./tests.asan

tests.asan:
	$(CC) $(CFLAGS) -ggdb -O3 -Fsanitize=address -lasan *.c -o tests.asan

cover: tests.cover
	./tests.cover
	for i in *gcda; do gcov $$i; done
	rm -rf tests.cover *.gcda *.gcno

tests.cover: *.c *.h
	$(CC) -E test.c | fgrep -v '#' | astyle > test-expanded.c
	$(CC) $(CFLAGS) -ggdb -O0 -fprofile-arcs -ftest-coverage -fno-inline test-expanded.c $(ls *.c | grep -v test) -o tests.cover
