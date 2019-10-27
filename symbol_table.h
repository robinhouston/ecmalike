#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "uthash.h"

/*
Each symbol has a name, category, scope and slot.
The name and scope constitute the key, so there is
only one symbol in the table with a particular name and scope.

The current implementation assigns a unique slot for
each symbol in the table. It would be possible to reuse
slots between different functions; getting more sophisticated,
slots could be reused within a function when the scopes are
non-overlapping.
*/

enum scope {
    GLOBAL,

    INITIAL,
    MULT,
    RESULT
};

enum category {
    PARAMETER,
    VARIABLE
};

struct symbol {
    // Key fields
    enum scope scope;
    char sym[IDENTIFIER_MAX + 1];

    // Value fields
    enum category category;
    int slot;

    // Used internally by uthash
    UT_hash_handle hh;
};

struct symbol_table {
    int next_free_slot;

    struct symbol *head; // head of linked list used by uthash
};

#define KEYLEN (offsetof(struct symbol, sym) - offsetof(struct symbol, scope) + sizeof(char[IDENTIFIER_MAX + 1]))

int symbol_table_add(struct symbol_table *stab, enum scope scope, enum category category, char *sym);
int symbol_table_get(struct symbol_table *stab, enum scope scope, char *sym);
void symbol_table_cleanup(struct symbol_table *stab);

#endif
