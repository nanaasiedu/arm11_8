#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <stdio.h>
#include "definitions.h"

instruction dequeue(Queue *queue);
instruction peek(Queue *queue);
void enqueue(Queue *queue, instruction value);
//Helper Functions
Queue* queueInit(void);
bool isEmpty(Queue *queue);
void queueFree(Queue *queue);
QueueNode* queueNodeInit(void);

#endif /* end of include guard: QUEUE_H */
