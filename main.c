#include <stdio.h>
#include <sysexits.h>

#include "alg.h"
#include "parse.h"

extern struct alg *parseFile(FILE *f);

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename.alg\n", argv[0]);
        return EX_USAGE;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror(argv[0]);
        return EX_NOINPUT;
    }

    struct alg *alg = parseFile(f);
    if (!alg) {
        fprintf(stderr, "%s: Failed to parse file: %s\n", argv[0], argv[1]);
        return EX_DATAERR;
    }

    printf("Parsed!\n");
}
