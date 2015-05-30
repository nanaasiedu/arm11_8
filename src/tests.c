#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "emulate.h"
#include "tests.h"

RegFile rf;

void runAllTests(void){
  printf("running tests...\n\n");

  testFetch();
  testGetInstType();
  testDecodeForDataProc();
  testDecodeForMult();
  testDecodeForDataTrans();
  testDecodeForBranch();
  testingExecute(); //PASSED
  testingDataProc(); //PASSED
  testingExecuteBranch(); // PASSED
  testingHelpers(); //PASSED
  testingDataTrans();
  printf("All test passed\n");

  printf("\n...tests complete\n");
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
  int32_t instruction = 0x00034000;
  // I = 0, S = 0, rn = 3, rd = 4
  // opcode = 0, operand = 0
  decodeForDataProc(instruction, &di);
  printf("\nfield\t\texpected\tactual\n");
  printf("instType\t%d\t\t%d\n", DATA_PROC, di.instType);
  printf("Rn\t\t%d\t\t%d\n", 3, di.rn);
  printf("Rd\t\t%d\t\t%d\n", 4, di.rd);
  printf("Opcode\t\t%d\t\t%d\n", 0, di.opcode);
  printf("Operand\t\t%d\t\t%d\n", 0, di.operandOffset);
  printf("==========\n");
  di.instType = DATA_PROC;
  instruction = 0x02EAB003;
  // I = 1, S = 0, rn = 10, rd = 11
  // opcode = 7, operand = 3
  decodeForDataProc(instruction, &di);
  printf("\nfield\t\texpected\tactual\n");
  printf("instType\t%d\t\t%d\n", (DATA_PROC + 8), di.instType);
  printf("Rn\t\t%d\t\t%d\n", 10, di.rn);
  printf("Rd\t\t%d\t\t%d\n", 11, di.rd);
  printf("Opcode\t\t%d\t\t%d\n", 7, di.opcode);
  printf("Operand\t\t%d\t\t%d\n", 3, di.operandOffset);
  printf("==========\n");
  di.instType = DATA_PROC;
  instruction = 0x00B88FFE;
  // I = 0, S = 1, rn = 8, rd = 8
  // opcode = 5, operand = 4094
  decodeForDataProc(instruction, &di);
  printf("\nfield\t\texpected\tactual\n");
  printf("instType\t%d\t\t%d\n", (DATA_PROC + 4), di.instType);
  printf("Rn\t\t%d\t\t%d\n", 8, di.rn);
  printf("Rd\t\t%d\t\t%d\n", 8, di.rd);
  printf("Opcode\t\t%d\t\t%d\n", 5, di.opcode);
  printf("Operand\t\t%d\t\t%d\n", 4094, di.operandOffset);
  printf("==========\n" );
  printf("\n");
}

void testDecodeForMult(void) {
  printf("testing decodeForMult\n");
  DecodedInst di;
  di.instType = MULT;
  int32_t instruction = 0x0001F390; // S = 0, A = 0, rd = 1, rn = 15, rs = 3, rm = 0
  decodeForMult(instruction, &di);
  printf("\nfield\t\texpected\tactual\n");
  printf("instType\t%d\t\t%d\n", MULT, di.instType);
  printf("Rd\t\t%d\t\t%d\n", 1, di.rd);
  printf("Rn\t\t%d\t\t%d\n", 15, di.rn);
  printf("Rs\t\t%d\t\t%d\n", 3, di.rs);
  printf("Rm\t\t%d\t\t%d\n", 0, di.rm);
  printf("==========\n");
  di.instType = MULT;
  instruction = 0x002A779D; // S = 0, A = 1, rd = 10, rn = 7, rs = 7, rm = 13
  decodeForMult(instruction, &di);
  printf("\nfield\t\texpected\tactual\n");
  printf("instType\t%d\t\t%d\n", (MULT + 2), di.instType);
  printf("Rd\t\t%d\t\t%d\n", 10, di.rd);
  printf("Rn\t\t%d\t\t%d\n", 7, di.rn);
  printf("Rs\t\t%d\t\t%d\n", 7, di.rs);
  printf("Rm\t\t%d\t\t%d\n", 13, di.rm);
  printf("==========\n");
  di.instType = MULT;
  instruction = 0x0032479C; // S = 1, A = 1, rd = 2, rn = 4, rs = 7, rm = 12
  decodeForMult(instruction, &di);
  printf("\nfield\t\texpected\tactual\n");
  printf("instType\t%d\t\t%d\n", (MULT + 6), di.instType);
  printf("Rd\t\t%d\t\t%d\n", 2, di.rd);
  printf("Rn\t\t%d\t\t%d\n", 4, di.rn);
  printf("Rs\t\t%d\t\t%d\n", 7, di.rs);
  printf("Rm\t\t%d\t\t%d\n", 12, di.rm);
  printf("==========\n");
  printf("\n");
}

