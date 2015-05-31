#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include "emulate.h"
#include "utils.h"
// #include "tests.h"

FILE *binFile = NULL; // binary file containing instructions
uint8_t *mem = NULL;  // LITTLE ENDIAN Main Memory
RegFile rf;           // sets register file

int main (int argc, char const *argv[]) {
  if (argc != 2) {
    printf("Incorrect number of arguments\n");
    exit(EXIT_FAILURE);
  }

  mem = calloc(MEM16BIT, 1);  // allocates 2^16 bit memory addresses to mem
  clearRegfile();             // sets all registers to 0
  loadFileToMem(argv[1]);     // binary loader: loads file passed through argv
                              // into mem
  int executeResult;          // controls pipeline flow

  int32_t instruction;        // stores the current fetched instruction
  DecodedInst di;             // stores the current decoded instruction

      // PC = 0 before entering loop, an effect of clearRegfile()
  do {
    instruction = fetch(mem);
    do {
      di = decode(instruction);
      instruction = fetch(mem);
      executeResult = execute(di);
    } while(executeResult == EXE_CONTINUE); // continue regular cycle
  } while(executeResult != EXE_HALT);       // fetch again if EXE_BRANCH

  outputMemReg();             // print memory and register contents to screen
  // runAllTests();
  dealloc();                  // frees up allocated memory
  return EXIT_SUCCESS;
}

// Fetch-Decode functions -------------------------

uint32_t fetch(uint8_t *mem){
  // reads and returns 4 byte LITTLE ENDIAN instruction from mem @ PC
  uint32_t instruction = 0;
  for (int i = 3; i >= 0; i--) {
    instruction <<= 8;              // more significant Bs shifted a byte left
    instruction += mem[*rf.PC + i]; // next byte added to instruction
  }
  *rf.PC = *rf.PC + 4;              // inc^ PC
  return instruction;
}

DecodedInst decode(uint32_t instruction) {
  // returns instruction broken into parts useful for execute()
  DecodedInst di;
  di.cond = getBinarySeg(instruction, 31, 4); // cond = inst[28..31]
  di.instType = getInstType(instruction); // only has correct 4 MSBs
  switch(di.instType){
    case DATA_PROC :
        decodeForDataProc(instruction, &di);
        break;
    case MULT :
        decodeForMult(instruction, &di);
        break;
    case DATA_TRANS :
        decodeForDataTrans(instruction, &di);
        break;
    case BRANCH :
        decodeForBranch(instruction, &di);
    case HALT :                           // nothing to decode for HALT
    default :
        break;
  }
  return di;
}

uint8_t getInstType(uint32_t instruction) {
  // returns correct 4 MSB of code for current instruction
  if (instruction == HALT){
    return HALT;
  }
  if(getBit(instruction, 27)){
    return BRANCH;
  }
  if (getBit(instruction, 26)) {
    return DATA_TRANS;
  }
  int multCode = 9;
  if (getBit(instruction, 25) | (getBinarySeg(instruction, 7, 4) != multCode)) {
    // bit 25 == 1 or bits [4..7] != 9
    return DATA_PROC;
  }
  return MULT; //bit 25 == 0; bits [4..7] == 9
}

void decodeForDataProc(uint32_t instruction, DecodedInst *di) {

  // set flag bits: I S x x
  if (getBit(instruction, 25)){
    di->instType += 8;
  }
  if (getBit(instruction, 20)){
    di->instType += 4;
  }

  // set opcode
  di->opcode = getBinarySeg(instruction, 24, 4);

  // set registers
  di->rn = getBinarySeg(instruction, 19, 4);
  di->rd = getBinarySeg(instruction, 15, 4);

  // get operandOffset (operand)
  di->operandOffset = getBinarySeg(instruction, 11, 12);

}

