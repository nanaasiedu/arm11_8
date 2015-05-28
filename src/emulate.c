#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "emulate.h"
#include <limits.h>

// TODO: CPSR bit const / opcode const / shift type const
// TODO: Redo reg mem output

FILE *binFile = NULL; //Binary file containing instructions
uint8_t *mem = NULL;  //Memory
struct regFile rf;    //sets register file

int main (int argc, char const *argv[]) {
  if (argc != 2) {
    printf("Incorrect number of arguments\n");
    exit(EXIT_FAILURE);
  }

  mem = calloc(MEM16BIT, 1); //allocates 2^16 bit memory addresses to mem

  clearRegfile(); // Sets all registers to 0

  loadFileToMem(argv[1]); //Binary loader: loads file passed through argv into
                          //mem

  /*int executeResult;
  int32_t instruction;
  DecodedInst di;
  // PC = 0 before entering loop
  do {
    instruction = fetch(mem);
    do {
      di = decode(instruction);
      instruction = fetch(mem);
      executeResult = execute(di);
    } while(executeResult == EXE_CONTINUE);
  } while(executeResult != EXE_HALT); //fetch again if EXE_BRANCH
  */

  //testingExecute(); //PASSED
  testingDataProc();
  testingExecuteBranch();
  //testingHelpers(); //PASSED

  outputMemReg();
  printf("The program is closing\n");
  dealloc(); //frees up allocated memory
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
  long mask = 1 << 25;
  if ((instruction & mask) != FALSE){
    di->instType += 8;
  }
  mask >>= 5;
  if ((instruction & mask) != FALSE){
    di->instType += 4;
  }

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
  di->rd &= mask;
  di->rn = instruction >> 12;
  di->rn &= mask;
  di->rs = instruction >> 8;
  di->rs &= mask;
  di->rm = instruction & mask;

}

void decodeForDataTrans(int32_t instruction, DecodedInst *di) {

  // set flag bits: I L P U
  long mask = 1 << 25;
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
  mask = 1 << 12;
  mask--; // 2^12 - 1 for bits [0..11]
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
    di->operandOffset |= 0xFFF00000;
    // 1s extended to MSB
  }

}

int execute(DecodedInst di) { //confirmed
  if (di.instType == EXE_HALT) {
    return EXE_HALT;
  }

  bool condPass = FALSE; //condPass will be TRUE iff cond is satisfied
  int res = EXE_CONTINUE; // res will contain the state of the next executed instruction depending on whether halt or branch is executed

  switch(di.cond) {
    case 0: // 0000: Z set / is equal
      condPass = getBit(*rf.CPSR,Zbit);
      break;
    case 1: // 0001: Z clear / not equal
      condPass = !getBit(*rf.CPSR,Zbit);
      break;
    case 10: // 1010: N equals V / greater equal
      condPass = getBit(*rf.CPSR,Nbit) == getBit(*rf.CPSR,Vbit);
      break;
    case 11: // 1011: N not equal V / less
      condPass = getBit(*rf.CPSR,Nbit) != getBit(*rf.CPSR,Vbit);
      break;
    case 12: // 1100: Z clear AND (N = V) / greater than
      condPass = (!getBit(*rf.CPSR,Zbit)) && (getBit(*rf.CPSR,Nbit) ==
                 getBit(*rf.CPSR,Vbit));
      break;
    case 13: // 1101: Z set OR (N != V) / less than
      condPass = getBit(*rf.CPSR,Zbit) ||  (getBit(*rf.CPSR,Nbit) !=
                 getBit(*rf.CPSR,Vbit));
      break;
    case 14: // 1110 ignore
      condPass = TRUE;
      break;
    default:
      perror("Invalid instruction entered with unknown condition");
      dealloc();
      exit(EXIT_FAILURE);
  }

  if (condPass) {

    if ((di.instType & DATA_PROC) != 0) {// Data processing
      executeDataProcessing(di.instType, di.opcode, di.rn, di.rd,
                            di.operandOffset);
      //printf("Entered Data processing\n"); // FOR TESTING
    } else if ((di.instType & MULT) != 0) { // Mult
      executeMult(di.instType, di.rd, di.rn, di.rs, di.rm);
      //printf("Entered Mult\n"); // FOR TESTING

    } else if ((di.instType & DATA_TRANS) != 0) { // Data transfer
      executeSingleDataTransfer(di.instType, di.rn, di.rd, di.operandOffset);
      //printf("Entered Data transfer\n"); // FOR TESTING

    } else if ((di.instType & BRANCH) != 0) { // Branch
      executeBranch(di.operandOffset);
      //printf("Entered Branch\n"); // FOR TESTING
      res = EXE_BRANCH;
    }

  }/* else {
    printf("Cond failed\n"); // FOR TESTING
  }*/

  return res;

}

