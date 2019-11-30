#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include "alg.h"
#include "parse.h"
#include "score.h"


extern struct alg *parseFile(FILE *f);

int main(int argc, char **argv) {
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "Usage: %s filename.alg [n_bits]\n", argv[0]);
        return EX_USAGE;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror(argv[0]);
        return EX_NOINPUT;
    }

    uint64_t mask = -1ull;
    if (argc == 3) {
        int n_bits = atoi(argv[2]);
        for (int i = 0; i < 64 - n_bits; ++i) mask = mask >> 1;
    }

    struct alg *alg = parseFile(f);
    if (!alg) {
        fprintf(stderr, "%s: Failed to parse file: %s\n", argv[0], argv[1]);
        return EX_DATAERR;
    }

    printf("Score: %f\n", multiscore(alg, mask));
}