void decodeForMult(uint32_t instruction, DecodedInst *di) {

  // set flag bits: x S A x
  if (getBit(instruction, 20)){
    di->instType += 4;
  }
  if (getBit(instruction, 21)){
    di->instType += 2;
  }

  // set registers
  di->rd = getBinarySeg(instruction, 19, 4);
  di->rn = getBinarySeg(instruction, 15, 4);
  di->rs = getBinarySeg(instruction, 11, 4);
  di->rm = getBinarySeg(instruction, 3, 4);

}

void decodeForDataTrans(uint32_t instruction, DecodedInst *di) {

  // set flag bits: I L P U
  if (getBit(instruction, 25)){
    di->instType += 8;
  }
  if (getBit(instruction, 20)){
    di->instType += 4;
  }
  if (getBit(instruction, 24)){
    di->instType += 2;
  }
  if (getBit(instruction, 23)){
    di->instType = di->instType + 1;
  }

  // set registers
  di->rn = getBinarySeg(instruction, 19, 4);
  di->rd = getBinarySeg(instruction, 15, 4);

  // get operandOffset (offset)
  di->operandOffset = getBinarySeg(instruction, 11, 12);

}

void decodeForBranch(uint32_t instruction, DecodedInst *di) {

  // flag bits: x x x x

  // get operandOffset (signed offset)
  di->operandOffset = getBinarySeg(instruction, 23, 24);// unaltered offset bits

  if (getBit(instruction, 23)) { // offset is negative
    di->operandOffset |= 0xFFF00000;
    // 1s extended to MSB
  }

}
// -----------------------------------------------

// Execute functions -----------------------------

int execute(DecodedInst di) {
  if (di.instType == HALT) {
    return EXE_HALT;
  }

  bool condPass = FALSE;  // condPass will be TRUE iff cond is satisfied
  int res = EXE_CONTINUE; // res will contain the state of the next executed
                          // instruction depending on whether halt or
                          // branch is executed
  switch(di.cond) {
    case EQ : // 0000: Z set / is equal
      condPass = getBit(*rf.CPSR, Zbit);
      break;
    case NE : // 0001: Z clear / not equal
      condPass = !getBit(*rf.CPSR, Zbit);
      break;
    case GE : // 1010: N equals V / greater equal
      condPass = getBit(*rf.CPSR, Nbit) == getBit(*rf.CPSR, Vbit);
      break;
    case LT : // 1011: N not equal V / less
      condPass = getBit(*rf.CPSR, Nbit) != getBit(*rf.CPSR, Vbit);
      break;
    case GT : // 1100: Z clear AND (N = V) / greater than
      condPass = (!getBit(*rf.CPSR, Zbit)) && (getBit(*rf.CPSR, Nbit) ==
                 getBit(*rf.CPSR, Vbit));
      break;
    case LE : // 1101: Z set OR (N != V) / less than
      condPass = getBit(*rf.CPSR, Zbit) ||  (getBit(*rf.CPSR, Nbit) !=
                 getBit(*rf.CPSR, Vbit));
      break;
    case AL : // 1110 always
      condPass = TRUE;
      break;
    default :
      perror("Invalid instruction entered with unknown condition");
      exit(EXIT_FAILURE);
  }

  if (condPass) {

    if ((di.instType & DATA_PROC) != FALSE) {// Data processing
      executeDataProcessing(di.instType, di.opcode, di.rn, di.rd,
                            di.operandOffset);

    } else if ((di.instType & MULT) != FALSE) { // Multiply
      executeMult(di.instType, di.rd, di.rn, di.rs, di.rm);


    } else if ((di.instType & DATA_TRANS) != FALSE) { // Data transfer
      executeSingleDataTransfer(di.instType, di.rn, di.rd, di.operandOffset);


    } else if ((di.instType & BRANCH) != FALSE) { // Branch
      executeBranch(di.operandOffset);
      res = EXE_BRANCH;
    }

  }

  return res;

}

