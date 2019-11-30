all: parse score

.PHONY: all

parse: main_parse.c parse.c prettyprint.c symbol_table.c

randomprime: main_randomprime.c randomprime.o forisek_prime.o numbers.o

score: main_score.o parse.o symbol_table.o score.o randomprime.o runtime.o forisek_prime.o numbers.o

smooth: main_smooth.o forisek_prime.o numbers.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

