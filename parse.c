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

struct alg *parseFile(FILE *f) {
    struct alg *alg = malloc(sizeof(struct alg));
    alg->dimension = 1;
    alg->n_parameters = 0;
    alg->n_globals = 0;
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

                    fprintf(stderr, "Unexpected token '%s' at line %d\n", token, line_number);
                    return NULL;

                case METADATA:
                    if (!strcmp("=", token)) {
                        break; // Ignore an = sign immediately following dimension/parameter/global
                    }

                    if (!strcmp("dimension", context)) {
                        alg->dimension = atoi(token);
                        if (alg->dimension <= 0) {
                            fprintf(stderr, "Dimension must be positive (not %d) at line %d\n",
                                alg->dimension, line_number);
                            return NULL;
                        }
                        state = TOP;
                        break;
                    }

                    if (!strcmp("parameter", context)) {
                        if (symbol_table_get(&stab, GLOBAL, token)) {
                            fprintf(stderr, "Duplicate parameter name on line %d\n", line_number);
                            return NULL;
                        }
                        symbol_table_add(&stab, GLOBAL, PARAMETER, token);
                        alg->n_parameters += 1;
                        break;
                    }

                    if (!strcmp("global", context)) {
                        if (symbol_table_get(&stab, GLOBAL, token)) {
                            fprintf(stderr, "Duplicate global name on line %d\n", line_number);
                            return NULL;
                        }
                        symbol_table_add(&stab, GLOBAL, VARIABLE, token);
                        alg->n_globals += 1;
                        break;
                    }

                    fprintf(stderr, "The impossible happened: state METADATA with context %s\n", context);
                    return NULL;

                case DEF:
                    if (scope == GLOBAL) {
                        if (!strcmp("initial", token)) {
                            if (alg->initial) {
                                fprintf(stderr, "Redefinition of function %s on line %d\n", token, line_number);
                                return NULL;
                            }
                            alg->initial = current_circuit = newCircuit(0, alg->dimension);
                            scope = INITIAL;
                        }
                        else if (!strcmp("mult", token)) {
                            if (alg->mult) {
                                fprintf(stderr, "Redefinition of function %s on line %d\n", token, line_number);
                                return NULL;
                            }
                            alg->mult = current_circuit = newCircuit(alg->dimension * 2, alg->dimension);
                            scope = MULT;
                        }
                        else if (!strcmp("result", token)) {
                            if (alg->result) {
                                fprintf(stderr, "Redefinition of function %s on line %d\n", token, line_number);
                                return NULL;
                            }
                            alg->result = current_circuit = newCircuit(alg->dimension, 1);
                            scope = RESULT;
                        }
                        else {
                            fprintf(stderr, "Unexpected function definition %s on line %d\n", token, line_number);
                            return NULL;
                        }
                        strncpy(context, token, IDENTIFIER_MAX);
                        break;
                    }

                    if (current_circuit->n_inputs == current_circuit->max_inputs) {
                        fprintf(stderr, "Too many parameters (expected %d) at line %d\n",
                            current_circuit->max_inputs, line_number);
                        return NULL;
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
                            fprintf(stderr, "Function %s too long at line %d\n", context, line_number);
                            return NULL;
                        }
                    }

                    if (!current_assignment->result) {
                        if (strlen(token) > IDENTIFIER_MAX) {
                            fprintf(stderr, "Result token %s too long at line %d\n", token, line_number);
                            return NULL;
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
                                fprintf(stderr, "Variable %s not found at line %d\n", token, line_number);
                                return NULL;
                            }
                        }

                        current_assignment->a = slot;
                        break;
                    }

                    if (current_assignment->op == NULLOP) {
                        if (token[1] != '\0') {
                            fprintf(stderr, "String '%s' found where operator expected at line %d\n", token, line_number);
                            return NULL;
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
                                fprintf(stderr, "String '%s' found where operator expected at line %d\n", token, line_number);
                                return NULL;
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
                                fprintf(stderr, "Variable %s not found at line %d\n", token, line_number);
                                return NULL;
                            }
                        }

                        current_assignment->b = slot;
                        break;
                    }

                    fprintf(stderr, "Unexpected token '%s' after end of statement on line %d\n", token, line_number);
                    return NULL;

                case RETURN:
                    if (current_circuit->n_outputs == current_circuit->max_outputs) {
                        fprintf(stderr, "Too many returned values (expected %d) at line %d\n",
                            current_circuit->max_outputs, line_number);
                        return NULL;
                    }

                    if (!strcmp("1", token)) {
                        slot = -1;
                    }
                    else {
                        slot = symbol_table_get(&stab, scope, token);
                        if (!slot) slot = symbol_table_get(&stab, GLOBAL, token);
                        if (!slot) {
                            fprintf(stderr, "Variable %s not found at line %d\n", token, line_number);
                            return NULL;
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
                    fprintf(stderr, "Syntax error: def statement not followed by function name at line %d\n", line_number);
                    return NULL;
                }

                if (current_circuit->n_inputs != current_circuit->max_inputs) {
                    fprintf(stderr, "Function %s takes %d arguments at line %d\n",
                        context, current_circuit->max_inputs, line_number);
                    return NULL;
                }

                state = FUNCTION;
                current_assignment = NULL;
                break;

            case RETURN:
                // return statements terminate the function body
                if (current_circuit->n_outputs != current_circuit->max_outputs) {
                    fprintf(stderr, "Too few returned values (expected %d) at line %d\n",
                        current_circuit->max_outputs, line_number);
                    return NULL;
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
                    fprintf(stderr, "Syntax error: incomplete assignment statement on line %d\n", line_number);
                    return NULL;
                }
                current_assignment = NULL;
                break;
        }
    }

    if (state != TOP) {
        fprintf(stderr, "Reached end of file in state %d\n", state);
        return NULL;
    }

    if (ferror(f)) {
        perror(NULL);
        return NULL;
    }

    symbol_table_cleanup(&stab);
    return alg;
}
