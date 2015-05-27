#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "emulate.h"

struct regFile rf;

/*  Test functions */
//  Remember to remove

void testFetch(void);
void testGetInstType(void);
void testDecodeForDataProc(void);

/*  End of test functions*/

int main(int argc, char const *argv[]) {

  printf("running tests...\n\n");

  testFetch();
  testGetInstType();
  testDecodeForDataProc();
  /* code */

  printf("\n...tests complete\n");
  return EXIT_SUCCESS;
}

int32_t fetch(uint8_t *mem){
  int32_t instruction = 0;
  for (int i = 3; i >= 0; i--) {
    instruction <<= 8;
    instruction += mem[*rf.PC + i];
  }
  *rf.PC = *rf.PC + 4;
  return instruction;
}

DecodedInst decode(int32_t instruction) {
  DecodedInst di;
  di.instType = getInstType(instruction); // only has correct 4 MSBs
  switch(di.instType){
    case 16 : //DATA_PROC
        decodeForDataProc(instruction, &di);
        break;
    case 32 : //MULT
        decodeForMult(instruction, &di);
        break;
    case 64 : //DATA_TRANS
        decodeForDataTrans(instruction, &di);
        break;
    case 128 : //BRANCH
        decodeForBranch(instruction, &di);
    case 0 : //HALT
    default :
        break;
  }
  return di;
}

// returns correct 4 MSB of code for current instruction
uint8_t getInstType(int32_t instruction) {
  if (instruction == HALT){
    return HALT;
  }
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
  mask = 15 << 4;
  int multCode = 9 << 4;
  if ((instruction & mask) != multCode){ // bit 25 == 0; bits [4..7] != 9
    return DATA_PROC;
  }
  return MULT; //bit 25 == 0; bits [4..7] == 9
}

void decodeForDataProc(int32_t instruction, DecodedInst *di) {

  // set flag bits: I S x x
  printf("%i\n",di->instType );
  long mask = 1 << 25;
  if ((instruction & mask) != FALSE){
    di->instType += 8;
  }
  printf("%i\n",di->instType );
  mask >>= 5;
  if ((instruction & mask) != FALSE){
    di->instType += 4;
  }
  printf("%i\n",di->instType );

  // set opcode
  mask = 15;
  di->opcode = instruction >> 21;
  di->opcode = di->opcode & mask;

  // set registers
  di->rn = instruction >> 16;
  di->rn &= mask;
  di->rd = instruction >> 12;
  di->rd = di->rd & mask;

  // get operandOffset (operand)
  mask = 4095; // 2^12 - 1 for bits [0..11]
  di->operandOffset = instruction & mask;

}

void decodeForMult(int32_t instruction, DecodedInst *di) {

  // set flag bits: x S A x
  long mask = 1;
  mask = mask << 20;
  if ((instruction & mask) != FALSE){
    di->instType = di->instType + 4;
  }
  mask = mask << 1;
  if ((instruction & mask) != FALSE){
    di->instType = di->instType + 2;
  }

  // set registers
  mask = 15;
  di->rd = instruction >> 16;
  di->rd = di->rd & mask;
  di->rn = instruction >> 12;
  di->rn = di->rn & mask;
  di->rs = instruction >> 8;
  di->rs = di->rs & mask;
  di->rm = instruction & mask;

}

void decodeForDataTrans(int32_t instruction, DecodedInst *di) {

  // set flag bits: I L P U
  long mask = 1;
  mask = mask << 25;
  if ((instruction & mask) != FALSE){
    di->instType = di->instType + 8;
  }
  mask = mask >> 5;
  if ((instruction & mask) != FALSE){
    di->instType = di->instType + 4;
  }
  mask = mask << 4;
  if ((instruction & mask) != FALSE){
    di->instType = di->instType + 2;
  }
  mask = mask >> 1;
  if ((instruction & mask) != FALSE){
    di->instType = di->instType + 1;
  }

  // set registers
  mask = 15;
  di->rn = instruction >> 16;
  di->rn = di->rn & mask;
  di->rd = instruction >> 12;
  di->rd = di->rd & mask;

  // get operandOffset (offset)
  mask = 4095; // 2^12 - 1 for bits [0..11]
  di->operandOffset = instruction & mask;

}

