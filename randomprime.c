/* Generate random 64-bit prime numbers */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "forisek_prime.h"

uint64_t random_odd_prime(uint64_t mask) {
    if (mask == 0) mask = -1ull;

    uint64_t n;
    for(;;) {
        // Generate a random odd 64-bit number
        arc4random_buf(&n, 8);
        n |= 1;

        n = n & mask;

        if (is_prime(n)) return n;
    }
}
