#ifndef INTARRAY_H
#define INTARRAY_H

#include <stdlib.h>
#include <stdio.h>
#include "definitions.h"
#include "typedefs.h"

int dequeue(IntArray *array);
void enqueue(IntArray *array, int value);
//Helper Functions
void init(IntArray *array, int maxSize);
void intArray_free(IntArray *array);

#endif /* end of include guard: INTARRAY_H */
