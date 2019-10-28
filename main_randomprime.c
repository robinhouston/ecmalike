#include <stdio.h>
#include <stdlib.h>

#include "randomprime.h"

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("%llu\n", random_odd_prime());
    }
    else {
        // For performance testing
        int n = atoi(argv[1]);
        uint64_t r = 0;
        for (int i = 0; i < n; ++i)
        {
            r += random_odd_prime();
            // printf("%llu\n", random_odd_prime());
        }
        printf("%llu\n", r);
    }
}
