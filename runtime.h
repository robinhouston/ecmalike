#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdint.h>

#include "alg.h"

typedef uint64_t *parameters_t;
typedef uint64_t *slots_t;
typedef uint64_t *value_t;

struct runtime {
    uint64_t p;
    struct alg *alg;
    slots_t slots;
    value_t value;
};

struct runtime *newRuntime(struct alg *alg, uint64_t p);
void destroyRuntime(struct runtime *runtime);

void runtime_power(struct runtime *runtime, uint64_t n);
uint64_t runtime_value(struct runtime *runtime);

#endif
