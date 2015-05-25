#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "emulateDefs.h"

char *mem = NULL; //RAM

int main(int argc, char **argv) {
  if (argc != 1) {
    printf("Incorrect number of arguments");
    exit(EXIT_FAILURE);
  }

  mem = calloc(65536, 1);

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
  free(mem);
}