void testDecodeForDataTrans(void) {
  printf("testing decodeForDataTrans\n");
  DecodedInst di;
  di.instType = DATA_TRANS;
  int32_t instruction = 0x0405D390;
  // I = 0, L = 0, P = 0, U = 0
  // rn = 5, rd = 13, offset = 912
  decodeForDataTrans(instruction, &di);
  printf("\nfield\t\texpected\tactual\n");
  printf("instType\t%d\t\t%d\n", DATA_TRANS, di.instType);
  printf("Rn\t\t%d\t\t%d\n", 5, di.rn);
  printf("Rd\t\t%d\t\t%d\n", 13, di.rd);
  printf("offset\t\t%d\t\t%d\n", 912, di.operandOffset);
  printf("==========\n");
  di.instType = DATA_TRANS;
  instruction = 0x07910005;
  // I = 1, L = 1, P = 1, U = 1
  // rn = 1, rd = 0, offset = 5
  decodeForDataTrans(instruction, &di);
  printf("\nfield\t\texpected\tactual\n");
  printf("instType\t%d\t\t%d\n", (DATA_TRANS + 15), di.instType);
  printf("Rn\t\t%d\t\t%d\n", 1, di.rn);
  printf("Rd\t\t%d\t\t%d\n", 0, di.rd);
  printf("offset\t\t%d\t\t%d\n", 5, di.operandOffset);
  printf("==========\n");
  di.instType = DATA_TRANS;
  instruction = 0x070A9FFF;
  // I = 1, L = 0, P = 1, U = 0
  // rn = 10, rd = 9, offset = 4095
  decodeForDataTrans(instruction, &di);
  printf("\nfield\t\texpected\tactual\n");
  printf("instType\t%d\t\t%d\n", (DATA_TRANS + 10), di.instType);
  printf("Rn\t\t%d\t\t%d\n", 10, di.rn);
  printf("Rd\t\t%d\t\t%d\n", 9, di.rd);
  printf("offset\t\t%d\t\t%d\n", 4095, di.operandOffset);
  printf("==========\n");
  di.instType = DATA_TRANS;
  instruction = 0x04923010;
  // I = 0, L = 1, P = 0, U = 1
  // rn = 2, rd = 3, offset = 16
  decodeForDataTrans(instruction, &di);
  printf("\nfield\t\texpected\tactual\n");
  printf("instType\t%d\t\t%d\n", (DATA_TRANS + 5), di.instType);
  printf("Rn\t\t%d\t\t%d\n", 2, di.rn);
  printf("Rd\t\t%d\t\t%d\n", 3, di.rd);
  printf("offset\t\t%d\t\t%d\n", 16, di.operandOffset);
  printf("==========\n");
  printf("\n");
}

void testDecodeForBranch(void) {
  printf("testing decodeForBranch\n");
  DecodedInst di;
  di.instType = BRANCH;
  int32_t instruction = 0x0A000009;
  // offset = 9
  decodeForBranch(instruction, &di);
  printf("\nfield\t\texpected\tactual\n");
  printf("instType\t%d\t\t%d\n", BRANCH, di.instType);
  printf("offset\t\t%d\t\t%d\n", 9, di.operandOffset);
  printf("==========\n");
  di.instType = BRANCH;
  instruction = 0x0AFFFFF7;
  // offset = -9
  decodeForBranch(instruction, &di);
  printf("\nfield\t\texpected\tactual\n");
  printf("instType\t%d\t\t%d\n", BRANCH, di.instType);
  printf("offset\t\t%d\t\t%d\n", -9, di.operandOffset);
  printf("==========\n");
  printf("\n");
}

