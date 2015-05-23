#ifndef SYMBOLMAP_H
#define SYMBOLMAP_H

static const int MAX_LINE_LENGTH = 512;
static const int INITIAL_MAP_SIZE = 20;

typedef struct {
  int capacity;
  int size;
  char **keys;
  int *values;
} SymbolTable;

void map_init(SymbolTable *map);

void map_print(SymbolTable *map);

int map_get(SymbolTable *map, char *key);

void map_set(SymbolTable *map, char *key, int value);

void map_double_capacity(SymbolTable *map);

void map_free(SymbolTable *map);

#endif /* end of include guard: SYMBOLMAP_H */
