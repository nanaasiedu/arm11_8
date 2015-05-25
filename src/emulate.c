#include <stdlib.h>
#include <stdio.h>
#include "emulateDefs.h"
#include <stdint.h> 

uint8_t mem[65536] = calloc(65536); // RAM

int main(int argc, char **argv) {
  if (argc != 1) {
    printf("Incorrect number of arguments");
    exit(EXIT_FAILURE);
  }

  cleanup()
  return EXIT_SUCCESS;
}

void cleanup(void) {
// cleanup frees up memory used within the program when program shuts down
  free(mem);
}
