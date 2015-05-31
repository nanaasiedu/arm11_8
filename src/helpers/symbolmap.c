#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symbolmap.h"

int find_index(char **a, int size, char *key);

int map_get(SymbolTable *map, char *key) {
  int index = find_index(map->keys, map->size, key);
  if (index == -1) {
    printf("Key not found in Symbol Table: %s\n", key);
    exit(EXIT_FAILURE);
  }
  return map->values[index];
}

void map_set(SymbolTable *map, char *key, int value) {
  map_double_capacity(map);
  map->keys[map->size] = malloc(sizeof(char)*MAX_LINE_LENGTH);
  strcpy(map->keys[map->size], key);
  map->values[map->size] = value;
  map->size++;
}

void map_double_capacity(SymbolTable *map) {
  if (map->size >= map->fullCapacity) {
    map->fullCapacity *= 2;
    map->keys = realloc(map->keys, sizeof(char) * map->fullCapacity);
    map->values = realloc(map->values, sizeof(int) * map->fullCapacity);
  }
}

int find_index(char **a, int size, char *key) {
   for (int i = 0; i < size; i++) {
  	 if (strcmp(a[i], key) == 0) {
  	    return i;
  	 }
   }
   return -1;
}

void map_init(SymbolTable *map) {
  // initialize size and capacity
  map->size = 0;
  map->fullCapacity = INITIAL_MAP_SIZE;
  // allocate memory for map->keys & map->values
  map->keys = malloc(sizeof(char) * map->fullCapacity);
  map->values = malloc(sizeof(int) * map->fullCapacity);
}

void map_print(SymbolTable *map) {
  char **keyIndex = map->keys;
  int *valueIndex = map->values;
  for (int i = 0; i < map->size; i++) {
    printf("(Key:%s, Value: %.8x)\n", *keyIndex++, *valueIndex++);
  }
}

void map_free(SymbolTable *map) {
  free(map->keys);
  free(map->values);
  free(map);
}
