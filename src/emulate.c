#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "emulate.h"
#include <limits.h>

// TODO: CPSR bit const / opcode const / shift type const
// TODO: Redo reg mem output
// ADD SUBTRACTION BORROW LOGIC

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

  //outputMemReg();
  printf("The program is closing");
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
  int rotate = (operand >> 7); // rotate segment if i = 1

  int shiftSeg = (operand >> 4); // 8 bit shift segment if i = 0
  int rm = operand & (ipow(2,4)-1); // 4 bit

  bool s = getBit(instType,2); // Set condition

  int testRes = 0; // result from test operations

  if (i) { // if operand is immediate
    operand = operand & (ipow(2,8)-1); // operand = Immediate segment
    operand = rotr8(operand,rotate*2);
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
      rf.reg[rd] = rf.reg[rn] - operand;
      setCPSRZN(rf.reg[rd],s);
    break;
    case 3: // rsb
      rf.reg[rd] = operand - rf.reg[rn];
      setCPSRZN(rf.reg[rd],s);
    break;
    case 4: // add
      rf.reg[rd] = rf.reg[rn] + operand;

      alterC((rf.reg[rn] > 0) && (operand > INT_MAX - rf.reg[rn])); // overflow will occur based on this condition
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

uint32_t barrelShift(uint32_t value, int shiftSeg, int s) {
  //POST: return shifted value of rm to operand
  bool shiftop = shiftSeg & 1; // 1 bit shiftop = shift option. selects whether shift amount is by integer or Rs
  int shiftType = shiftSeg & 6; //2 bit
  int rs = shiftSeg >> 4; // 4 bit
  int conint = shiftSeg >> 3; // 4 bit
  uint32_t res = 0; // result
  int shift; // value to shift by

  if (shiftop) { // if shiftseg is in reg rs mode
    shift = rf.reg[rs] & (ipow(2,8) - 1);
  } else { // if shiftseg is in constant int mode
    shift = conint;
  }

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
        res = (value >> shift) && (ipow(2,32)-1 -(ipow(2,30-shift+1)-1));
      } else {
        res = (value >> shift);
      }
      alterC(getBit(value,shift - 1));
    break;
    case 3: // rotate right
      rotr32(value,shift);
      alterC(getBit(value,shift - 1));
    break;
  }

  return res;

}

void setCPSRZN(int value, bool trigger) {
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
  rf.CPSR += offset;
}

