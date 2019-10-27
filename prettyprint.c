#include <stdio.h>

#include "alg.h"

static char opChar(enum op op) {
    switch (op) {
        case NULLOP: return 'N';
        case PLUS: return '+';
        case MINUS: return '-';
        case TIMES: return '*';
        case DIVIDE: return '/';
    }
}

static char var[3][5];
static char *v(int slot, int place) {
    if (slot == -1) return "1";
    snprintf(var[place], 5, "v%d", slot);
    return var[place];
}

void prettyPrintCircuit(const char *name, struct circuit *circuit) {
    printf("def %s", name);
    for (int i = 0; i < circuit->n_inputs; ++i)
    {
        printf(" %s", v(circuit->inputs[i], 0));
    }
    printf("\n");

    for (int i = 0; i < circuit->n_assignments; ++i)
    {
        struct assignment *ass = &(circuit->assignments[i]);
        printf("    %s = %s %c %s\n", v(ass->result, 0), v(ass->a, 1), opChar(ass->op), v(ass->b, 2));
    }

    printf("    return");
    for (int i = 0; i < circuit->n_outputs; ++i)
    {
        printf(" %s", v(circuit->outputs[i], 0));
    }
    printf("\n");
}

void prettyPrint(struct alg *alg) {
    printf("dimension = %d\n", alg->dimension);

    if (alg->n_parameters > 0) {
        printf("parameters");
        for (int i = 0; i < alg->n_parameters; ++i)
        {
            printf(" %s", v(alg->parameters[i], 0));
        }
        printf("\n");
    }

    if (alg->n_globals > 0) {
        printf("global");
        for (int i = 0; i < alg->n_globals; ++i)
        {
            printf(" %s", v(alg->globals[i], 0));
        }
        printf("\n");
    }

    printf("\n");


    prettyPrintCircuit("initial", alg->initial);
    printf("\n");

    prettyPrintCircuit("mult", alg->mult);
    printf("\n");

    prettyPrintCircuit("result", alg->result);
    printf("\n");
}
