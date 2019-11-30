#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include "alg.h"
#include "forisek_prime.h"
#include "parse.h"
#include "runtime.h"

extern struct alg *parseFile(FILE *f);

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s filename.alg p k\n", argv[0]);
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

    uint64_t p = strtoull(argv[2], NULL, 10);
    uint64_t k = strtoull(argv[3], NULL, 10);

    if (!is_prime(p)) {
        fprintf(stderr, "%s: the parameter p (%llu) is not prime\n", argv[0], p);
        return EX_USAGE;
    }

    struct runtime *runtime = newRuntime(alg, p);
    runtime_power(runtime, k);
    uint64_t value = runtime_value(runtime);
    destroyRuntime(runtime);

    printf("%llu\n", value);
}