void executeDataProcessing(uint8_t instType, uint8_t opcode, uint8_t rn, uint8_t
                           rd, uint32_t operand) {//TESTING
  bool i = getBit(instType,3); // Immediate Operand
  int rotate = getBinarySeg(operand,11,4); // 4 bit rotate segment if i = 1

  int shiftSeg = getBinarySeg(operand,11,8); // 8 bit shift segment if i = 0
  int rm = getBinarySeg(operand,3,4); // 4 bit

  bool s = getBit(instType,2); // Set condition

  int testRes = 0; // result from test operations

  if (i) { // if operand is immediate
    operand = getBinarySeg(operand,7,8); // operand = Immediate segment
    operand = rotr32(operand,rotate*2);
  } else { // operand is a register
    operand = barrelShift(rf.reg[rm], shiftSeg, s);
  }

  switch(opcode) {
    case 0: // and
      rf.reg[rd] = rf.reg[rn] & operand;
      setCPSRZN(rf.reg[rd],s);
    break;
    case 1: // eor
      rf.reg[rd] = rf.reg[rn] ^ operand;
      setCPSRZN(rf.reg[rd],s);
    break;
    case 2:// sub
      rf.reg[rd] = (int)rf.reg[rn] - (int)operand;
      alterC((int)operand > (int)rf.reg[rn]); // borrow
      //will occur if subtraend > minuend
      setCPSRZN(rf.reg[rd],s);
    break;
    case 3: // rsb
      rf.reg[rd] = (int)operand - (int)rf.reg[rn];
      setCPSRZN(rf.reg[rd],s);
      alterC((int)operand < (int)rf.reg[rn]);
    break;
    case 4: // add
      rf.reg[rd] = (int)rf.reg[rn] + (int)operand;

      alterC((rf.reg[rn] > 0) && (operand > INT_MAX - rf.reg[rn])); // overflow
      // will occur based on this condition
      setCPSRZN(rf.reg[rd],s);
    break;
    case 8: // tst
      testRes = rf.reg[rn] & operand;
      setCPSRZN(testRes,s);
    break;
    case 9: // teq
      testRes = rf.reg[rn] ^ operand;
      setCPSRZN(testRes,s);
    break;
    case 10: // cmp
      testRes = rf.reg[rn] - operand;
      setCPSRZN(testRes,s);
    break;
    case 12: // orr
      testRes = rf.reg[rn] | operand;
      setCPSRZN(testRes,s);
    break;
    case 13: // mov
      rf.reg[rd] = operand;
      setCPSRZN(rf.reg[rd],s);
    break;
  }

}

uint32_t barrelShift(uint32_t value, int shiftSeg, int s) { //confirmed
  //POST: return shifted value of rm to operand
  bool shiftop = getBit(shiftSeg,0); // 1 bit shiftop = shift option. selects whether shift amount is by integer or Rs
  int shiftType = getBinarySeg(shiftSeg,2,2); //2 bit
  int rs = getBinarySeg(shiftSeg,7,4); // 4 bit
  int conint = getBinarySeg(shiftSeg,7,5); // 5 bit
  uint32_t res = 0; // result
  int shift; // value to shift by

  /* printf("shifSeg = %d\n",shiftSeg);
  printf("shiftop = %d\n",shiftop);
  printf("rs = %d\n",rs);
  printf("shift type = %d\n", shiftType);
  printf("conint = %d\n", conint); */

  if (shiftop) { // if shiftseg is in reg rs mode
    shift = rf.reg[rs] & (ipow(2,8) - 1);
  } else { // if shiftseg is in constant int mode
    shift = conint;
  }

  //printf("shift = %d\n", shift);

  switch(shiftType) {
    case 0: // logical left
      res = value << shift;
      alterC(getBit(value,sizeof(value)*8 - shift));
    break;
    case 1: // logical right
      res = value >> shift;
      alterC(getBit(value,shift - 1));
    break;
    case 2: // arithmetic right
      if (getBit(value,31)) { // if value is negative
        res = (value >> shift) | ((ipow(2,shift+1)-1) << (31-shift));
      } else {
        res = value >> shift;
      }
      alterC(getBit(value,shift - 1));
    break;
    case 3: // rotate right
      res = rotr32(value,shift);
      alterC(getBit(value,shift - 1));
    break;
  }

  return res;

}