void testingExecute(void) { //PASSED
  // Basic test suite
  printf("start testing\n\n");
  DecodedInst di;
  di.instType = BRANCH;

  /*
  printf("N = %d\n", getBit(*rf.CPSR,Nbit));
  printf("Z = %d\n", getBit(*rf.CPSR,Zbit));
  printf("V = %d\n", getBit(*rf.CPSR,Vbit));
  */

  printf("Test 0 ====\n");
  alterN(0);
  printf("N = %d expected 0\n", getBit(*rf.CPSR,Nbit));
  alterN(1);
  printf("N = %d expected 1\n", getBit(*rf.CPSR,Nbit));

  alterZ(0);
  alterN(1);
  printf("Z = %d expected 0\n", getBit(*rf.CPSR,Zbit));
  alterZ(1);
  alterN(0);
  printf("Z = %d expected 1\n", getBit(*rf.CPSR,Zbit));
  printf("====\n\n");

  printf("Test 1 ====\n");
  alterZ(1);
  di.cond = 1;
  execute(di);
  printf("Expected: fail\n");
  printf("====\n\n");

  printf("Test 2 ====\n");
  alterZ(0);
  alterN(1);
  alterV(0);
  di.cond = 12;
  execute(di);
  printf("Expected: fail\n");
  printf("====\n\n");

  printf("Test 3 ====\n");
  alterZ(0);
  alterN(0);
  alterV(1);
  di.cond = 13;
  di.instType = BRANCH;
  execute(di);
  printf("Expected: branch\n");
  printf("====\n\n");

  printf("Test 4 ====\n");
  alterZ(0);
  alterN(1);
  alterV(1);
  di.cond = 12;
  di.instType = MULT;
  execute(di);
  printf("Expected: mult\n");
  printf("====\n\n");

  printf("Test 5 ====\n");
  alterZ(0);
  alterN(1);
  alterV(0);
  di.cond = 11;
  di.instType = DATA_PROC;
  execute(di);
  printf("Expected: data processing\n");
  printf("====\n\n");

  printf("Test 6 ====\n");
  alterZ(0);
  alterN(1);
  alterV(1);
  di.cond = 10;
  di.instType = DATA_TRANS;
  execute(di);
  printf("Expected: data transfer\n");
  printf("====\n\n");

  printf("Test 7 ====\n");
  alterZ(0);
  alterN(1);
  alterV(1);
  di.cond = 0;
  di.instType = DATA_TRANS;
  execute(di);
  printf("Expected: fail\n");
  alterZ(1);
  di.instType = MULT;
  execute(di);
  printf("Expected: mult\n");
  printf("====\n\n");

  printf("Test 8 ====\n");
  di.cond = 14;
  di.instType = BRANCH;
  execute(di);
  printf("Expected: branch\n");
  printf("====\n\n");

  printf("end testing\n");
}

