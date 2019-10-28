/* Generate random 64-bit prime numbers */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Minimal known basis for Miller-Rabin to be deterministic in the desired range:
// see http://miller-rabin.appspot.com/
static uint64_t BASIS[] = {
    2,325,9375,28178,450775,9780504,1795265022,
    0
};

typedef unsigned __int128 uint128_t;

static uint64_t powmod(uint64_t a, uint64_t b, uint64_t n)
{
    uint128_t power = a;
    uint128_t result = 1;

    while (b) {
        if (b & 1) {
            result = (result * power) % n;
        }
        power = (power * power) % n;
        b >>= 1;
    }

    return result;
}

static bool miller_rabin_composite(uint64_t n, uint64_t s, uint64_t d, uint64_t a) {
    uint128_t x = powmod(a, d, n);
    uint64_t y;
 
    while (s) {
        y = (x * x) % n;
        if (y == 1 && x != 1 && x != n-1) return true;
        x = y;
        --s;
    }

    return y != 1;
}


bool isprime(uint64_t n) {
    if (n == 1) return false;
    if (n == 3) return true;

    uint64_t d = n / 2;
    uint64_t s = 1;
    while (!(d & 1)) {
        d /= 2;
        ++s;
    }

    for (uint64_t *p = &BASIS[0]; *p; ++p) {
        if (miller_rabin_composite(n, s, d, *p)) return false;
    }

    return true;
}

uint64_t random_odd_prime() {
    uint64_t n;
    for(;;) {
        // Generate a random odd number
        arc4random_buf(&n, 8);
        n |= 1;

        if (isprime(n)) return n;
    }
}
