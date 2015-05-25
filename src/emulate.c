#include <stdlib.h>
#include <stdio.h>
#include "emulateDefs.h"
#include <stdint.h>

void *voidMem = calloc(65536, 1); // Memory location of the start of the RAM addresses
uint8_t *mem = (uint8_t *)voidMem; //RAM

int main(int argc, char **argv) {
  if (argc != 1) {
    printf("Incorrect number of arguments");
    exit(EXIT_FAILURE);
  }

  FILE *binFile = NULL;

	binFile=fopen(argv[0],"wb");
	if (!binFile)
	{
		printf("Unable to open file!");
		return EXIT_FAILURE;
	}

  cleanup();
  return EXIT_SUCCESS;
}

void cleanup(void) {
// cleanup frees up memory used within the program when program shuts down
  //free(voidMem);
}
