#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alg.h"
#include "numbers.h"
#include "runtime.h"

typedef unsigned __int128 uint128_t;

static parameters_t newParameters(struct alg *alg, uint64_t p) {
    parameters_t parameters = malloc(sizeof(uint64_t) * alg->n_parameters);

    uint64_t mask = -1ull;
    do {
        mask >>= 1;
    } while (((mask >> 1) & p) == p);

    for (int i = 0; i < alg->n_parameters; ++i) {
        uint64_t *slot = &parameters[i];
        do {
            arc4random_buf(slot, sizeof(uint64_t));
            *slot = *slot & mask;
        } while (*slot >= p || *slot == 0);
    }

    return parameters;
}

static slots_t newSlots(struct alg *alg, parameters_t parameters) {
    slots_t slots = malloc(sizeof(uint64_t) * alg->n_slots);

    // Initialise parameter slots
    for (int i = 0; i < alg->n_parameters; ++i) {
        slots[alg->parameters[i]] = parameters[i];
    }

    return slots;
}

static value_t newValue(struct alg *alg) {
    value_t value = malloc(sizeof(uint64_t) * alg->dimension);
    return value;
}


static uint64_t modInv(uint64_t p, uint64_t n) {
    uint64_t u;
    uint64_t v;
    xbinGCD(n, p, &u, &v);
    return u;
}

static uint64_t getSlotValue(slots_t slots, int index) {
    if (index == -1) return 1;
    if (index == -2) return 0;
    return slots[index];
}

static void runAssignment(struct assignment *assignment, slots_t slots, uint64_t p) {
    switch (assignment->op) {
        case NULLOP: break;
        case PLUS:
            slots[assignment->result] = (
                getSlotValue(slots, assignment->a)
                +
                getSlotValue(slots, assignment->b)
            ) % p;
            break;

        case MINUS:
            slots[assignment->result] = (
                getSlotValue(slots, assignment->a)
                -
                getSlotValue(slots, assignment->b)
            ) % p;
            break;

        case TIMES:
            slots[assignment->result] = (
                getSlotValue(slots, assignment->a)
                *
                getSlotValue(slots, assignment->b)
            ) % p;
            break;

        case DIVIDE:
            slots[assignment->result] = (
                getSlotValue(slots, assignment->a)
                *
                modInv(p, getSlotValue(slots, assignment->b))
            ) % p;
            break;
    }

    // printf("[%d] = [%d] %llu %c [%d] %llu = %llu\n",
    //     assignment->result,
    //     assignment->a, getSlotValue(slots, assignment->a),
    //     "N+-*/"[assignment->op],
    //     assignment->b, getSlotValue(slots, assignment->b),
    //     slots[assignment->result]);
}

static void runCircuit(
    struct circuit *circuit,
    slots_t slots,
    uint64_t *input,
    uint64_t *output,
    uint64_t p
) {
    for (int i = 0; i < circuit->n_inputs; ++i) {
        slots[circuit->inputs[i]] = input[i];
    }

    for (int i = 0; i < circuit->n_assignments; ++i) {
        struct assignment *assignment = &circuit->assignments[i];
        runAssignment(assignment, slots, p);
    }

    for (int i = 0; i < circuit->n_outputs; ++i) {
        output[i] = getSlotValue(slots, circuit->outputs[i]);
    }
}

struct runtime *newRuntime(struct alg *alg, uint64_t p) {
    struct runtime *runtime = malloc(sizeof(struct runtime));

    runtime->p = p;
    runtime->alg = alg;
    parameters_t parameters = newParameters(alg, p);
    runtime->slots = newSlots(alg, parameters);
    free(parameters);
    runtime->value = newValue(alg);

    runCircuit(alg->initial, runtime->slots, NULL, runtime->value, p);

    return runtime;
}

void destroyRuntime(struct runtime *runtime) {
    free(runtime->slots);
    free(runtime->value);
    free(runtime);
}

void runtime_power(struct runtime *runtime, uint64_t n) {
    struct alg *alg = runtime->alg;
    int dimension = alg->dimension;

    uint64_t inputs[dimension * 2];
    uint64_t initial_value[dimension];
    memcpy(&initial_value, runtime->value, sizeof(uint64_t) * dimension);

    uint64_t b = 1;
    while ((b<<1) && n >= (b<<1)) b <<= 1;

    while (b > 1) {
        b >>= 1;

        memcpy(&inputs[0], runtime->value, sizeof(uint64_t) * dimension);
        memcpy(&inputs[dimension], runtime->value, sizeof(uint64_t) * dimension);
        runCircuit(alg->mult, runtime->slots, &inputs[0], runtime->value, runtime->p);

        if (n & b) {
            memcpy(&inputs[0], initial_value, sizeof(uint64_t) * dimension);
            memcpy(&inputs[dimension], runtime->value, sizeof(uint64_t) * dimension);
            runCircuit(alg->mult, runtime->slots, &inputs[0], runtime->value, runtime->p);
        }
    }
}

uint64_t runtime_value(struct runtime *runtime) {
    uint64_t value;
    runCircuit(runtime->alg->result, runtime->slots, runtime->value, &value, runtime->p);
    return value;
}
