#include <stdio.h>
#include <math.h>
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

  //testing(); //FOR TESTING PURPOSES

  //execute(14);// WILL CHANGE TO DECODE

  printf("The program is closing");
  dealloc(); //frees up allocated memory
  return EXIT_SUCCESS;
}

void execute(DecodedInst di) {
  bool condPass = FALSE; //condPass will be TRUE iff cond is satisfied

  switch(di.cond) {
    case 0: // 0000: Z set / is equal
      condPass = ((*rf.CPSR & ipow(2,30)) != 0);
      break;
    case 1: // 0001: Z clear / not equal
      condPass = ((*rf.CPSR & ipow(2,30)) == 0);
      break;
    case 10: // 1010: N equals V / greater equal
      condPass = (*rf.CPSR & ipow(2,31)) == ((*rf.CPSR & ipow(2,38)) << 3);
      break;
    case 11: // 1011: N not equal V / less
      condPass = (*rf.CPSR & ipow(2,31)) != ((*rf.CPSR & ipow(2,28)) << 3);
      break;
    case 12: // 1100: Z clear AND (N = V) / greater than
      condPass = ((*rf.CPSR & ipow(2,30)) == 0) && ((*rf.CPSR & ipow(2,31)) == ((*rf.CPSR & ipow(2,28)) << 3));
      break;
    case 13: // 1101: Z set OR (N != V) / less than
      condPass = ((*rf.CPSR & ipow(2,30)) != 0) || ((*rf.CPSR & ipow(2,31)) != ((*rf.CPSR & ipow(2,28)) << 3));
      break;
    case 14: // 1110 ignore
      condPass = TRUE;
      break;
    default:
      perror("Invalid instruction entered with unknown condition");
      exit(EXIT_FAILURE);
  }

  if (condPass) {
    if ((di.instType & 16) != 0) {// Data processing
      executeDataProcessing(di.instType, di.opcode, di.rn, di.rd, di.operandOffset);

    } else if ((di.instType & 32) != 0) { // Mult
      executeMult(di.instType, di.rd, di.rn, di.rs, di.rm);

    } else if ((di.instType & 64) != 0) { // Data transfer
      executeSingleDataTransfer(di.instType, di.rn, di.rd, di.operandOffset);

    } else if ((di.instType & 128) != 0) { // Branch
      executeBranch(di.operandOffset);
    }
  }

}

void executeDataProcessing(uint8_t instType, uint8_t opcode, uint8_t rn, uint8_t rd, uint32_t operand) {
  int i = (instType & 8) >> 3; // Immediate Operand
  int rotate = (operand >> 7); // rotate segment if i = 1

  int shiftSeg = (operand >> 4); // 8 bit shift segment if i = 0
  int rm = operand & (ipow(2,4)-1); // 4 bit

  int s = (instType & 4) >> 2; // Set condition

  if (i) { // if operand is immediate
    operand = operand & (ipow(2,8)-1); // operand = Immediate segment
    operand = rotr8(operand,rotate*2);
  } else {// operand is a register
    operand = shiftReg(rf.reg[rm], shiftSeg);
  }

}

uint32_t shiftReg(uint32_t value, int shiftSeg) {
  //POST: return shifted value of rm to operand
  int shiftop = shiftSeg & 1; // 1 bit shiftop = shift option. selects whether shift amount is by integer or Rs
  int shiftType = shiftSeg & 6; //2 bit
  int rs = shiftSeg >> 4; // 4 bit
  int conint = shiftSeg >> 3; // 4 bit
  uint32_t res; // result
  int shift; // value to shift by

  if (shiftop) { // if shiftseg is in reg rs mode
    shift = rf.reg[rs] & (2^8 - 1) // DO
  } else { // if shiftseg is in constant int mode
    shift = conint;
  }

  switch(shiftType) {
    case 0: // logical left

    break;
    case 1: // logical right

    break;
    case 2: // arithmetic right

    break;
    case 3: // rotate right 

    break;
  }

  return res;

}

void executeMult(uint8_t instType, uint8_t rd, uint8_t rn, uint8_t rs, uint8_t rm) {

}

void executeSingleDataTransfer(uint8_t instType, uint8_t rn, uint8_t rd, uint32_t offset) {

}

void executeBranch(uint32_t offset) {

}

void testing(void) {
  // Basic test suite
  printf("start testing\n");
  rf.CPSR = malloc(4);
  *rf.CPSR = ipow(2,31);
  DecodedInst di;
  di.cond = 13;
  di.instType = 64;

  execute(di);

  printf("end testing\n");
  free(rf.CPSR);
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
  rf.reg = calloc(4*17,1);
  rf.SP = &rf.reg[13];
  rf.LR = &rf.reg[14];
  rf.PC = &rf.reg[15];
  rf.CPSR = &rf.reg[16];
}

int ipow(int x, int y) {
  // POST: returns x^y cast as an int
  return (int)pow(x,y);
}

int rotr8(uint8_t x, int n) {
  // PRE: x is an unsigned 8 bit number (note x may be any type with 8 or more bits). n is the number x will be rotated by.
  // POST: rotr8 will return the 8 bit value of x rotated n spaces to the right
  uint8_t a = (x & (ipow(2,n)-1)) << (sizeof(x)*8 - n);
  return (x >> n) | a;
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