void setCPSRZN(int value, bool trigger) { //Confirmed
  //will set the CPSR Z and N bits depending on value
  if (!trigger) {
    return;
  }

  alterZ(value == 0);
  alterN(value & ipow(2,31));
}

void executeMult(uint8_t instType, uint8_t rd, uint8_t rn, uint8_t rs, uint8_t
                 rm) {
  bool a = getBit(instType,1); //Accumulate
  bool s = getBit(instType,2); //set condition

  if (!a) { // Normal multiply
    rf.reg[rd] = rf.reg[rm]*rf.reg[rs];
  } else { // multiply-accumulate
    rf.reg[rd] = rf.reg[rm]*rf.reg[rs] + rf.reg[rn];
  }

  setCPSRZN(rf.reg[rd],s);

}

void executeSingleDataTransfer(uint8_t instType, uint8_t rn, uint8_t rd,
                               uint32_t offset) {
  bool i = getBit(instType,3); // immediate offset
  bool l = getBit(instType,2); // Load/Store
  bool p = getBit(instType,1); // Pre/Post, set = Pre
  bool u = getBit(instType,0); // Up bit

  int shiftSeg = (offset >> 4); // 8 bit shift segment if i = 0
  int rm = offset & (ipow(2,4)-1); // 4 bit

  if (!i) { // if offset is immediate
    offset = offset & (ipow(2,12)-1); // offset = Immediate
  } else {// offset is a register
    offset = barrelShift(rf.reg[rm], shiftSeg, 0);
  }

  int soffset = offset; // signed offset
  int pcoffset = 0;     // if rn = PC we must add 8 bytes to account for pipeline
  if (!u) { // if we are subtracting soffset will be negative
    soffset *= -1;
  }

  if(rn == 15) {
    pcoffset = 8*8; // 8 bytes
  }

  if (p) { //pre-indexing
    if (l) { //load
      rf.reg[rd] = mem[rf.reg[rn]+soffset+pcoffset];
    } else { //store
      mem[rf.reg[rn]+soffset+pcoffset] = rf.reg[rd];
    }

  } else { //post-indexing
    if (l) { //load
      rf.reg[rd] = mem[rf.reg[rn]+pcoffset];
    } else { //store
      mem[rf.reg[rn]+pcoffset] = rf.reg[rd];
    }

    rf.reg[rn] = rf.reg[rn] + soffset +pcoffset;
  }

}

