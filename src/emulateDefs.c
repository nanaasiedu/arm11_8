#include <stdlib.h>
#include <stdint.h>
#include "emulateDefs.h"

struct regFile rf;

int main(int argc, char const *argv[]) {
  /* code */
  return EXIT_SUCCESS;
}

int32_t fetch(uint8_t *mem){
  int32_t instruction = 0;
  for (int i = 3; i >= 0; i--) {
    instruction = instruction * 256 + mem[(*rf.PC + i)];
  }
  *rf.PC = *rf.PC + 4;
  return instruction;
}

DecodedInst decode(int32_t instruction) {
  DecodedInst di;
  int instType = getInstType(instruction);

  // code

  return di;
}

// returns correct 4 MSB of code for current instruction
int getInstType(int32_t instruction) {
  long mask = 1 << 27;
  if ((instruction & mask) != FALSE){ // bit 27 == 1
    return BRANCH;
  }
  mask = mask >> 1;
  if ((instruction & mask) != FALSE) { // bit 26 == 1
    return DATA_TRANS;
  }
  mask = mask >> 1;
  if ((instruction & mask) != FALSE) { // bit 25 == 1
    return DATA_PROC;
  }
  instruction = instruction >> 4;
  mask = 15;
  if ((instruction & mask) != 9){ // bit 25 == 0; bits [4..7] != 9
    return DATA_PROC;
  }
  return MULT; //bit 25 == 0; bits [4..7] == 9
}