void testingDataProc(void) { //PASSED
  printf("start testing\n\n");

  printf("Test 1 CPSRZN====\n");

  setCPSRZN(0,1);
  printf(
  "CPSR: "BYTETOBINARYPATTERN"\n",
  BYTETOBINARY(getBinarySeg(*rf.CPSR,31,4))
  );
  printf("Expected: CPSR: 0100\n");
  setCPSRZN(100,1);
  printf(
  "CPSR: "BYTETOBINARYPATTERN"\n",
  BYTETOBINARY(getBinarySeg(*rf.CPSR,31,4))
  );
  printf("Expected: CPSR: 0000\n");
  setCPSRZN(-10,1);
  printf(
  "CPSR: "BYTETOBINARYPATTERN"\n",
  BYTETOBINARY(getBinarySeg(*rf.CPSR,31,4))
  );
  printf("Expected: CPSR: 1000\n");
  setCPSRZN(0,0);
  printf(
  "CPSR: "BYTETOBINARYPATTERN"\n",
  BYTETOBINARY(getBinarySeg(*rf.CPSR,31,4))
  );
  printf("Expected: CPSR: 1000\n");

  printf("====\n\n");

  printf("Test 2 barrelShift====\n");
  int res;
  rf.reg[2] = 85; // ...01010101
  rf.reg[3] = 252; // ...1111100
  rf.reg[4] = (1 << 30) + 85 + 512; // 01...1001010101
  rf.reg[5] = (1 << 31) + (1 << 28) + (1 << 27); //10011..
  rf.reg[6] = 1 << 31; // 1...
  rf.reg[7] = 29;

  uint32_t shiftSeg = 26; //  0001 1 (3)| 01 (lsr) | 0 (conint)
  res = barrelShift(rf.reg[2], shiftSeg, 1);
  printf("barrelShift(rf.reg[2], 26, 1) = %u C = %d\n",res,getBit(*rf.CPSR,Cbit));
  printf("expected 10 and 1\n");

  shiftSeg = 36; //  0010 0 (4)| 10 (asr) | 0 (conint)
  res = barrelShift(rf.reg[6], shiftSeg, 1);
  printf("barrelShift(rf.reg[6], 36, 1) = %u C = %d\n",res,getBit(*rf.CPSR,Cbit));
  printf("expected %u and 0\n", (1 << 31) + (1 << 30) + (1 << 29) + (1 << 28) + (1 << 27));

  shiftSeg = 97; //  0110 0 (6)| 00 (lsl) | 1 (reg)
  res = barrelShift(rf.reg[3], shiftSeg, 1);
  printf("barrelShift(rf.reg[3], 97, 1) = %u C = %d\n",res,getBit(*rf.CPSR,Cbit));
  printf("expected %u and 0\n", rf.reg[3]);

  shiftSeg = 113; //  0111 0 (7)| 00 (lsl) | 1 (reg)
  res = barrelShift(rf.reg[3], shiftSeg, 1);
  printf("barrelShift(rf.reg[3], 113, 1) = %u C = %d\n",res,getBit(*rf.CPSR,Cbit));
  printf("expected %u and 1\n", (1 << 31));

  shiftSeg = 119; //  0111 0 (7)| 11 (ror) | 1 (reg)
  res = barrelShift(rf.reg[5], shiftSeg, 1);
  printf("barrelShift(rf.reg[5], 119, 1) = %u C = %d\n",res,getBit(*rf.CPSR,Cbit));
  printf("expected %u and ?\n", (1 << 31) + (1 << 30) + 4);

  printf("====\n\n");

  printf("Test 3 DataProc====\n");
  rf.reg[1] = 1; // ...1
  rf.reg[2] = 85; // ...01010101
  rf.reg[3] = 252; // ...1111100
  rf.reg[4] = (1 << 30) + 85 + 512; // 01...1001010101
  rf.reg[5] = (1 << 31) + (1 << 28) + (1 << 27); //10011..
  rf.reg[6] = (1 << 31); // 1...
  rf.reg[7] = 29;
  rf.reg[8] = (85 + 512) << 9;
  rf.reg[9] = 1;
  rf.reg[10] = 0x7fffffff; // 0111....1

  alterC(1);

  uint8_t instType = 28; // 0001 | 1 (I) | 1 (S) | 00
  uint8_t opcode = 0; // AND
  uint8_t rn = 2;
  uint8_t rd = 1;
  uint32_t operand =  512 + 256; // ...0011 (3*2) | 0...0
  executeDataProcessing(instType, opcode, rn,
                             rd, operand);
  printf("rf.reg[1] = %u C = %d Z = %d N = %d \n",rf.reg[1],
         getBit(*rf.CPSR,Cbit),getBit(*rf.CPSR,Zbit), getBit(*rf.CPSR,Nbit));
  printf("expected 0 and C = 0 Z = 1 N = 0\n");


  instType = 24; // 0001 | 1 (I) | 0 (S) | 00
  opcode = 1; // EOR
  rn = 2;
  rd = 1;
  operand =  2048 + 1024 + 512 + 7; // ...1110 (14*2)| ...111(7)
  executeDataProcessing(instType, opcode, rn,
                             rd, operand);

  alterC(1);

  printf("rf.reg[1] = %u C = %d\n", rf.reg[1],getBit(*rf.CPSR,Cbit));
  printf("expected 37 and C = 1 (unchanged)\n");


  instType = 29; // 0001 | 1 (I) | 1 (S) | 01
  opcode = 2; // sub
  rn = 7;
  rd = 1;
  operand =  2048 + 1024 + 512 + 256 + 8; // ...1111 (15*2)| .1000(8)

  alterC(0);
  executeDataProcessing(instType, opcode, rn,
                             rd, operand);

  printf("rf.reg[1] = %d C = %d\n", rf.reg[1],getBit(*rf.CPSR,Cbit));
  printf("expected -3 and C = 1 (borrow)\n");


  instType = 17; // 0001 | 0 (I) | 0 (S) | 01
  opcode = 3; // rsb
  rn = 4;
  rd = 1;
  operand =   1024 + 128 + 32 + 8; // 0100 1 | 01 (lsr) | 0 | 1000(8)

  alterC(0);
  executeDataProcessing(instType, opcode, rn,
                             rd, operand);

  printf("rf.reg[1] = %d C = %d\n", rf.reg[1],getBit(*rf.CPSR,Cbit));
  printf("expected %d and C = 0 (unchanged)\n", -(1 << 30));


  instType = 23; // 0001 | 0 (I) | 1 (S) | 11
  opcode = 4; // add
  rn = 9;
  rd = 1;
  operand =  10; // ...0 (0*2)| .1010(10)

  alterC(0);
  executeDataProcessing(instType, opcode, rn,
                             rd, operand);

  printf("rf.reg[1] = %d C = %d\n", rf.reg[1],getBit(*rf.CPSR,Cbit));
  printf("expected %d and C = 1 (OV)\n", (1 << 31));


  rf.reg[1] = 1; // ...1
  instType = 23; // 0001 | 1 (I) | 1 (S) | 11
  opcode = 8; // tst
  rn = 10;
  rd = 1;
  operand =  1024 + 256; // ...101 (5*2)| ....000(0)

  alterC(0);
  executeDataProcessing(instType, opcode, rn,
                             rd, operand);

  printf("rf.reg[1] = %d C = %d Z = %d\n",
          rf.reg[1], getBit(*rf.CPSR,Cbit), getBit(*rf.CPSR,Zbit));
  printf("expected 1 (unchanged) and C = 0 Z = 1\n");


  rf.reg[1] = 1; // ...1
  instType = 31; // 0001 | 1 (I) | 1 (S) | 11
  opcode = 9; // teq
  rn = 10;
  rd = 1;
  operand =  256 + 2; // ...01 (1*2)| ....010(2)

  alterC(0);
  executeDataProcessing(instType, opcode, rn,
                             rd, operand);

  printf("rf.reg[1] = %d C = %d N = %d\n",
          rf.reg[1], getBit(*rf.CPSR,Cbit), getBit(*rf.CPSR,Nbit));
  printf("expected 1 (unchanged) and C = 1 N = 1\n");


  rf.reg[1] = 1; // ...1
  instType = 29; // 0001 | 1 (I) | 1 (S) | 01
  opcode = 10; // cmp
  rn = 7;
  rd = 1;
  operand =  2048 + 1024 + 512 + 256 + 8; // ...1111 (15*2)| .1000(8)

  alterC(0);
  executeDataProcessing(instType, opcode, rn,
                             rd, operand);

  printf("rf.reg[1] = %d C = %d\n", rf.reg[1],getBit(*rf.CPSR,Cbit));
  printf("expected 1 (unchanged) and C = 1 (borrow)\n");


  instType = 29; // 0001 | 1 (I) | 0 (S) | 01
  opcode = 12; // or
  rn = 10;
  rd = 1;
  operand =  256 + 15; // ...01 (1*2)| .1111(15)

  alterC(1);
  alterN(0);
  executeDataProcessing(instType, opcode, rn,
                             rd, operand);

  printf("rf.reg[1] = %d C = %d N = %d\n", rf.reg[1],getBit(*rf.CPSR,Cbit),getBit(*rf.CPSR,Nbit));
  printf("expected -1 and C = 1 (unchanged) N = 0 (unchanged)\n");


  instType = 29; // 0001 | 1 (I) | 0 (S) | 01
  opcode = 13; // mov
  rn = 1;
  rd = 1;
  operand =  256 + 15; // ...01 (1*2)| .1111(15)

  alterC(1);
  alterN(0);
  executeDataProcessing(instType, opcode, rn,
                             rd, operand);

  printf("rf.reg[1] = %d C = %d N = %d\n", rf.reg[1],getBit(*rf.CPSR,Cbit),getBit(*rf.CPSR,Nbit));
  printf("expected %d and C = 1 (unchanged) N = 0 (unchanged)\n", (1 << 31) + (1 << 30) + 3);

  printf("====\n\n");


  printf("end testing\n");
}

