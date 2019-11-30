#ifndef ALG_H
#define ALG_H

enum op {
    NULLOP,

    PLUS,
    MINUS,
    TIMES,
    DIVIDE
};

struct assignment {
    int a;
    enum op op;
    int b;

    int result;
};

struct circuit {
    int max_inputs;
    int n_inputs;
    int *inputs;

    int max_outputs;
    int n_outputs;
    int *outputs;

    int n_assignments;
    struct assignment *assignments;
};

struct alg {
    int dimension;
    int n_slots;

    int n_parameters;
    int *parameters;

    // TODO: do we need these?
    int n_globals;
    int *globals;

    struct circuit *initial;
    struct circuit *mult;
    struct circuit *result;
};

#endif
