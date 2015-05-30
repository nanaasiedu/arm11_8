#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include "emulate.h"
// #include "tests.h"

// TODO: CPSR bit const / opcode const / shift type const
// TODO: CPSR Fix

FILE *binFile = NULL; //Binary file containing instructions
uint8_t *mem = NULL;  // LITTLE ENDIAN Main Memory
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

  int executeResult;
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

  outputMemReg();

  //runAllTests();

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
  di.cond = getBinarySeg(instruction, 31, 4);
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
  if (di.instType == HALT) {
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
      res = EXE_BRANCH;
    }

  }
  return res;

}

void executeDataProcessing(uint8_t instType, uint8_t opcode, uint8_t rn, uint8_t
                           rd, uint32_t operand) {//confirmed
  bool i = getBit(instType,3); // Immediate Operand
  int rotate = getBinarySeg(operand,11,4); // 4 bit rotate segment if i = 1

  int shiftSeg = getBinarySeg(operand,11,8); // 8 bit shift segment if i = 0
  int rm = getBinarySeg(operand,3,4); // 4 bit

  bool s = getBit(instType,2); // Set condition

  int testRes = 0; // result from test operations

  if (i) { // if operand is immediate
    operand = getBinarySeg(operand,7,8); // operand = Immediate segment
    alterCPSR(getBit(operand, rotate*2 - 1), s, Cbit);

    operand = rotr32(operand, rotate*2);

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

      alterCPSR((int)operand <= (int)rf.reg[rn], s, Cbit); // borrow
      //will occur if subtraend > minuend

      setCPSRZN(rf.reg[rd],s);
    break;
    case 3: // rsb
      rf.reg[rd] = (int)operand - (int)rf.reg[rn];

      alterCPSR((int)operand >= (int)rf.reg[rn], s, Cbit); // borrow
        //will occur if subtraend > minuend

      setCPSRZN(rf.reg[rd],s);
    break;
    case 4: // add
      rf.reg[rd] = (int)rf.reg[rn] + (int)operand;

      alterCPSR((rf.reg[rn] > 0) && (operand > INT_MAX - rf.reg[rn]), s, Cbit);
       // overflow will occur based on this condition
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
      alterCPSR((int)operand <= (int)rf.reg[rn], s, Cbit); // borrow
      //will occur if subtraend > minuend
      setCPSRZN(testRes,s);
    break;
    case 12: // orr
      rf.reg[rd] = rf.reg[rn] | operand;
      setCPSRZN(testRes,s);
    break;
    case 13: // mov
      rf.reg[rd] = operand;
      setCPSRZN(rf.reg[rd], s);
    break;
  }

}

uint32_t barrelShift(uint32_t value, int shiftSeg, int s) { //confirmed
  //POST: return shifted value of rm to operand
  bool shiftop = getBit(shiftSeg,0); // 1 bit shiftop = shift option. selects whether shift amount is by integer or Rs
  int shiftType = getBinarySeg(shiftSeg,2,2); //2 bits
  int rs = getBinarySeg(shiftSeg,7,4); // 4 bits
  int conint = getBinarySeg(shiftSeg,7,5); // 5 bits
  uint32_t res = 0; // result
  int shift; // value to shift by

  if (shiftop) { // if shiftseg is in reg rs mode
    shift = rf.reg[rs] & 0xff;
  } else { // if shiftseg is in constant int mode
    shift = conint;
  }

  switch(shiftType) {
    case 0: // logical left
      res = value << shift;
      alterCPSR(getBit(value,sizeof(value)*8 - shift - 1), s, Cbit);
    break;
    case 1: // logical right
      res = value >> shift;
      alterCPSR(getBit(value, shift - 1), s, Cbit);
    break;
    case 2: // arithmetic right
      if (getBit(value,31)) { // if value is negative
        res = (value >> shift) | (((1 << (shift+1))-1) << (31-shift));
      } else {
        res = value >> shift;
      }
      alterCPSR(getBit(value, shift - 1), s, Cbit);
    break;
    case 3: // rotate right
      res = rotr32(value,shift);
      alterCPSR(getBit(value,shift - 1), s, Cbit);
    break;
  }
  //alterCPSR(res, s, Cbit);
  return res;

}

