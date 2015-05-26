#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "emulate.h"

struct regFile rf;

int main(int argc, char const *argv[]) {

  printf("running tests...\n\n");
  
  /* code */

  printf("\n...tests complete\n");
  return EXIT_SUCCESS;
}

int32_t fetch(uint8_t *mem){
  int32_t instruction = 0;
  for (int i = 3; i >= 0; i--) {
    instruction = instruction * 256 + mem[*rf.PC + i];
  }
  *rf.PC = *rf.PC + 4;
  return instruction;
}

DecodedInst decode(int32_t instruction) {
  DecodedInst di;
  di.instType = getInstType(instruction); // only has correct 4 MSBs
  di.instType = getFlags(instruction, di.instType); // all bits correct

  // code

  return di;
}

// returns correct 4 MSB of code for current instruction
uint8_t getInstType(int32_t instruction) {
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

uint8_t getFlags(int32_t instruction, uint8_t instType){
  int mask = 1;
  uint8_t result = instType;
  switch(instType){
    case 16 : //DATA_PROC
        mask = mask << 25;
        if ((instruction & mask) != FALSE){
          result = result + 8;
        }
        mask = mask >> 5;
        if ((instruction & mask) != FALSE){
          result = result + 4;
        }
        break;
    case 32 : //MULT
        mask = mask << 20;
        if ((instruction & mask) != FALSE){
          result = result + 4;
        }
        mask = mask << 1;
        if ((instruction & mask) != FALSE){
          result = result + 2;
        }
        break;
    case 64 : //DATA_TRANS
        mask = mask << 25;
        if ((instruction & mask) != FALSE){
          result = result + 8;
        }
        mask = mask >> 5;
        if ((instruction & mask) != FALSE){
          result = result + 4;
        }
        mask = mask << 4;
        if ((instruction & mask) != FALSE){
          result = result + 2;
        }
        mask = mask >> 1;
        if ((instruction & mask) != FALSE){
          result = result + 1;
        }
        break;
    case 128 : //BRANCH
    default :
        break;
  }
  return result;
}