void executeDataProcessing(uint8_t instType, uint8_t opcode, uint8_t rn, uint8_t
                           rd, uint32_t operand) {
  bool i = getBit(instType,3);                // Immediate Operand
  int rotate = getBinarySeg(operand,11,4);    // 4 bit rotate segment if i = 1

  int shiftSeg = getBinarySeg(operand,11,8);  // 8 bit shift segment if i = 0
  int rm = getBinarySeg(operand,3,4);         // 4 bit

  bool s = getBit(instType,2);                // Set condition

  int testRes = 0;                            // result from test operations

  if (i) {                                    // if operand is immediate
    operand = getBinarySeg(operand,7,8);      // operand = Immediate segment
    alterCPSR(getBit(operand, rotate*2 - 1), s, Cbit);

    operand = rotr32(operand, rotate*2);

  } else {                                    // operand is a register
    operand = barrelShift(rf.reg[rm], shiftSeg, s);
  }

  switch(opcode) {
    case AND :
      rf.reg[rd] = rf.reg[rn] & operand;
      setCPSRZN(rf.reg[rd],s);
    break;
    case EOR :
      rf.reg[rd] = rf.reg[rn] ^ operand;
      setCPSRZN(rf.reg[rd],s);
    break;
    case SUB :
      rf.reg[rd] = (int)rf.reg[rn] - (int)operand;

      alterCPSR((int)operand <= (int)rf.reg[rn], s, Cbit); // borrow
      //will occur if subtrahend > minuend

      setCPSRZN(rf.reg[rd],s);
    break;
    case RSB :
      rf.reg[rd] = (int)operand - (int)rf.reg[rn];

      alterCPSR((int)operand >= (int)rf.reg[rn], s, Cbit); // borrow
      //will occur if subtraend > minuend

      setCPSRZN(rf.reg[rd],s);
    break;
    case ADD :
      rf.reg[rd] = (int)rf.reg[rn] + (int)operand;

      alterCPSR((rf.reg[rn] > 0) && (operand > INT_MAX - rf.reg[rn]), s, Cbit);
       // overflow will occur based on this condition
      setCPSRZN(rf.reg[rd],s);
    break;
    case TST :
      testRes = rf.reg[rn] & operand;
      setCPSRZN(testRes,s);
    break;
    case TEQ :
      testRes = rf.reg[rn] ^ operand;
      setCPSRZN(testRes,s);
    break;
    case CMP :
      testRes = rf.reg[rn] - operand;
      alterCPSR((int)operand <= (int)rf.reg[rn], s, Cbit); // borrow
      //will occur if subtrahend > minuend
      setCPSRZN(testRes,s);
    break;
    case ORR :
      rf.reg[rd] = rf.reg[rn] | operand;
      setCPSRZN(testRes,s);
    break;
    case MOV :
      rf.reg[rd] = operand;
      setCPSRZN(rf.reg[rd], s);
    break;
  }

}

void executeMult(uint8_t instType, uint8_t rd, uint8_t rn, uint8_t rs, uint8_t
                 rm) {
  bool a = getBit(instType,1); //Accumulate
  bool s = getBit(instType,2); //set condition

  if (!a) { // Normal multiply
    rf.reg[rd] = rf.reg[rm]*rf.reg[rs];
  } else { // multiply with accumulate
    rf.reg[rd] = rf.reg[rm]*rf.reg[rs] + rf.reg[rn];
  }

  setCPSRZN(rf.reg[rd], s);

}

