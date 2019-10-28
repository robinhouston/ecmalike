/* Generate random 64-bit prime numbers */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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

uint64_t gcd(uint64_t a, uint64_t b) {
    uint64_t t;
    while (b) {
        t = b;
        b = a % b;
        a = t;
    }
    return a;
}

bool isprime(uint64_t n) {
    if (n < 103) {
        switch (n) {
            case 2: case 3: case 5: case 7: case 11: case 13: case 17: case 19: case 23:
            case 29: case 31: case 37: case 41: case 43: case 47: case 53: case 59:
            case 61: case 67: case 71: case 73: case 79: case 83: case 89: case 97:
            case 101: return true;
            default: return false;
        }
    }

    // 16294579238595022365LLU == 3*5*7*11*13*17*19*23*29*31*37*41*43*47*53
    if (gcd(n, 16294579238595022365LLU) != 1) return false;

    // 16294579238595022365LLU == 59*61*67*71*73*79*83*89*97*101
    if (gcd(n, 7145393598349078859LLU) != 1) return false;

    uint64_t d = n / 2;
    uint64_t s = 1;
    while (!(d & 1)) {
        d /= 2;
        ++s;
    }

    // Minimal known basis for Miller-Rabin to be deterministic in the desired range:
    // see http://miller-rabin.appspot.com/
    return !(
        miller_rabin_composite(n, s, d, 2)
     || miller_rabin_composite(n, s, d, 325)
     || miller_rabin_composite(n, s, d, 9375)
     || miller_rabin_composite(n, s, d, 28178)
     || miller_rabin_composite(n, s, d, 450775)
     || miller_rabin_composite(n, s, d, 9780504)
     || miller_rabin_composite(n, s, d, 1795265022)
    );
}

uint64_t random_odd_prime() {
    uint64_t n;
    for(;;) {
        // Generate a random odd 64-bit number
        arc4random_buf(&n, 8);
        n |= 1;

        if (isprime(n)) return n;
    }
}
