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
    int max_outputs;
    int n_outputs;
    int *outputs;

    int max_inputs;
    int n_inputs;
    int *inputs;

    int n_assignments;
    struct assignment *assignments;
};

struct alg {
    int dimension;
    int n_parameters;
    int n_globals;

    struct circuit *initial;
    struct circuit *mult;
    struct circuit *result;
};

#endif
