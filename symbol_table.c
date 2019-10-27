#include <string.h>

#include "const.h"
#include "uthash.h"
#include "symbol_table.h"

int symbol_table_add(struct symbol_table *stab, enum scope scope, enum category category, char *sym) {
    struct symbol *s = malloc(sizeof(struct symbol));

    s->scope = scope;
    strncpy(s->sym, sym, IDENTIFIER_MAX);
    s->sym[IDENTIFIER_MAX] = '\0'; // Make sure s->sym is null-terminated, even if sym is IDENTIFIER_MAX or more characters long
    s->category = category;
    s->slot = ++stab->next_free_slot;

    HASH_ADD(hh, stab->head, scope, KEYLEN, s);
    return s->slot;
}

struct symbol *symbol_table_get_symbol(struct symbol_table *stab, enum scope scope, char *sym) {
    struct symbol *lookup_key = malloc(sizeof(struct symbol));
    struct symbol *result;

    strncpy(lookup_key->sym, sym, IDENTIFIER_MAX);
    lookup_key->sym[IDENTIFIER_MAX] = '\0';
    lookup_key->scope = scope;

    HASH_FIND(hh, stab->head, &lookup_key->scope, KEYLEN, result);
    free(lookup_key);
    return result;
}

int symbol_table_get(struct symbol_table *stab, enum scope scope, char *sym) {
    struct symbol *result = symbol_table_get_symbol(stab, scope, sym);
    return result ? result->slot : 0;
}

void symbol_table_cleanup(struct symbol_table *stab) {
    struct symbol *s, *t;
    HASH_ITER(hh, stab->head, s, t) {
        HASH_DEL(stab->head, s);
        free(s);
    }
}
