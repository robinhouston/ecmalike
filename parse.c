#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "alg.h"
#include "const.h"
#include "symbol_table.h"

struct circuit *newCircuit(int max_inputs, int max_outputs) {
    struct circuit *circuit = malloc(sizeof(struct circuit));

    circuit->max_outputs = max_outputs;
    circuit->n_outputs = 0;
    circuit->outputs = malloc(sizeof(int) * max_outputs);

    circuit->max_inputs = max_inputs;
    circuit->n_inputs = 0;
    circuit->inputs = malloc(sizeof(int) * max_inputs);

    circuit->n_assignments = 0;
    circuit->assignments = malloc(sizeof(struct assignment) * STATEMENT_MAX);

    return circuit;
}

struct assignment *addAssignment(struct circuit *circuit) {
    if (circuit->n_assignments == STATEMENT_MAX) {
        return NULL;
    }

    struct assignment *assignment = &(circuit->assignments[circuit->n_assignments++]);
    assignment->a = 0;
    assignment->op = NULLOP;
    assignment->b = 0;
    assignment->result = 0;
    return assignment;
}

void destroyCircuit(struct circuit *circuit) {
    if (!circuit) return;
    free(circuit->outputs);
    free(circuit->inputs);
    free(circuit->assignments);
    free(circuit);
}

void destroy(struct alg *alg) {
    free(alg->parameters);
    free(alg->globals);
    destroyCircuit(alg->initial);
    destroyCircuit(alg->mult);
    destroyCircuit(alg->result);
    free(alg);
}

#define die(message) do { \
    fprintf(stderr, "%s at line %d\n", message, line_number); \
    destroy(alg); \
    return NULL; \
} while (0)

#define dief(fmt, ...) do { \
    fprintf(stderr, fmt " at line %d\n", __VA_ARGS__, line_number); \
    destroy(alg); \
    return NULL; \
} while (0)

struct alg *parseFile(FILE *f) {
    struct alg *alg = malloc(sizeof(struct alg));
    alg->dimension = 1;

    alg->n_parameters = 0;
    alg->parameters = malloc(sizeof(int) * PARAMETERS_MAX);

    alg->n_globals = 0;
    alg->globals = malloc(sizeof(int) * GLOBALS_MAX);

    alg->initial = NULL;
    alg->mult = NULL;
    alg->result = NULL;

    struct symbol_table stab;

    char line[LINE_MAX];
    char context[IDENTIFIER_MAX + 1];
    enum { TOP, METADATA, DEF, FUNCTION, RETURN } state = TOP;
    enum scope scope = GLOBAL;
    int line_number = 0;
    struct circuit *current_circuit;
    struct assignment *current_assignment;
    int slot;

