#include "queue.h"

instruction dequeue(Queue *queue) {
  if (!isEmpty(queue)) {
    QueueNode *node = queue->front;
    instruction value = node->value;
    if (queue->front == queue->end) {
      queue->end = NULL;
    }
    QueueNode *next = node->next;
    queue->front = next;
    free(node);
    return value;
  }
  return NOT_FOUND;
}

instruction peek(Queue *queue) {
  return queue->front->next->value;
}

void enqueue(Queue *queue, instruction value) {
  QueueNode *node = queueNodeInit();
  node->value = value;
  if (isEmpty(queue)) {
    queue->front = node;
    queue->end = node;
  }

  queue->end->next = node;
  queue->end = node;
}

//Helper Functions
bool isEmpty(Queue *queue) {
  return queue->front == NULL;
}

Queue* queueInit(void) {
  Queue *queue = calloc(sizeof(Queue), 1);
  queue->front = NULL;
  queue->end = NULL;
  return queue;
}

void queueFree(Queue *queue) {
  // QueueNode *node = queue->front;
  // while (!isEmpty(queue)) {
  //   QueueNode *next = node->next;
  //   free(node);
  //   node = next;
  // }
  free(queue);
}

QueueNode* queueNodeInit(void) {
  return (QueueNode *) calloc(sizeof(QueueNode), 1);
}