void setCPSRZN(int value, bool trigger) { //Confirmed
  //will set the CPSR Z and N bits depending on value
  alterCPSR(value == 0, trigger, Zbit);
  alterCPSR(getBit(value,31), trigger, Nbit);//ALTERED
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

  setCPSRZN(rf.reg[rd], s);

}

void executeSingleDataTransfer(uint8_t instType, uint8_t rn, uint8_t rd,
                               uint32_t offset) {
  bool i = getBit(instType, 3); // immediate offset
  bool l = getBit(instType, 2); // Load/Store
  bool p = getBit(instType, 1); // Pre/Post, set = Pre
  bool u = getBit(instType, 0); // Up bit

  int shiftSeg = (offset >> 4); // 8 bit shift segment if i = 0
  int rm = getBinarySeg(offset,3,4); // 4 bit

  if (!i) { // if offset is immediate
    offset = getBinarySeg(offset,11,12); // offset = Immediate
  } else {// offset is a register
    offset = barrelShift(rf.reg[rm], shiftSeg, 0);
  }

  int soffset = offset; // signed offset

  if (!u) { // if we are subtracting soffset will be negative
    soffset = -soffset;
  }

  if (p) { //pre-indexing
    if (l) { //load
      if(rn == 15) {
        rf.reg[rd] = wMem(rf.reg[rn]+soffset);
      } else {
        rf.reg[rd] = wMem(rf.reg[rn]+soffset);
      }
    } else { //store
      writewMem(rf.reg[rd], rf.reg[rn+soffset]);
    }

  } else { //post-indexing
    if (l) { //load
      rf.reg[rd] = wMem(rf.reg[rn]);
    } else { //store
      writewMem(rf.reg[rd],rf.reg[rn]);
    }

    rf.reg[rn] = rf.reg[rn] + soffset;
  }

}

void executeBranch(int offset) {
  *rf.PC += offset << 2;
}

void loadFileToMem(char const *file) {
  // Reads bytes from file and inserts them into the mem array
  if ((binFile = fopen(file,"r")) == NULL){
    perror("Unable to open file!");
    exit(EXIT_FAILURE);
  }

  fread(mem,1,MEM16BIT,binFile);
}

uint32_t wMem(uint16_t startAddr) { //confirmed
  // Returns 32 bit word starting from addr startAddr
  uint32_t word = 0;
  for (int i = 0; i < 4; i++) {
    word = word | ((uint32_t)mem[startAddr + i] << (8*i));
  }

  return word;
}

void writewMem(uint32_t value, uint16_t startAddr) { //confirmed
  // Stores 32 bit word starting from addr startAddr

  for (int i = 0; i < 4; i++) {
    mem[startAddr+i] = getBinarySeg(value,8*(i+1) - 1 ,8);
  }

}

void clearRegfile (void) {
  // allocates 4 bytes for each register
  rf.reg = calloc(NUM_REG,4);
  rf.SP = &rf.reg[13];
  rf.LR = &rf.reg[14];
  rf.PC = &rf.reg[15];
  rf.CPSR = &rf.reg[16];
}

void alterCPSR(bool set, bool shouldSet, int nthBit) { //confirmed
  if (shouldSet) {
    *rf.CPSR ^= (-set ^ *rf.CPSR) & (1 << nthBit);
  }
}

int getBit(uint32_t x, int pos) { //Confirmed
  //returns 1 bit value of the bit at position pos of x
  // e.g getBit(10010011, 0) = 1
  return (x >> pos) & 1;
}