void decodeForBranch(int32_t instruction, DecodedInst *di) {

  // flag bits: x x x x

  // get operandOffset (signed offset)
  long mask = 1 << 24;
  mask--; // 2^24 - 1 for bits [0..23]
  di->operandOffset = instruction & mask; //  = unaltered offset bits

  mask = 1 << 23;
  if ((instruction & mask) != FALSE) { // offset is negative
    di->operandOffset = ~(di->operandOffset ^ 0) + 1;
        // = di->operandOffset XNOR 0, then + 1
        // = two's complement +ve value
    di->operandOffset *= (-1);
  }

}


/*  Test functions */
//  Remember to remove

void testFetch(void) {
  printf("testing fetch\n");
  uint8_t *mem = NULL;
  mem = calloc(16, 1);
  for (uint8_t i = 0; i < 16; i++) {
    mem[i] = i;
  }
  /* addr | 32bit value
   * 0    | 03020100H = 50462976
   * 4    | 07060504H = 117835012
   * 8    | 0B0A0908H = 185207048
   * 12   | 0F0E0D0CH = 252579084
   */
   uint32_t zero = 0;
   rf.PC = &zero;
    printf("At address 0, should store 50462976\nActually stores            %d\n", fetch(mem));
    printf("At address 4, should store 117835012\nActually stores            %d\n", fetch(mem));
    printf("At address 8, should store 185207048\nActually stores            %d\n", fetch(mem));
    printf("At address 12, should store 252579084\nActually stores             %d\n", fetch(mem));
    free(mem);
    printf("\n");
}

void testGetInstType(void) {
  printf("testing getInstType\n");
  int32_t instruction = 183562240; // Branch : xxxx1010xx...
  printf("Should be a branch statement, code: %d\n actual code: %d\n", BRANCH, getInstType(instruction));
  instruction = 250671104; // Branch : xxxx11xx...
  printf("Should be a branch statement, code: %d\n actual code: %d\n", BRANCH, getInstType(instruction));
  instruction = 116453376; // DataTrans : xxxx01xx...
  printf("Should be a data trans statement, code: %d\n actual code: %d\n", DATA_TRANS, getInstType(instruction));
  instruction = 144; // Mult : ...xx1001xxxx
  printf("Should be a mult statement, code: %d\n actual code: %d\n", MULT, getInstType(instruction));
  instruction = 33554576; // DataProc : xxxxxx1xx...xx1001xxxx
  printf("Should be a dataProc statement, code: %d\n actual code: %d\n", DATA_PROC, getInstType(instruction));
  instruction = 1; // DataProc : xxxxxx0xx...xx0000xxx1
  printf("Should be a dataProc statement, code: %d\n actual code: %d\n", DATA_PROC, getInstType(instruction));
  instruction = 33554432; // DataProc : xxxxxx1xx...
  printf("Should be a dataProc statement, code: %d\n actual code: %d\n", DATA_PROC, getInstType(instruction));
  instruction = 0; // Halt : 0
  printf("Should be a halt statement, code: %d\n actual code: %d\n", HALT, getInstType(instruction));
  printf("\n");
}

void testDecodeForDataProc(void) {
  printf("testing decodeForDataProc\n");
  DecodedInst di;
  di.instType = DATA_PROC;
  int32_t instruction = 212992; // 00034000H I = 0, S = 0, rn = 3, rd = 4
  decodeForDataProc(instruction, &di);
  printf("\nfield\t\texpected\tactual\n");
  printf("instType\t%d\t\t%d\n", DATA_PROC, di.instType);
  printf("Rn\t\t%d\t\t%d\n", 3, di.rn);
  printf("Rd\t\t%d\t\t%d\n", 4, di.rd);
  printf("==========\n");
  di.instType = DATA_PROC;
  instruction = 34254848; // 020AB000H I = 1, S = 0, rn = 10, rd = 11
  decodeForDataProc(instruction, &di);
  printf("\nfield\t\texpected\tactual\n");
  printf("instType\t%d\t\t%d\n", (DATA_PROC + 8), di.instType);
  printf("Rn\t\t%d\t\t%d\n", 10, di.rn);
  printf("Rd\t\t%d\t\t%d\n", 11, di.rd);
  printf("==========\n");
  di.instType = DATA_PROC;
  instruction = 1605632; // 00188000H I = 0, S = 1, rn = 8, rd = 8
  decodeForDataProc(instruction, &di);
  printf("\nfield\t\texpected\tactual\n");
  printf("instType\t%d\t\t%d\n", (DATA_PROC + 4), di.instType);
  printf("Rn\t\t%d\t\t%d\n", 8, di.rn);
  printf("Rd\t\t%d\t\t%d\n", 8, di.rd);
  printf("==========\n" );
  printf("\n");

}

/*  End of test functions*/