void executeBranch(int offset) {
  *rf.PC += offset << 2;
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

void testingDataProc(void) {
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
  rf.reg[4] = ipow(2,30) + 85 + 512; // 01...1001010101
  rf.reg[5] = ipow(2,31) + ipow(2,28) + ipow(2,27); //10011..
  rf.reg[6] = -ipow(2,31); // 1...
  rf.reg[7] = 29;

  uint32_t shiftSeg = 26; //  0001 1 (3)| 01 (lsr) | 0 (conint)
  res = barrelShift(rf.reg[2], shiftSeg, 1);
  printf("barrelShift(rf.reg[2], 26, 1) = %u C = %d\n",res,getBit(*rf.CPSR,Cbit));
  printf("expected 10 and 1\n");

  shiftSeg = 36; //  0010 0 (4)| 10 (asr) | 0 (conint)
  res = barrelShift(rf.reg[6], shiftSeg, 1);
  printf("barrelShift(rf.reg[6], 36, 1) = %u C = %d\n",res,getBit(*rf.CPSR,Cbit));
  printf("expected %u and 0\n", (uint32_t)(ipow(2,31) + ipow(2,30) + ipow(2,29) + ipow(2,28) + ipow(2,27)));

  shiftSeg = 97; //  0110 0 (6)| 00 (lsl) | 1 (reg)
  res = barrelShift(rf.reg[3], shiftSeg, 1);
  printf("barrelShift(rf.reg[3], 97, 1) = %u C = %d\n",res,getBit(*rf.CPSR,Cbit));
  printf("expected %u and 0\n", rf.reg[3]);

  shiftSeg = 113; //  0111 0 (7)| 00 (lsl) | 1 (reg)
  res = barrelShift(rf.reg[3], shiftSeg, 1);
  printf("barrelShift(rf.reg[3], 113, 1) = %u C = %d\n",res,getBit(*rf.CPSR,Cbit));
  printf("expected %u and 1\n", (uint32_t)ipow(2,31));

  shiftSeg = 119; //  0111 0 (7)| 11 (ror) | 1 (reg)
  res = barrelShift(rf.reg[5], shiftSeg, 1);
  printf("barrelShift(rf.reg[5], 119, 1) = %u C = %d\n",res,getBit(*rf.CPSR,Cbit));
  printf("expected %u and ?\n", (uint32_t)(ipow(2,31) + ipow(2,30) + (uint64_t)4));

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

void loadFileToMem(char const *file) {
  // Reads bytes from file and inserts them into the mem array
  if ((binFile = fopen(file,"r")) == NULL){
    perror("Unable to open file!");
    exit(EXIT_FAILURE);
  }

  fread(mem,1,MEM16BIT,binFile);
}

void clearRegfile (void) {
  // allocates 4 bytes for each register
  rf.reg = calloc(NUM_REG,4);
  rf.SP = &rf.reg[13];
  rf.LR = &rf.reg[14];
  rf.PC = &rf.reg[15];
  rf.CPSR = &rf.reg[16];
}

void alterV(bool set) {
  if (set) {
    *rf.CPSR = *rf.CPSR | ipow(2,28);
  } else {
    *rf.CPSR = *rf.CPSR & ((ipow(2,32) - 1) - ipow(2,28));
  }
}

void alterC(bool set) {
  // Sets/clears CPSR bit C depending on set
  if (set) {
    *rf.CPSR = *rf.CPSR | ipow(2,29);
  } else {
    *rf.CPSR = *rf.CPSR & ((ipow(2,32) - 1) - ipow(2,29));
  }
}

void alterZ(bool set) {
  // Sets/clears CPSR bit Z depending on set
  if (set) {
    *rf.CPSR = *rf.CPSR | ipow(2,30);
  } else {
    *rf.CPSR = *rf.CPSR & ((ipow(2,32) - 1) - ipow(2,30));
  }
}

void alterN(bool set) {
  if (set) {
    *rf.CPSR = *rf.CPSR | (-ipow(2,31));
  } else {
    *rf.CPSR = *rf.CPSR & (ipow(2,31)-1);
  }
}

int64_t ipow(int x, int y) { //Confirmed
  // POST: returns x^y cast as an int
  return (int64_t)pow(x,y);
}

int getBit(uint32_t x, int pos) { //Confirmed
  //returns 1 bit value of the bit at position pos of x
  // e.g getBit(10010011, 0) = 1
  return (x & ipow(2,pos)) >> pos;
}

uint32_t getBinarySeg(uint32_t x, uint32_t start, uint32_t length) { //Confirmed
  //PRE: sizeof(x) > start > 0 / length > 0
  //POST: res = int value of binary segment between start and end
  uint32_t acc = ipow(2,start); // an accumulator which will set the positions of the bits with the segment we want to return

  for (int i = 1; i < length; i++) {
    acc += ipow(2,start-i);
  }

  return (x & acc) >> (start - (length - 1));
}

int rotr8(uint8_t x, int n) { //Confirmed
  // PRE: x is an unsigned 8 bit number (note x may be any type with 8 or more bits). n is the number x will be rotated by.
  // POST: rotr8 will return the 8 bit value of x rotated n spaces to the right
  uint8_t a = (x & (ipow(2,n)-1)) << (sizeof(x)*8 - n);
  return (x >> n) | a;
}

int rotr32(uint32_t x, int n) { //Confirmed
  // PRE: x is an unsigned 32 bit number (note x may be any type with 32 or more bits). n is the number x will be rotated by.
  // POST: rotr32 will return the 32 bit value of x rotated n spaces to the right
  uint32_t a = (x & (ipow(2,n)-1)) << (sizeof(x)*8 - n);
  return (x >> n) | a;
}

void testingHelpers(void) { //PASSED
  int x;
  uint32_t ux;
  printf("start testing\n\n");

  printf("Test ipow ====\n");

  x = ipow(2,30);
  printf("ipow(2,30) = %d\n",x);
  printf("expected 1073741824\n");
  x = ipow(2,31);
  ux = ipow(2,31);
  printf("(uint32_t)ipow(2,31) = %u\n",ux);
  printf("expected 2147483648\n");
  x = ipow(2,32);
  printf("(int)ipow(2,32) = %d\n",x);
  printf("expected 0\n");

  printf("====\n\n");

  printf("Test ipow ====\n");

  printf("getBit(5,7) = %d\n",getBit(5,7));
  printf("expected 0\n");
  printf("getBit(69,2) = %d\n",getBit(69,2));
  printf("expected 1\n");
  printf("getBit(78,6) = %d\n",getBit(78,6));
  printf("expected 1\n");
  printf("getBit(ipow(2,31),31) = %d\n",getBit(ipow(2,31),31));
  printf("expected 1\n");
  printf("getBit(ipow(2,35),31) = %d\n",getBit(ipow(2,35),31));
  printf("expected 0\n");
  printf("getBit(78,45) = %d\n",getBit(78,45));
  printf("expected 0\n");

  printf("====\n\n");

  printf("Test getBinarySeg ====\n");

  printf("getBinarySeg(15,3,3) = %d\n", getBinarySeg(15,3,3));
  printf("expected 7\n");
  printf("getBinarySeg(179,7,6) = %d\n", getBinarySeg(179,7,6));
  printf("expected 44\n");
  printf("getBinarySeg(ipow(2,31)+ipow(2,29),3,3) = %d\n", getBinarySeg(ipow(2,31)+ipow(2,29),31,3));
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
  printf("rotr32(ipow(2,31),3) = %u\n", (uint32_t)rotr32(ipow(2,31),3));
  printf("expected %u\n", (uint32_t)ipow(2,28));
  printf("rotr32(13,3) = %u\n", rotr32(13,3));
  printf("expected %u\n", (uint32_t)(ipow(2,31) + ipow(2,29) + 1));

  printf("====\n\n");

  printf("end testing\n");
}

void outputMemReg(void) {
  //outputs the state of the main memory and register file
  // output mem -----------------------
  printf("\nOutput in Little Endian format\n");
  unsigned int addr = 0; //current address
  printf("Main memory --- \n");
  uint32_t pcValue = *rf.PC;
  *rf.PC = 0;
  uint32_t instruction;
  while(addr < MEM16BIT) {
    instruction = fetch(mem);
    if (instruction != 0){
      printf("%X: ",addr);
      outputData(instruction);
    }
    addr += 4; // go to next byte
  }
  printf("---\n\n");

  // reset PC
  *rf.PC = pcValue;

  // Output registers ------------
  printf("Register file --- \n");
  for (int i = 0; i < NUM_GREG; i++) {
    printf("register %d: ", i);
    outputData(rf.reg[i]);
  }
  printf("SP: ");
  outputData(*rf.SP);
  printf("LR: ");
  outputData(*rf.LR);
  printf("PC: ");
  outputData(*rf.PC);
  printf("CPSR: ");
  outputData(*rf.CPSR);
  printf("---\n\n");

}

void outputData(uint32_t i) {
  uint8_t b0,b1,b2,b3;
  uint32_t littleEndian_format = 0;

  b0 = i;// & 0xff);
  b1 = i >> 8;// & 0xff);
  b2 = i >> 16;// & 0xff);
  b3 = i >> 24;// & 0xff);

  littleEndian_format = (littleEndian_format | b0) << 8;
  littleEndian_format = (littleEndian_format | b1) << 8;
  littleEndian_format = (littleEndian_format | b2) << 8;
  littleEndian_format = (littleEndian_format | b3);

  printf("0x%.8x\n", littleEndian_format);

}

void dealloc(void) {
  // Frees all memory locations alloacted during the execution of the program
  free(rf.reg);
  free(mem);
}

void enterC(void) {
  printf("Press enter");
  while (getchar() != '\n');
}