void executeSingleDataTransfer(uint8_t instType, uint8_t rn, uint8_t rd,
                               uint32_t offset) {
  bool i = getBit(instType,3); // immediate offset
  bool l = getBit(instType,2); // Load/Store
  bool p = getBit(instType,1); // Pre/Post, set = Pre
  bool u = getBit(instType,0); // Up bit

  int shiftSeg = (offset >> 4); // 8 bit shift segment if i = 0
  int rm = getBinarySeg(offset,3,4); // 4 bit

  if (!i) { // if offset is immediate
    offset = getBinarySeg(offset,11,12); // offset = Immediate
  } else {// offset is a register
    offset = barrelShift(rf.reg[rm], shiftSeg, 0);
  }

  int soffset = offset; // signed offset

  if (!u) { // if we are subtracting, soffset will be negative
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
      writewMem(rf.reg[rd], rf.reg[rn]+soffset);
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
// -----------------------------------------------

// Helper functions -----------------------------
void loadFileToMem(char const *file) {
  // Reads bytes from file and inserts them into mem in LITTLE ENDIAN format
  if ((binFile = fopen(file,"r")) == NULL){
    perror("Unable to open file!");
    exit(EXIT_FAILURE);
  }

  fread(mem,1,MEM16BIT,binFile);
  fclose(binFile);
}

uint32_t wMem(uint32_t startAddr) {
  // Returns 32 bit word starting from address startAddr
  // value in BIG ENDIAN
  if (startAddr > MEM16BIT) {
    printf("Error: Out of bounds memory access at address 0x%.8x\n", startAddr);
    return 0;
  }

  uint32_t word = 0;
  for (int i = 0; i < 4; i++) {
    word = word | ((uint32_t)mem[startAddr + i] << (8*i));
  }

  return word;
}

void writewMem(uint32_t value, uint32_t startAddr) {
  //Stores 32 bit word value to address starting from startAddr

  if (startAddr > MEM16BIT - 3) {
    printf("Error: Out of bounds memory access at address 0x%.8x\n", startAddr);
    return;
  }

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

void setCPSRZN(int value, bool trigger) {
  //will set the CPSR Z and N bits depending on value iff trigger = TRUE
  alterCPSR(value == 0, trigger, Zbit);
  alterCPSR(getBit(value,31), trigger, Nbit);
}

void alterCPSR(bool set, bool shouldSet, int nthBit) {
  //Sets/Clears CPSR bits
  if (shouldSet) {
    *rf.CPSR ^= (-set ^ *rf.CPSR) & (1 << nthBit);
  }
}

uint32_t barrelShift(uint32_t value, int shiftSeg, int s) {
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
    case LSL : // logical left
      res = value << shift;
      alterCPSR(getBit(value,sizeof(value)*8 - shift - 1), s, Cbit);
    break;
    case LSR : // logical right
      res = value >> shift;
      alterCPSR(getBit(value, shift - 1), s, Cbit);
    break;
    case ASR : // arithmetic right
      if (getBit(value,31)) { // if value is negative
        res = (value >> shift) | (((1 << (shift+1))-1) << (31-shift));
      } else {
        res = value >> shift;
      }
      alterCPSR(getBit(value, shift - 1), s, Cbit);
    break;
    case ROR : // rotate right
      res = rotr32(value,shift);
      alterCPSR(getBit(value,shift - 1), s, Cbit);
    break;
  }

  return res;

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
  *rf.PC = 0;
  uint32_t instruction;
  while(*rf.PC < MEM16BIT) {
    instruction = fetch(mem);
    if (instruction != 0){
      printf("0x%.8x: ", *rf.PC - 4); // since fetch automatically incr. PC
      outputData(instruction, !isRegister);
    }
  }
}

void outputData(uint32_t i, bool isRegister) {

  if (isRegister) {                       // BIG ENDIAN
    printf("(0x%.8x)\n", i);

  } else  {                               // LITTLE ENDIAN
    uint8_t b0,b1,b2,b3;
    uint32_t little_endian = 0;
    b0 = getBinarySeg(i, 7, 8);           // bits [0..7]
    b1 = getBinarySeg(i, 15, 8);          // bits [8..15]
    b2 = getBinarySeg(i, 23, 8);          // bits [16..23]
    b3 = getBinarySeg(i, 31, 8);          // bits [24..31]
    little_endian = (little_endian | b0) << 8;
    little_endian = (little_endian | b1) << 8;
    little_endian = (little_endian | b2) << 8;
    little_endian = (little_endian | b3);
    printf("0x%.8x\n", little_endian);
  }

}

void dealloc(void) {
  // Frees all memory locations alloacted during the execution of the program
  free(rf.reg);
  free(mem);
}
