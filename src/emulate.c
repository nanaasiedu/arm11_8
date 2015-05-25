#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "emulate.h"

FILE *binFile = NULL; //Binary file containing instructions
uint8_t *mem = NULL; //Memory

int main (int argc, char const *argv[]) {
  if (argc != 2) {
    printf("Incorrect number of arguments");
    exit(EXIT_FAILURE);
  }

  mem = calloc(MEM16BIT, 1); // allocates 2^16 bit memory addresses to mem

  loadFileToMem(argv[1]); //Binary loader: loads file passed through argv into mem


  enterC();
  dealloc();
  return EXIT_SUCCESS;
}

void loadFileToMem(char const *file) {
  // Reads bytes from file and inserts them into the mem array
  if ((binFile = fopen(file,"r")) == NULL){
    perror("Unable to open file!");
    exit(EXIT_FAILURE);
  }

  fread(mem,1,MEM16BIT,binFile);
}

void dealloc(void) {
  // Frees all memory locations alloacted during the execution of the program
  free(mem);
}

void enterC(void) {
  printf("Press enter");
  while (getchar() != '\n');
}
