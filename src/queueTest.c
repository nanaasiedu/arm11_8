#include "helpers/definitions.h"
#include "helpers/queue.h"

int main(int argc, char const *argv[]) {

  printf("Starting Test...\n");

  Queue *queue = queueInit();

  printf("Queue: %p\n", queue);
  printf("Front: %p\n", queue->front);
  printf("End  : %p\n", queue->end);

  instruction i = 4;
  instruction j = 5;
  instruction k = 6;

  enqueue(queue, i);
  enqueue(queue, j);
  enqueue(queue, k);
   
   while (!isEmpty(queue)) {
     instruction x = dequeue(queue);
     printf("x: %d\n", x);
   }

  queueFree(queue);

  printf("...Finishing Test\n");

  return 0;
}
