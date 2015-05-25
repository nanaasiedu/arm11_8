#include <stdlib.h>
#include <stdio.h>
#include "emulateDefs.h"

int main(int argc, char **argv) {
  if (argc != 1) {
    printf("Incorrect number of arguments");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
