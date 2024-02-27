CC:=gcc
CFLAGS:=-c -Wall -Wextra -Werror -std=c11 -pedantic
LDFLAGS:=-lm
CGTKFLAGS:=`pkg-config --cflags gtk4`
LDGTKFLAGS:=`pkg-config --libs gtk4`
GDBFLAGS:=#-g
SANFLAGS:=#-fsanitize=address -fsanitize=leak
GCFLAGS:=-fprofile-arcs -ftest-coverage
GLFLAGS:=-lgcov --coverage
CHECKFLAGS:=$(shell pkg-config --cflags --libs check)
SOURCES:=stack.c dekstra.c calc.c credit_calc.c deposit_calc.c
TSOURCES:=tests.c
OBJECTS:=$(SOURCES:.c=.o)
TOBJECTS:=$(TSOURCES:.c=.o)
EXECUTABLE:=tests

all: clean install tests

clean: uninstall
	rm -rf $(EXECUTABLE) *.o *.gcov *.gcda *.gcno *.info ./gcov_report tests

install: uninstall
	mkdir ./bin ./bin/logfiles
	$(CC) $(CGTKFLAGS) graph.c $(SOURCES) -o ./bin/graph $(LDGTKFLAGS) $(SANFLAGS)

uninstall:
	rm -rf ./bin

dvi:

dist:

tests:
	$(CC) $(CFLAGS) $(GCFLAGS) $(SOURCES) $(TSOURCES) $(GDBFLAGS)
	$(CC) $(OBJECTS) $(TOBJECTS) $(CHECKFLAGS) -o tests $(GLFLAGS) $(SANFLAGS) $(LDFLAGS)
	./tests

$(OBJECTS):
	$(CC) $(CFLAGS) $(SOURCES) $(SANFLAGS) $(GDBFLAGS)

$(TOBJECTS):
	$(CC) $(CFLAGS) $(TSOURCES) $(SANFLAGS) $(GDBFLAGS)

gcov_report:
	gcov $(SOURCES) $(TSOURCES)
	lcov --capture --directory ./ --output-file coverage.info
	genhtml coverage.info --output-directory ./gcov_report

rebuild: clean all

lintc:
	clang-format -n *.c

linth:
	clang-format -n *.h

leaks:
	leaks -atExit -- ./tests