uint32_t getBinarySeg(uint32_t x, uint32_t start, uint32_t length) { //Confirmed
  //PRE: sizeof(x) > start > 0 / length > 0
  //POST: res = int value of binary segment between start and end
  long mask = (1 << length) - 1;
  return (x >> (start-length+1)) & mask;
}

int rotr8(uint8_t x, int n) { //Confirmed
  // PRE: x is an unsigned 8 bit number (note x may be any type with 8 or more bits). n is the number x will be rotated by.
  // POST: rotr8 will return the 8 bit value of x rotated n spaces to the right
  uint8_t a = (x & ((1 << n)-1)) << (sizeof(x)*8 - n);
  return (x >> n) | a;
}

int rotr32(uint32_t x, int n) { //Confirmed
  // PRE: x is an unsigned 32 bit number (note x may be any type with 32 or more bits). n is the number x will be rotated by.
  // POST: rotr32 will return the 32 bit value of x rotated n spaces to the right
  uint32_t a = (x & ((1 << n)-1)) << (sizeof(x)*8 - n);
  return (x >> n) | a;
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
  printf("expected %u\n", (uint32_t)(1 << 30) + (uint32_t)(1 << 29) + 256 + 3);
  printf("====\n\n");

  printf("Test writewMem ====\n");
  writewMem((uint32_t)(1 << 30) + 85,12);
  printf("wMem[12] = %u\n", wMem(12));
  printf("expected %u\n", (uint32_t)(1 << 30) + 85);

  writewMem((uint32_t)(1 << 30) + (uint32_t)(1 << 29) + 256 + 3,80);
  printf("wMem[80] = %u\n", wMem(80));
  printf("expected %u\n", (uint32_t)(1 << 30) + (uint32_t)(1 << 29) + 256 + 3);
  printf("====\n\n");

  printf("end testing\n");
}

void outputMemReg(void) {
  //outputs the state of the main memory and register file
  bool isRegister = TRUE;
  // Output registers ------------
  printf("Registers:\n");
  for (int i = 0; i < NUM_GREG; i++) {
    printf("$%-3d: %10d ", i, rf.reg[i]);
    outputData(rf.reg[i], isRegister);
  }
  printf("PC  : %10d ", *rf.PC);
  outputData(*rf.PC, isRegister);
  printf("CPSR: %10d ", *rf.CPSR);
  outputData(*rf.CPSR, isRegister);

  // output mem -----------------------
  printf("Non-zero memory:\n");
  uint32_t pcValue = *rf.PC;
  *rf.PC = 0;
  uint32_t instruction;
  while(*rf.PC < MEM16BIT) {
    instruction = fetch(mem);
    if (instruction != 0){
      printf("0x%.8x: ", *rf.PC - 4); // since fetch automatically inc^ PC
      outputData(instruction, !isRegister);
    }
  }

  // reset PC
  *rf.PC = pcValue;

}

void outputData(uint32_t i, bool isRegister) {
  uint8_t b0,b1,b2,b3;
  uint32_t hexFormat = 0;
  b0 = i;// & 0xff);
  b1 = i >> 8;// & 0xff);
  b2 = i >> 16;// & 0xff);
  b3 = i >> 24;// & 0xff);

  if (isRegister) {
    hexFormat = (hexFormat | b3) << 8;
    hexFormat = (hexFormat | b2) << 8;
    hexFormat = (hexFormat | b1) << 8;
    hexFormat = (hexFormat | b0);
    printf("(0x%.8x)\n", hexFormat);
  } else  {
    hexFormat = (hexFormat | b0) << 8;
    hexFormat = (hexFormat | b1) << 8;
    hexFormat = (hexFormat | b2) << 8;
    hexFormat = (hexFormat | b3);
    printf("0x%.8x\n", hexFormat);
  }

}

void dealloc(void) {
  // Frees all memory locations alloacted during the execution of the program
  free(rf.reg);
  free(mem);
}
