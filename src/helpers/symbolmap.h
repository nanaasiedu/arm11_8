#ifndef SYMBOLMAP_H
#define SYMBOLMAP_H

#include "definitions.h"

static const int MAX_LINE_LENGTH = 512;
static const int INITIAL_MAP_SIZE = 20;

//ADT Functions
int map_get(SymbolTable *map, char *key);
void map_set(SymbolTable *map, char *key, int value);
void map_double_capacity(SymbolTable *map);
//Helper Functions
void map_init(SymbolTable *map);
void map_print(SymbolTable *map);
void map_free(SymbolTable *map);

#endif /* end of include guard: SYMBOLMAP_H */