void testingExecute(void) {
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

  printf("Test 1 ====\n");
  setCPSRZN(0,1);
  printf(
  "CPSR: "BYTETOBINARYPATTERN"\n",
  BYTETOBINARY(getBinarySeg(*rf.CPSR,4,31))
  );
  printf("Expected: CPSR: 0100\n");
  printf("====\n\n");

  printf("end testing\n");
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

int64_t ipow(int x, int y) {
  // POST: returns x^y cast as an int
  return (int64_t)pow(x,y);
}

int getBit(uint32_t x, int pos) {
  //returns 1 bit value of the bit at position pos of x
  // e.g getBit(10010011, 0) = 1
  return (x & ipow(2,pos)) >> pos;
}

uint32_t getBinarySeg(uint32_t x, uint32_t start, uint32_t length) {
  //PRE: sizeof(x) > start > 0 / length > 0
  //POST: res = int value of binary segment between start and end
  uint32_t acc = ipow(2,start); // an accumulator which will set the positions of the bits with the segment we want to return

  for (int i = 1; i < length; i++) {
    acc += ipow(2,start-i);
  }

  return (x & acc) >> (start - (length - 1));
}

int rotr8(uint8_t x, int n) {
  // PRE: x is an unsigned 8 bit number (note x may be any type with 8 or more bits). n is the number x will be rotated by.
  // POST: rotr8 will return the 8 bit value of x rotated n spaces to the right
  uint8_t a = (x & (ipow(2,n)-1)) << (sizeof(x)*8 - n);
  return (x >> n) | a;
}

int rotr32(uint32_t x, int n) {
  // PRE: x is an unsigned 32 bit number (note x may be any type with 32 or more bits). n is the number x will be rotated by.
  // POST: rotr32 will return the 32 bit value of x rotated n spaces to the right
  uint32_t a = (x & (ipow(2,n)-1)) << (sizeof(x)*8 - n);
  return (x >> n) | a;
}

void testingHelpers(void) {
  //int x;
  printf("start testing\n\n");

  printf("Test 1 ====\n");

  printf("====\n\n");

  printf("end testing\n");
}

void outputMemReg(void) {
  //outputs the state of the main memory and register file
  // output mem -----------------------
  uint8_t addr = 0; //current address
  printf("Main memory --- \n");

  while(addr < MEM16BIT) {
    printf("%X: ",addr);

    for (int i = 0; i < 4; i++) {
      printf(BYTETOBINARYPATTERN" ", BYTETOBINARY(mem[addr+i]));
    }

    printf("\n");
    addr += 4; // go to next byte

    if (mem[addr] == 0 && mem[addr+1] == 0 && mem[addr+2] == 0 && mem[addr+3] == 0) {
      break; // we have reached a halt intruction
    }
  }

  printf("---\n\n");

  // Output registers ------------
  printf("Register file --- \n");
  for (int i = 0; i < NUM_GREG; i++) {
    printf(
    "register %d: "BYTETOBINARYPATTERN" "
    BYTETOBINARYPATTERN" "BYTETOBINARYPATTERN"  "
    BYTETOBINARYPATTERN"\n", i,
    BYTETOBINARY(getBinarySeg(rf.reg[i],31,8)),
    BYTETOBINARY(getBinarySeg(rf.reg[i],23,8)),
    BYTETOBINARY(getBinarySeg(rf.reg[i],15,8)),
    BYTETOBINARY(getBinarySeg(rf.reg[i],7,8))
    );
  }

  printf(
  "SP: "BYTETOBINARYPATTERN" "
  BYTETOBINARYPATTERN" "BYTETOBINARYPATTERN"  "
  BYTETOBINARYPATTERN"\n",
  BYTETOBINARY(getBinarySeg(*rf.SP,31,8)),
  BYTETOBINARY(getBinarySeg(*rf.SP,23,8)),
  BYTETOBINARY(getBinarySeg(*rf.SP,15,8)),
  BYTETOBINARY(getBinarySeg(*rf.SP,7,8))
  );

  printf(
  "LR: "BYTETOBINARYPATTERN" "
  BYTETOBINARYPATTERN" "BYTETOBINARYPATTERN"  "
  BYTETOBINARYPATTERN"\n",
  BYTETOBINARY(getBinarySeg(*rf.LR,31,8)),
  BYTETOBINARY(getBinarySeg(*rf.LR,23,8)),
  BYTETOBINARY(getBinarySeg(*rf.LR,15,8)),
  BYTETOBINARY(getBinarySeg(*rf.LR,7,8))
  );

  printf(
  "PC: "BYTETOBINARYPATTERN" "
  BYTETOBINARYPATTERN" "BYTETOBINARYPATTERN"  "
  BYTETOBINARYPATTERN"\n",
  BYTETOBINARY(getBinarySeg(*rf.PC,31,8)),
  BYTETOBINARY(getBinarySeg(*rf.PC,23,8)),
  BYTETOBINARY(getBinarySeg(*rf.PC,15,8)),
  BYTETOBINARY(getBinarySeg(*rf.PC,7,8))
  );

  printf(
  "CPSR: "BYTETOBINARYPATTERN" "
  BYTETOBINARYPATTERN" "BYTETOBINARYPATTERN"  "
  BYTETOBINARYPATTERN"\n",
  BYTETOBINARY(getBinarySeg(*rf.CPSR,31,8)),
  BYTETOBINARY(getBinarySeg(*rf.CPSR,23,8)),
  BYTETOBINARY(getBinarySeg(*rf.CPSR,15,8)),
  BYTETOBINARY(getBinarySeg(*rf.CPSR,7,8))
  );

  printf("---\n\n");

}

/*
void printSpecialReg(uint32_t value, char *message) {
  // Takes in a register value and prints it out in binary form
  printf(
  "%s "BYTETOBINARYPATTERN" "
  BYTETOBINARYPATTERN" "BYTETOBINARYPATTERN"  "
  BYTETOBINARYPATTERN"\n", *message
  BYTETOBINARY(getBinarySeg(value,31,8)),
  BYTETOBINARY(getBinarySeg(value,23,8)),
  BYTETOBINARY(getBinarySeg(value,15,8)),
  BYTETOBINARY(getBinarySeg(value,7,8))
  );
}
*/

void dealloc(void) {
  // Frees all memory locations alloacted during the execution of the program
  free(rf.reg);
  free(mem);
}

void enterC(void) {
  printf("Press enter");
  while (getchar() != '\n');
}
