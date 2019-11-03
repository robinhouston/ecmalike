all: parse randomprime

.PHONY: all

parse: main_parse.c parse.c prettyprint.c symbol_table.c

randomprime: main_randomprime.c randomprime.c forisek_prime.c numbers.c
