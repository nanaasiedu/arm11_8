#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "emulate.h"
#include <limits.h>

// ADD: CPSR bit const / opcode const / shift type const
// ADD SUBTRACTION BORROW LOGIC
// ADD memory and register output

FILE *binFile = NULL; //Binary file containing instructions
uint8_t *mem = NULL;  //Memory
struct regFile rf;    //sets register file

int main (int argc, char const *argv[]) {
  if (argc != 2) {
    printf("Incorrect number of arguments");
    exit(EXIT_FAILURE);
  }

  mem = calloc(MEM16BIT, 1); //allocates 2^16 bit memory addresses to mem

  clearRegfile(); // Sets all registers to 0

  loadFileToMem(argv[1]); //Binary loader: loads file passed through argv into mem

  //testing(); //FOR TESTING PURPOSES

  outputMemReg();
  printf("The program is closing");
  dealloc(); //frees up allocated memory
  return EXIT_SUCCESS;
}

int execute(DecodedInst di) {
  if (di.instType == EXE_HALT) {
    return EXE_HALT;
  }

  bool condPass = FALSE; //condPass will be TRUE iff cond is satisfied
  int res = EXE_CONTINUE; // res will contain the state of the next executed instruction depending on whether halt or branch is executed

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
    if ((di.instType & DATA_PROC) != 0) {// Data processing
      executeDataProcessing(di.instType, di.opcode, di.rn, di.rd, di.operandOffset);

    } else if ((di.instType & MULT) != 0) { // Mult
      executeMult(di.instType, di.rd, di.rn, di.rs, di.rm);

    } else if ((di.instType & DATA_TRANS) != 0) { // Data transfer
      executeSingleDataTransfer(di.instType, di.rn, di.rd, di.operandOffset);

    } else if ((di.instType & BRANCH) != 0) { // Branch
      executeBranch(di.operandOffset);
      res = EXE_BRANCH;
    }
  }

  return res;

}

void executeDataProcessing(uint8_t instType, uint8_t opcode, uint8_t rn, uint8_t rd, uint32_t operand) {
  bool i = getBit(instType,3); // Immediate Operand
  int rotate = (operand >> 7); // rotate segment if i = 1

  int shiftSeg = (operand >> 4); // 8 bit shift segment if i = 0
  int rm = operand & (ipow(2,4)-1); // 4 bit

  bool s = getBit(instType,2); // Set condition

  int testRes = 0; // result from test operations

  if (i) { // if operand is immediate
    operand = operand & (ipow(2,8)-1); // operand = Immediate segment
    operand = rotr8(operand,rotate*2);
  } else {// operand is a register
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

void executeMult(uint8_t instType, uint8_t rd, uint8_t rn, uint8_t rs, uint8_t rm) {
  bool a = getBit(instType,1); //Accumulate
  bool s = getBit(instType,2); //set condition

  if (!a) { // Normal multiply
    rf.reg[rd] = rf.reg[rm]*rf.reg[rs];
  } else { // multiply-accumulate
    rf.reg[rd] = rf.reg[rm]*rf.reg[rs] + rf.reg[rn];
  }

  setCPSRZN(rf.reg[rd],s);

}

void executeSingleDataTransfer(uint8_t instType, uint8_t rn, uint8_t rd, uint32_t offset) {
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

void clearRegfile (void) {
  // allocates 4 bytes for each register
  rf.reg = calloc(NUM_REG,4);
  rf.SP = &rf.reg[13];
  rf.LR = &rf.reg[14];
  rf.PC = &rf.reg[15];
  rf.CPSR = &rf.reg[16];
}

void alterC(bool set) {
  // Sets/clears CPSR bit C depending on set
  if (set) {
    *rf.CPSR = *rf.CPSR | ipow(2,29);
  } else {
    *rf.CPSR = *rf.CPSR & (ipow(2,32) - 1 - ipow(2,29));
  }
}

void alterZ(bool set) {
  // Sets/clears CPSR bit Z depending on set
  if (set) {
    *rf.CPSR = *rf.CPSR | ipow(2,30);
  } else {
    *rf.CPSR = *rf.CPSR & (ipow(2,32) - 1 - ipow(2,30));
  }
}

void alterN(bool set) {
  if (set) {
      *rf.CPSR = *rf.CPSR | ipow(2,31);
  } else {
    *rf.CPSR = *rf.CPSR & (ipow(2,32) - 1 - ipow(2,31));
  }
}

int ipow(int x, int y) {
  // POST: returns x^y cast as an int
  return (int)pow(x,y);
}

int getBit(int x, int pos) {
  //returns bit value of the bit at position pos of x
  // e.g getBit(10010011, 0) = 1
  return (x & ipow(2,pos)) >> pos;
}

int getBinarySeg(int x, int start, int length) {
  //PRE: sizeof(x) > start > 0 / length > 0
  //POST: res = int value of binary segment between start and end
  int acc = ipow(2,start); // an accumulator which will set the positions of the bits with the segment we want to return

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
