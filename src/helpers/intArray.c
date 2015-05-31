#include "intArray.h"

int dequeue(IntArray *array) {
  if (array->size > 0) {
    int elem = array->values[array->first];
    array->first = (array->first + 1) % array->fullCapacity;
    array->size--;
    return elem;
  } else {
    return NOT_FOUND;
  }
}

void enqueue(IntArray *array, int value) {
  if (array->size < array->fullCapacity) {
    array->last = (array->last + 1) % array->fullCapacity;
    array->values[array->last] = value;
    array->size++;
  }
}

//Helper Functions
void init(IntArray *array, int maxSize) {
  array->fullCapacity = maxSize;
  array->first = 0;
  array->last = array->fullCapacity - 1;
  array->size = 0;
  array->values = calloc(array->fullCapacity, sizeof(int));
}

void intArray_free(IntArray *array) {
  free(array->values);
  free(array);
}
