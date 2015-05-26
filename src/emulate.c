#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "emulate.h"

FILE *binFile = NULL; //Binary file containing instructions
uint8_t *mem = NULL;  //Memory
struct regFile rf;    //sets register file

int main (int argc, char const *argv[]) {
  if (argc != 2) {
    printf("Incorrect number of arguments");
    exit(EXIT_FAILURE);
  }

  mem = calloc(MEM16BIT, 1); //allocates 2^16 bit memory addresses to mem

 clearRegfile(rf); // Sets all registers to 0

  loadFileToMem(argv[1]); //Binary loader: loads file passed through argv into mem

  execute(14);// WILL CHANGE

  dealloc(); //frees up allocated memory
  return EXIT_SUCCESS;
}

void execute(uint16_t cond) {
  //COND WILL CHANGE TO DECODE STRUCT
  bool condPass = FALSE; //condPass will be TRUE iff cond is satisfied

  switch(cond) {
    case 0: // 0000: Z set / is equal
      condPass = ((*rf.CPSR & (2^30)) != 0);
      break;
    case 1: // 0001: Z clear / not equal
      condPass = ((*rf.CPSR & (2^30)) == 0);
      break;
    case 10: // 1010: N equals V / greater equal
      condPass = (*rf.CPSR & (2^31)) == (*rf.CPSR & (2^28) << 3);
      break;
    case 11: // 1011: N not equal V / less
      condPass = (*rf.CPSR & (2^31)) != (*rf.CPSR & (2^28) << 3);
      break;
    case 12: // 1100: Z clear AND (N = V) / greater than
      condPass = ((*rf.CPSR & (2^30)) == 0) && ((*rf.CPSR & (2^31)) == (*rf.CPSR & (2^28) << 3));
      break;
    case 13: // 1101: Z set OR (N != V) / less than
      condPass = ((*rf.CPSR & (2^30)) != 0) || ((*rf.CPSR & (2^31)) != (*rf.CPSR & (2^28) << 3));
      break;
    case 14: // 1110 ignore
      condPass = TRUE;
      break;
    default:
      perror("Invalid instruction entered with unknown condition");
      exit(EXIT_FAILURE);
  }

  if (condPass) {

  }
  
}

void loadFileToMem(char const *file) {
  // Reads bytes from file and inserts them into the mem array
  if ((binFile = fopen(file,"r")) == NULL){
    perror("Unable to open file!");
    exit(EXIT_FAILURE);
  }

  fread(mem,1,MEM16BIT,binFile);
}

void clearRegfile (struct regFile rf) {
  // allocates 12 bytes for each register
  rf.reg = calloc(4*12,1);
  rf.SP = calloc(4,1);
  rf.LR = calloc(4,1);
  rf.PC = calloc(4,1);
  rf.CPSR = calloc(4,1);
}

void dealloc(void) {
  // Frees all memory locations alloacted during the execution of the program
  free(rf.reg);
  free(rf.SP);
  free(rf.LR);
  free(rf.PC);
  free(rf.CPSR);
  free(mem);
}

void enterC(void) {
  printf("Press enter");
  while (getchar() != '\n');
}
