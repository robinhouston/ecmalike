#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include "forisek_prime.h"

int main (int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s bound\n", argv[0]);
        return EX_USAGE;
    }

    unsigned long smoothness_bound = strtoul(argv[1], NULL, 10);
    uint64_t product = 1;

    for (unsigned long i = 2; i <= smoothness_bound; ++i) {
        if (!is_prime(i)) continue;

        uint64_t factor = 1;
        while (factor * i < smoothness_bound) {
            factor *= i;
            if (product * i < product) {
                printf("0x%llx, ", product);
                product = 1;
            }
            product *= i;
        }
    }
    printf("0x%llx\n", product);
}
