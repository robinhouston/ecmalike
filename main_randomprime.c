#include <stdio.h>
#include <stdlib.h>

#include "randomprime.h"

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("%llu\n", random_odd_prime());
    }
    else {
        int n = atoi(argv[1]);
        for (int i = 0; i < n; ++i)
        {
            printf("%llu\n", random_odd_prime());
        }
    }
}
