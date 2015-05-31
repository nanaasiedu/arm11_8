#ifndef EMULATE_H
#define EMULATE_H

#include <stdlib.h>
#include <stdint.h>
#include "helpers/definitions.h"

// execute return values
const int EXE_HALT = 0;
const int EXE_CONTINUE = -1;
const int EXE_BRANCH = 1;

typedef struct { // stores decoded instrustion in useful format
  uint8_t cond;
  uint8_t instType;
  uint8_t opcode;
  uint8_t rn;
  uint8_t rd;
  uint8_t rm;
  uint8_t rs;
  int32_t operandOffset;
} DecodedInst;

typedef struct {
  uint32_t *reg; // registers 0-12 are general purpose.
  uint32_t *SP;
  uint32_t *LR;
  uint32_t *PC;
  uint32_t *CPSR;
} RegFile;

const uint32_t MEM16BIT = 65536;        // 2^16
const uint32_t MAX_UINT32 = 0xFFFFFFFF; // 32 1s
const int NUM_REG = 17;                 // number of registers
const int NUM_GREG = 13;                // number of general purpose registers
const int Nbit = 31;                    // position of status bits in CPSR
const int Zbit = 30;
const int Cbit = 29;
const int Vbit = 28;

const int EXE_HALT = 0;
const int EXE_CONTINUE = -1;
const int EXE_BRANCH = 1;

// fetch-decode functions --
uint32_t fetch(uint8_t *mem);
DecodedInst decode(uint32_t instruction);
uint8_t getInstType(uint32_t instruction);
void decodeForDataProc(uint32_t instruction, DecodedInst *di);
void decodeForMult(uint32_t instruction, DecodedInst *di);
void decodeForDataTrans(uint32_t instruction, DecodedInst *di);
void decodeForBranch(uint32_t instruction, DecodedInst *di);
// --

// execute functions --
int execute(DecodedInst di);
void executeDataProcessing(uint8_t instType, uint8_t opcode, uint8_t Rn, uint8_t Rd, uint32_t operand);
void executeMult(uint8_t instType, uint8_t rd, uint8_t rn, uint8_t rs, uint8_t rm);
void executeSingleDataTransfer(uint8_t instType, uint8_t rn, uint8_t rd, uint32_t offset);
void executeBranch(int offset);
// --

// helper functions --
void loadFileToMem(char const *file);
uint32_t wMem(uint32_t startAddr);
void writewMem(uint32_t value, uint32_t startAddr);
void clearRegfile (void);
void setCPSRZN(int value, int trigger);
void alterCPSR(bool set, bool shouldSet, int nthBit);
uint32_t barrelShift(uint32_t valu, int shiftSeg, int s);
void outputMemReg(void);
void outputData(uint32_t i, bool isRegister);
void dealloc(void);
//--

#endif /* end of include guard: EMULATE_H */
