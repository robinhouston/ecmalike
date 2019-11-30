all: parse score

.PHONY: all

CPPFLAGS = --std=c99

parse: main_parse.o parse.o prettyprint.o symbol_table.o

randomprime: main_randomprime.c randomprime.o forisek_prime.o numbers.o

score: main_score.o parse.o symbol_table.o score.o randomprime.o runtime.o forisek_prime.o numbers.o

smooth: main_smooth.o forisek_prime.o numbers.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

runalg: main_runalg.o parse.o symbol_table.o forisek_prime.o numbers.o runtime.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^