    while (fgets(line, LINE_MAX, f)) {
        ++line_number;
        // Split the line into whitespace-separated tokens
        for (char *token = strtok(line, " \t\n"); token; token = strtok(NULL, " \t\n")) {
            if (token[0] == '#') break; // Comments

            switch(state) {
                case TOP:
                    if (!strcmp("def", token)) {
                        state = DEF;
                        current_circuit = NULL;
                        break;
                    }

                    if (
                        !strcmp("dimension", token)
                     || !strcmp("parameter", token)
                     || !strcmp("global", token)
                    ) {
                        state = METADATA;
                        strncpy(context, token, IDENTIFIER_MAX);
                        break;
                    }

                    dief("Unexpected token '%s'", token);

                case METADATA:
                    if (!strcmp("=", token)) {
                        break; // Ignore an = sign immediately following dimension/parameter/global
                    }

                    if (!strcmp("dimension", context)) {
                        alg->dimension = atoi(token);
                        if (alg->dimension <= 0) {
                            dief("Dimension must be positive (not %d)", alg->dimension);
                        }
                        state = TOP;
                        break;
                    }

                    if (!strcmp("parameter", context)) {
                        if (symbol_table_get(&stab, GLOBAL, token)) {
                            die("Duplicate parameter name");
                        }
                        slot = symbol_table_add(&stab, GLOBAL, PARAMETER, token);
                        if (alg->n_parameters == PARAMETERS_MAX) {
                            dief("Too many global parameters (max %d)", PARAMETERS_MAX);
                        }
                        alg->parameters[alg->n_parameters++] = slot;
                        break;
                    }

                    if (!strcmp("global", context)) {
                        if (symbol_table_get(&stab, GLOBAL, token)) {
                            die("Duplicate global name");
                        }
                        slot = symbol_table_add(&stab, GLOBAL, VARIABLE, token);
                        if (alg->n_globals == GLOBALS_MAX) {
                            dief("Too many global variables (max %d)", GLOBALS_MAX);
                        }
                        alg->globals[alg->n_globals++] = slot;
                        break;
                    }

                    dief("The impossible happened: state METADATA with context %s", context);

                case DEF:
                    if (scope == GLOBAL) {
                        if (!strcmp("initial", token)) {
                            if (alg->initial) {
                                dief("Redefinition of function %s", token);
                            }
                            alg->initial = current_circuit = newCircuit(0, alg->dimension);
                            scope = INITIAL;
                        }
                        else if (!strcmp("mult", token)) {
                            if (alg->mult) {
                                dief("Redefinition of function %s", token);
                            }
                            alg->mult = current_circuit = newCircuit(alg->dimension * 2, alg->dimension);
                            scope = MULT;
                        }
                        else if (!strcmp("result", token)) {
                            if (alg->result) {
                                dief("Redefinition of function %s", token);
                            }
                            alg->result = current_circuit = newCircuit(alg->dimension, 1);
                            scope = RESULT;
                        }
                        else {
                            dief("Unexpected function definition %s", token);
                        }
                        strncpy(context, token, IDENTIFIER_MAX);
                        break;
                    }

                    if (current_circuit->n_inputs == current_circuit->max_inputs) {
                        dief("Too many parameters (expected %d)", current_circuit->max_inputs);
                    }

                    slot = symbol_table_add(&stab, scope, PARAMETER, token);
                    current_circuit->inputs[current_circuit->n_inputs++] = slot;

                    break;

                case FUNCTION:
                    if (!current_assignment) {
                        if (!strcmp("return", token)) {
                            state = RETURN;
                            break;
                        }

                        current_assignment = addAssignment(current_circuit);
                        if (!current_assignment) {
                            dief("Function %s too long", context);
                        }
                    }

                    if (!current_assignment->result) {
                        if (strlen(token) > IDENTIFIER_MAX) {
                            dief("Result token %s too long", token);
                        }
                        current_assignment->result = symbol_table_add(&stab, scope, VARIABLE, token);
                        break;
                    }

                    if (!current_assignment->a) {
                        // Yes, this means you can have any number of = signs here
                        if (!strcmp("=", token)) break;

                        if (!strcmp("1", token)) {
                            slot = -1;
                        }
                        else {
                            slot = symbol_table_get(&stab, scope, token);
                            if (!slot) slot = symbol_table_get(&stab, GLOBAL, token);
                            if (!slot) {
                                dief("Variable %s not found", token);
                            }
                        }

                        current_assignment->a = slot;
                        break;
                    }

                    if (current_assignment->op == NULLOP) {
                        if (token[1] != '\0') {
                            dief("String '%s' found where operator expected", token);
                        }
                        switch(token[0]) {
                            case '+':
                                current_assignment->op = PLUS;
                                break;

                            case '-':
                                current_assignment->op = MINUS;
                                break;

                            case '*':
                                current_assignment->op = TIMES;
                                break;

                            case '/':
                                current_assignment->op = DIVIDE;
                                break;

                             default:
                                dief("String '%s' found where operator expected", token);
                        }
                        break;
                    }

                    if (!current_assignment->b) {
                        if (!strcmp("1", token)) {
                            slot = -1;
                        }
                        else {
                            slot = symbol_table_get(&stab, scope, token);
                            if (!slot) slot = symbol_table_get(&stab, GLOBAL, token);
                            if (!slot) {
                                dief("Variable %s not found", token);
                            }
                        }

                        current_assignment->b = slot;
                        break;
                    }

                    dief("Unexpected token '%s' after end of statement", token);

                case RETURN:
                    if (current_circuit->n_outputs == current_circuit->max_outputs) {
                        dief("Too many returned values (expected %d)", current_circuit->max_outputs);
                    }

                    if (!strcmp("1", token)) {
                        slot = -1;
                    }
                    else {
                        slot = symbol_table_get(&stab, scope, token);
                        if (!slot) slot = symbol_table_get(&stab, GLOBAL, token);
                        if (!slot) {
                            dief("Variable %s not found", token);
                        }
                    }

                    current_circuit->outputs[current_circuit->n_outputs++] = slot;
                    break;
            }
        }

        // End of line
        switch (state) {
            case TOP:
                break;

            case METADATA:
                state = TOP;
                break;
            case DEF:
                if (!current_circuit) {
                    die("Syntax error: def statement not followed by function name");
                }

                if (current_circuit->n_inputs != current_circuit->max_inputs) {
                    dief("Function %s takes %d arguments",
                        context, current_circuit->max_inputs);
                }

                state = FUNCTION;
                current_assignment = NULL;
                break;

            case RETURN:
                // return statements terminate the function body
                if (current_circuit->n_outputs != current_circuit->max_outputs) {
                    dief("Too few returned values (expected %d)", current_circuit->max_outputs);
                }

                state = TOP;
                scope = GLOBAL;
                break;

            case FUNCTION:
                if (!current_assignment) {
                    // Ignore blank line in function body
                    break;
                }

                if (
                       !current_assignment->result
                    || !current_assignment->a
                    || current_assignment->op == NULLOP
                    || !current_assignment->b
                ) {
                    die("Syntax error: incomplete assignment statement");
                }
                current_assignment = NULL;
                break;
        }
    }

    if (state == FUNCTION) {
        dief("Function %s does not return", context);
    }
    else if (state != TOP) {
        dief("The impossible happened: reached end of file in state %d", state);
    }

    if (!alg->initial) die("The initial function is not defined");
    if (!alg->mult) die("The mult function is not defined");
    if (!alg->result) die("The result function is not defined");

    if (ferror(f)) {
        perror(NULL);
        destroy(alg);
        return NULL;
    }

    symbol_table_cleanup(&stab);
    return alg;
}
