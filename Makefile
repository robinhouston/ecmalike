all: parse

.PHONY: all

parse: main_parse.c parse.c prettyprint.c symbol_table.c