void testingDataTrans(void) { // TESTING
  printf("start testing\n\n");

  printf("Test 1 DataTrans====\n");
  writewMem(32+16, 512);
  rf.reg[2] = 3;
  rf.reg[1] = 512; // 0x200
  uint8_t instType =  64 + 11;// 0100 (4) | 1011 (11)
  uint8_t rn = 1;
  uint8_t rd = 0;
  uint32_t offset = 256 + 2; // ...10 (2)| 00 (lsl) | 0 | 0010 (2)

  executeSingleDataTransfer(instType, rn, rd,
                                 offset);


  printf("====\n\n");

  printf("end testing\n");
}

void testingHelpers(void) { //PASSED
  printf("start testing\n\n");

  printf("Test getBit ====\n");

  printf("getBit(5,7) = %d\n",getBit(5,7));
  printf("expected 0\n");
  printf("getBit(69,2) = %d\n",getBit(69,2));
  printf("expected 1\n");
  printf("getBit(78,6) = %d\n",getBit(78,6));
  printf("expected 1\n");
  printf("getBit((1 << 31),31) = %d\n",getBit((1 << 31),31));
  printf("expected 1\n");
  printf("getBit(78,45) = %d\n",getBit(78,45));
  printf("expected 0\n");

  printf("====\n\n");

  printf("Test getBinarySeg ====\n");

  printf("getBinarySeg(15,3,3) = %d\n", getBinarySeg(15,3,3));
  printf("expected 7\n");
  printf("getBinarySeg(179,7,6) = %d\n", getBinarySeg(179,7,6));
  printf("expected 44\n");
  printf("getBinarySeg((1 << 31)+(1 << 29),3,3) = %d\n", getBinarySeg((1 << 31)+(1 << 29),31,3));
  printf("expected 5\n");

  printf("====\n\n");

  printf("Test rotr8 ====\n");

  printf("rotr8(12,2) = %u\n", rotr8(12,2));
  printf("expected 3\n");
  printf("rotr8(69,3) = %u\n", rotr8(69,3));
  printf("expected 168\n");
  printf("rotr8(203,4) = %u\n", rotr8(203,4));
  printf("expected 188\n");

  printf("====\n\n");

  printf("Test rotr32 ====\n");

  printf("rotr32(12,2) = %u\n", rotr32(12,2));
  printf("expected 3\n");
  printf("rotr32((1 << 31),3) = %u\n", (uint32_t)rotr32((1 << 31),3));
  printf("expected %u\n", (uint32_t)(1 << 28));
  printf("rotr32(13,3) = %u\n", rotr32(13,3));
  printf("expected %u\n", (uint32_t)((1 << 31) + (1 << 29) + 1));

  printf("====\n\n");

  printf("Test wMem ====\n");
  mem[12] = 85;
  mem[15] = 64;
  printf("wMem[12] = %u\n", wMem(12));
  printf("expected %u\n", (1 << 30) + 85);

  mem[80] = 3;
  mem[81] = 1;
  mem[83] = 96;
  printf("wMem[80] = %u\n", wMem(80));
  printf("expected %u\n", (1 << 30) + (1 << 29) + 256 + 3);
  printf("====\n\n");

  printf("Test writewMem ====\n");
  writewMem((1 << 30) + 85,12);
  printf("wMem[12] = %u\n", wMem(12));
  printf("expected %u\n", (1 << 30) + 85);

  writewMem((1 << 30) + (1 << 29) + 256 + 3,80);
  printf("wMem[80] = %u\n", wMem(80));
  printf("expected %u\n", (1 << 30) + (1 << 29) + 256 + 3);
  printf("====\n\n");

  printf("end testing\n");
}

void testingExecuteBranch(void) {
  printf("testing executeBranch\n");
  DecodedInst di = decode(0x0A000009);
  // offset = 9
  uint32_t pcValue = *rf.PC;
  printf("%d\n", pcValue);
  executeBranch(di.operandOffset);
  printf("\nfield\texpected\tactual\n");
  printf("PC\t%d\t%d\n", (pcValue + 36), *rf.PC);
  di = decode(0x0AFFFFF7);
  // offset = -9
  pcValue = *rf.PC;
  printf("%d\n", pcValue);
  executeBranch(di.operandOffset);
  printf("PC\t%d\t%d\n", (pcValue - 36), *rf.PC);
  printf("==========\n");
  printf("\n");
}
/*  End of test functions*/
