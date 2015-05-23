#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symbolmap.h"

int find_index(char **a, int size, char *key);

void map_init(SymbolTable *map) {
  // initialize size and capacity
  map->size = 0;
  map->capacity = INITIAL_MAP_SIZE;
  // allocate memory for map->keys & map->values
  map->keys = malloc(sizeof(char) * map->capacity * MAX_LINE_LENGTH);
  map->values = malloc(sizeof(int) * map->capacity);
}

void map_print(SymbolTable *map) {
  char **keyIndex = map->keys;
  int *valueIndex = map->values;
  for (int i = 0; i < map->size; i++) {
    printf("(Key:%s, Value: %d)\n", *keyIndex, *valueIndex);
    keyIndex++;
    valueIndex++;
  }
}


int map_get(SymbolTable *map, char *key) {
  int index = find_index(map->keys, map->size, key);
  if (index == -1) {
    perror("Key not found in Symbol Table");
    exit(EXIT_FAILURE);
  }
  return map->values[index];
}

int find_index(char **a, int size, char *key) {
   for (int i = 0; i < size; i++) {
  	 if (strcmp(a[i], key) == 0) {
  	    return(i);
  	 }
   }
   return(-1);
}

void map_set(SymbolTable *map, char *key, int value) {
  map->keys[map->size] = malloc(sizeof(char)*MAX_LINE_LENGTH);
  strcpy(map->keys[map->size], key);
  map->values[map->size] = value;
  map->size++;
}

void map_double_capacity(SymbolTable *map) {
  if (map->size >= map->capacity) {
    map->capacity *= 2;
    map->keys = realloc(map->keys, sizeof(char) * map->capacity * MAX_LINE_LENGTH);
    map->values = realloc(map->values, sizeof(int) * map->capacity);
  }
}

void map_free(SymbolTable *map) {
  free(map->keys);
  free(map->values);
}
